// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_SETTINGS_CHROMEOS_CALCULATOR_SIZE_CALCULATOR_TEST_API_H_
#define CHROME_BROWSER_UI_WEBUI_SETTINGS_CHROMEOS_CALCULATOR_SIZE_CALCULATOR_TEST_API_H_

#include <utility>

#include "chrome/browser/ui/webui/settings/chromeos/calculator/size_calculator.h"
#include "chrome/browser/ui/webui/settings/chromeos/device_storage_handler.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace chromeos {
namespace settings {
namespace calculator {

class TotalDiskSpaceTestAPI {
 public:
  TotalDiskSpaceTestAPI(StorageHandler* handler,
                        TotalDiskSpaceCalculator* total_disk_space_calculator) {
    total_disk_space_calculator_ = total_disk_space_calculator;
    total_disk_space_calculator_->AddObserver(handler);
  }

  void StartCalculation() { total_disk_space_calculator_->StartCalculation(); }

  void SimulateOnGetRootDeviceSize(absl::optional<uint64_t> reply) {
    total_disk_space_calculator_->OnGetRootDeviceSize(reply);
  }

 private:
  TotalDiskSpaceCalculator* total_disk_space_calculator_;
};

class FreeDiskSpaceTestAPI {
 public:
  FreeDiskSpaceTestAPI(StorageHandler* handler,
                       FreeDiskSpaceCalculator* free_disk_space_calculator) {
    free_disk_space_calculator_ = free_disk_space_calculator;
    free_disk_space_calculator_->AddObserver(handler);
  }

  void StartCalculation() { free_disk_space_calculator_->StartCalculation(); }

  void SimulateOnGetFreeDiskSpace(int64_t* available_bytes) {
    free_disk_space_calculator_->OnGetFreeDiskSpace(available_bytes);
  }

 private:
  FreeDiskSpaceCalculator* free_disk_space_calculator_;
};

class MyFilesSizeTestAPI {
 public:
  MyFilesSizeTestAPI(StorageHandler* handler,
                     MyFilesSizeCalculator* my_files_size_calculator) {
    my_files_size_calculator_ = my_files_size_calculator;
    my_files_size_calculator_->AddObserver(handler);
  }

  void StartCalculation() { my_files_size_calculator_->StartCalculation(); }

  void SimulateOnGetTotalBytes(int64_t total_bytes) {
    my_files_size_calculator_->NotifySizeCalculated(total_bytes);
  }

 private:
  MyFilesSizeCalculator* my_files_size_calculator_;
};

class BrowsingDataSizeTestAPI {
 public:
  BrowsingDataSizeTestAPI(
      StorageHandler* handler,
      BrowsingDataSizeCalculator* browsing_data_size_calculator) {
    browsing_data_size_calculator_ = browsing_data_size_calculator;
    browsing_data_size_calculator_->AddObserver(handler);
  }

  void StartCalculation() {
    browsing_data_size_calculator_->StartCalculation();
  }

  void SimulateOnGetBrowsingDataSize(bool is_site_data, int64_t size) {
    browsing_data_size_calculator_->OnGetBrowsingDataSize(is_site_data, size);
  }

 private:
  BrowsingDataSizeCalculator* browsing_data_size_calculator_;
};

class AppsSizeTestAPI {
 public:
  AppsSizeTestAPI(StorageHandler* handler,
                  AppsSizeCalculator* apps_size_calculator) {
    apps_size_calculator_ = apps_size_calculator;
    apps_size_calculator_->AddObserver(handler);
  }

  void StartCalculation() { apps_size_calculator_->StartCalculation(); }

  void SimulateOnGetAppsSize(int64_t total_bytes) {
    apps_size_calculator_->OnGetAppsSize(total_bytes);
  }

  void SimulateOnGetAndroidAppsSize(bool succeeded,
                                    uint64_t total_code_bytes,
                                    uint64_t total_data_bytes,
                                    uint64_t total_cache_bytes) {
    arc::mojom::ApplicationsSizePtr result(
        ::arc::mojom::ApplicationsSize::New());
    result->total_code_bytes = total_code_bytes;
    result->total_data_bytes = total_data_bytes;
    result->total_cache_bytes = total_cache_bytes;
    apps_size_calculator_->OnGetAndroidAppsSize(succeeded, std::move(result));
  }

 private:
  AppsSizeCalculator* apps_size_calculator_;
};

class CrostiniSizeTestAPI {
 public:
  CrostiniSizeTestAPI(StorageHandler* handler,
                      CrostiniSizeCalculator* crostini_size_calculator) {
    crostini_size_calculator_ = crostini_size_calculator;
    crostini_size_calculator_->AddObserver(handler);
  }

  void StartCalculation() { crostini_size_calculator_->StartCalculation(); }

  void SimulateOnGetCrostiniSize(int64_t size) {
    crostini_size_calculator_->OnGetCrostiniSize(
        crostini::CrostiniResult::SUCCESS, size);
  }

 private:
  CrostiniSizeCalculator* crostini_size_calculator_;
};

class OtherUsersSizeTestAPI {
 public:
  OtherUsersSizeTestAPI(StorageHandler* handler,
                        OtherUsersSizeCalculator* other_users_size_calculator) {
    other_users_size_calculator_ = other_users_size_calculator;
    other_users_size_calculator_->AddObserver(handler);
  }

  void StartCalculation() { other_users_size_calculator_->StartCalculation(); }

  void InitializeOtherUserSize(int user_count) {
    // When calling OnGetOtherUserSize, a callback is fired when
    // user_sizes_.size() == other_users_.size(). We want to control the size of
    // |other_users_|, rather than its content. This function initializes
    // other_users_ as a list of |user_count| nullptrs.
    other_users_size_calculator_->other_users_ =
        user_manager::UserList(user_count);
  }

  void SimulateOnGetOtherUserSize(
      absl::optional<user_data_auth::GetAccountDiskUsageReply> reply) {
    other_users_size_calculator_->OnGetOtherUserSize(reply);
  }

 private:
  OtherUsersSizeCalculator* other_users_size_calculator_;
};

}  // namespace calculator
}  // namespace settings
}  // namespace chromeos

#endif  // CHROME_BROWSER_UI_WEBUI_SETTINGS_CHROMEOS_CALCULATOR_SIZE_CALCULATOR_TEST_API_H_
