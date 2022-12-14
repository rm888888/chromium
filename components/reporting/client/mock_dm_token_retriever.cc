// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/reporting/client/mock_dm_token_retriever.h"

#include "base/test/gmock_callback_support.h"
#include "components/reporting/client/dm_token_retriever.h"

namespace reporting {

using ::base::test::RunOnceCallback;
using ::testing::_;

MockDMTokenRetriever::MockDMTokenRetriever() = default;

MockDMTokenRetriever::~MockDMTokenRetriever() = default;

void MockDMTokenRetriever::ExpectRetrieveDMTokenAndReturnResult(
    size_t times,
    const StatusOr<std::string> dm_token_result) {
  EXPECT_CALL(*this, RetrieveDMToken(_))
      .Times(times)
      .WillRepeatedly(RunOnceCallback<0>(std::move(dm_token_result)))
      .RetiresOnSaturation();
}

}  // namespace reporting
