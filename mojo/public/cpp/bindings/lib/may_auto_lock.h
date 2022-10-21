// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_PUBLIC_CPP_BINDINGS_LIB_MAY_AUTO_LOCK_H_
#define MOJO_PUBLIC_CPP_BINDINGS_LIB_MAY_AUTO_LOCK_H_

#include "base/component_export.h"
#include "base/memory/raw_ptr.h"
#include "base/synchronization/lock.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace mojo {
namespace internal {

// Similar to base::AutoLock, except that it does nothing if |lock| passed into
// the constructor is null.
class COMPONENT_EXPORT(MOJO_CPP_BINDINGS_BASE) MayAutoLock {
 public:
  explicit MayAutoLock(absl::optional<base::Lock>* lock)
      : lock_(lock->has_value() ? &lock->value() : nullptr) {
    if (lock_)
      lock_->Acquire();
  }

  MayAutoLock(const MayAutoLock&) = delete;
  MayAutoLock& operator=(const MayAutoLock&) = delete;

  ~MayAutoLock() {
    if (lock_) {
      lock_->AssertAcquired();
      lock_->Release();
    }
  }

 private:
  raw_ptr<base::Lock> lock_;
};

// Similar to base::AutoUnlock, except that it does nothing if |lock| passed
// into the constructor is null.
class COMPONENT_EXPORT(MOJO_CPP_BINDINGS_BASE) MayAutoUnlock {
 public:
  explicit MayAutoUnlock(absl::optional<base::Lock>* lock)
      : lock_(lock->has_value() ? &lock->value() : nullptr) {
    if (lock_) {
      lock_->AssertAcquired();
      lock_->Release();
    }
  }

  MayAutoUnlock(const MayAutoUnlock&) = delete;
  MayAutoUnlock& operator=(const MayAutoUnlock&) = delete;

  ~MayAutoUnlock() {
    if (lock_)
      lock_->Acquire();
  }

 private:
  raw_ptr<base::Lock> lock_;
};

}  // namespace internal
}  // namespace mojo

#endif  // MOJO_PUBLIC_CPP_BINDINGS_LIB_MAY_AUTO_LOCK_H_