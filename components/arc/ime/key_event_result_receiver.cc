// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/arc/ime/key_event_result_receiver.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/logging.h"
#include "base/metrics/histogram_functions.h"
#include "base/threading/thread_task_runner_handle.h"
#include "base/time/time.h"
#include "components/arc/ime/arc_ime_util.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/event.h"
#include "ui/events/event_dispatcher.h"

namespace arc {

namespace {

// TODO(b/183573525): This timeout is chosen tentatively. We should adjust the
// value after collecting the latency metrics.
constexpr base::TimeDelta kKeyEventDoneCallbackTimeout =
    base::Milliseconds(300);
constexpr base::TimeDelta kKeyEventLatencyMin = base::Milliseconds(1);
constexpr base::TimeDelta kKeyEventLatencyMax = base::Milliseconds(350);

constexpr char kImeLatencyHistogramName[] = "Arc.ChromeOsImeLatency";

}  // namespace

KeyEventResultReceiver::KeyEventResultReceiver() = default;

KeyEventResultReceiver::~KeyEventResultReceiver() = default;

bool KeyEventResultReceiver::DispatchKeyEventPostIME(ui::KeyEvent* event) {
  // This method is called by `ui::InputMethodAsh` when IME finishes
  // handling a key event coming from |ArcImeService::SendKeyEvent()|. If the
  // key event seems not to be consumed by IME, it's sent back to ARC to give it
  // to the focused View in ARC side.

  DLOG_IF(WARNING, !callback_)
      << "DispatchKeyEventPostIME is called without setting a callback";

  if (event->stopped_propagation()) {
    // The host IME wants to stop propagation of the event.
    RunCallbackIfNeeded(true);
    return true;
  }

  if (event->key_code() == ui::VKEY_PROCESSKEY) {
    // This event is consumed by IME.
    RunCallbackIfNeeded(true);
    return true;
  }

  if (!expected_key_event_ || event->type() != expected_key_event_->type() ||
      event->key_code() != expected_key_event_->key_code() ||
      event->time_stamp() != expected_key_event_->time_stamp()) {
    // Another key event was dispatched from IME before the expected key event.
    return false;
  }

  if (!event->GetCharacter()) {
    // The event has no character and the host IME doesn't consume it.
    RunCallbackIfNeeded(false);
    return true;
  }

  // Check whether the event is sent via |InsertChar()| later.
  // If it's sent via |InsertChar()|, let the proxy IME stop sending it to an
  // app.
  const bool sent_by_insert_char =
      !IsControlChar(event) && !ui::IsSystemKeyModifier(event->flags());
  RunCallbackIfNeeded(sent_by_insert_char);
  return true;
}

void KeyEventResultReceiver::SetCallback(KeyEventDoneCallback callback,
                                         const ui::KeyEvent* event) {
  // Cancel the obsolete callback if exist.
  RunCallbackIfNeeded(false);
  callback_ = std::move(callback);
  callback_set_time_ = base::TimeTicks::Now();
  expected_key_event_ = *event;
  // Start expiring timer for the callback.
  base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
      FROM_HERE,
      base::BindOnce(&KeyEventResultReceiver::ExpireCallback,
                     weak_ptr_factory_.GetWeakPtr()),
      kKeyEventDoneCallbackTimeout);
}

bool KeyEventResultReceiver::HasCallback() const {
  return !callback_.is_null();
}

void KeyEventResultReceiver::ExpireCallback() {
  weak_ptr_factory_.InvalidateWeakPtrs();
  RunCallbackIfNeeded(false);
}

void KeyEventResultReceiver::RunCallbackIfNeeded(bool result) {
  if (callback_) {
    weak_ptr_factory_.InvalidateWeakPtrs();
    RecordImeLatency();
    std::move(callback_).Run(result);
    callback_.Reset();
  }
}

void KeyEventResultReceiver::RecordImeLatency() {
  base::UmaHistogramCustomTimes(
      kImeLatencyHistogramName,
      base::TimeTicks::Now() - callback_set_time_.value(), kKeyEventLatencyMin,
      kKeyEventLatencyMax, 50);
  callback_set_time_ = absl::nullopt;
}

}  // namespace arc
