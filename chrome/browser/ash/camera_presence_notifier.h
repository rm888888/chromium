// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_ASH_CAMERA_PRESENCE_NOTIFIER_H_
#define CHROME_BROWSER_ASH_CAMERA_PRESENCE_NOTIFIER_H_

#include "base/macros.h"
#include "base/memory/singleton.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/timer/timer.h"

namespace ash {

// Camera presence status dispatcher.
class CameraPresenceNotifier {
 public:
  class Observer {
   public:
    virtual void OnCameraPresenceCheckDone(bool is_camera_present) = 0;
   protected:
    virtual ~Observer() {}
  };

  static CameraPresenceNotifier* GetInstance();

  CameraPresenceNotifier(const CameraPresenceNotifier&) = delete;
  CameraPresenceNotifier& operator=(const CameraPresenceNotifier&) = delete;

  void AddObserver(CameraPresenceNotifier::Observer* observer);
  void RemoveObserver(CameraPresenceNotifier::Observer* observer);

 private:
  friend struct base::DefaultSingletonTraits<CameraPresenceNotifier>;
  CameraPresenceNotifier();
  ~CameraPresenceNotifier();

  void CheckCameraPresence();

  // Sends out a camera presence status notification.
  void OnCameraPresenceCheckDone();

  bool camera_present_on_last_check_;

  // Timer for camera check cycle.
  base::RepeatingTimer camera_check_timer_;

  base::ObserverList<Observer>::Unchecked observers_;

  base::WeakPtrFactory<CameraPresenceNotifier> weak_factory_{this};
};

}  // namespace ash

// TODO(https://crbug.com/1164001): remove after the migration is finished.
namespace chromeos {
using ::ash::CameraPresenceNotifier;
}

#endif  // CHROME_BROWSER_ASH_CAMERA_PRESENCE_NOTIFIER_H_
