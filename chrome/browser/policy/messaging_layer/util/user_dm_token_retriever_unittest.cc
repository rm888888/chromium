// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/policy/messaging_layer/util/user_dm_token_retriever.h"

#include <memory>

#include "base/bind.h"
#include "base/test/task_environment.h"
#include "chrome/browser/policy/dm_token_utils.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/test/base/testing_browser_process.h"
#include "chrome/test/base/testing_profile_manager.h"
#include "components/policy/core/common/cloud/dm_token.h"
#include "components/reporting/util/statusor.h"
#include "components/reporting/util/test_support_callbacks.h"
#include "content/public/test/browser_task_environment.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace reporting {
namespace {

using ::testing::MockFunction;
using ::testing::Return;
using ::testing::StrEq;

constexpr char kDMToken[] = "dm-token";
constexpr char kUserId[] = "test-user";

class UserDMTokenRetrieverTest : public ::testing::Test {
 protected:
  UserDMTokenRetrieverTest()
      : testing_profile_manager_(TestingBrowserProcess::GetGlobal()) {
    // Ensure test profile manager is set up
    CHECK(testing_profile_manager_.SetUp())
        << "TestingProfileManager not setup";
    profile_ = CreateProfile(kUserId);
  }

  void SetUp() override {
    policy::SetDMTokenForTesting(
        policy::DMToken::CreateValidTokenForTesting(kDMToken));

    // Use the mock profile retrieval callback for setting up the retriever
    user_dm_token_retriever_ = UserDMTokenRetriever::CreateForTest(
        base::BindRepeating(&MockFunction<Profile*()>::Call,
                            base::Unretained(&profile_retrieval_cb_)));
  }

  void TearDown() override {
    // Unset mocks and test hooks
    policy::SetDMTokenForTesting(policy::DMToken::CreateEmptyTokenForTesting());
    user_dm_token_retriever_.reset();
  }

  // Creates a new profile with the current context for testing purposes.
  TestingProfile* CreateProfile(base::StringPiece id) {
    TestingProfile* const profile =
        testing_profile_manager_.CreateTestingProfile(std::string(id));
    return profile;
  }

  TestingProfile* profile() const { return profile_; }

  UserDMTokenRetriever* user_dm_token_retriever() const {
    return user_dm_token_retriever_.get();
  }

  MockFunction<Profile*()>& profile_retrieval_cb() {
    return profile_retrieval_cb_;
  }

 private:
  content::BrowserTaskEnvironment task_environment_{
      base::test::TaskEnvironment::MainThreadType::UI};
  TestingProfileManager testing_profile_manager_;
  TestingProfile* profile_;
  MockFunction<Profile*()> profile_retrieval_cb_;
  std::unique_ptr<UserDMTokenRetriever> user_dm_token_retriever_;
};

TEST_F(UserDMTokenRetrieverTest, GetDMToken) {
  EXPECT_CALL(profile_retrieval_cb(), Call()).WillOnce(Return(profile()));

  test::TestEvent<StatusOr<std::string>> dm_token_retrieved_event;
  user_dm_token_retriever()->RetrieveDMToken(dm_token_retrieved_event.cb());
  const auto dm_token_result = dm_token_retrieved_event.result();
  ASSERT_OK(dm_token_result);
  EXPECT_THAT(dm_token_result.ValueOrDie(), StrEq(kDMToken));
}

TEST_F(UserDMTokenRetrieverTest, GetDMTokenMultipleRequests) {
  EXPECT_CALL(profile_retrieval_cb(), Call())
      .Times(2)
      .WillRepeatedly(Return(profile()));

  test::TestEvent<StatusOr<std::string>> dm_token_retrieved_event_1;
  user_dm_token_retriever()->RetrieveDMToken(dm_token_retrieved_event_1.cb());

  test::TestEvent<StatusOr<std::string>> dm_token_retrieved_event_2;
  user_dm_token_retriever()->RetrieveDMToken(dm_token_retrieved_event_2.cb());

  const auto dm_token_result_1 = dm_token_retrieved_event_1.result();
  const auto dm_token_result_2 = dm_token_retrieved_event_2.result();
  ASSERT_OK(dm_token_result_1);
  ASSERT_OK(dm_token_result_2);
  EXPECT_THAT(dm_token_result_1.ValueOrDie(), StrEq(kDMToken));
  EXPECT_THAT(dm_token_result_2.ValueOrDie(), StrEq(kDMToken));
}

TEST_F(UserDMTokenRetrieverTest, GetDMTokenInvalid) {
  policy::SetDMTokenForTesting(policy::DMToken::CreateInvalidTokenForTesting());
  EXPECT_CALL(profile_retrieval_cb(), Call()).WillOnce(Return(profile()));

  test::TestEvent<StatusOr<std::string>> dm_token_retrieved_event;
  user_dm_token_retriever()->RetrieveDMToken(dm_token_retrieved_event.cb());
  EXPECT_FALSE(dm_token_retrieved_event.result().ok());
}

}  // namespace
}  // namespace reporting
