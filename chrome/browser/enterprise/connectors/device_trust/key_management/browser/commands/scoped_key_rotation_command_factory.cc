// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/enterprise/connectors/device_trust/key_management/browser/commands/scoped_key_rotation_command_factory.h"

#include "base/check.h"
#include "chrome/browser/enterprise/connectors/device_trust/key_management/browser/commands/mock_key_rotation_command.h"
#include "testing/gmock/include/gmock/gmock.h"

namespace enterprise_connectors {

ScopedKeyRotationCommandFactory::ScopedKeyRotationCommandFactory() {
  KeyRotationCommandFactory::SetFactoryInstanceForTesting(this);
}

ScopedKeyRotationCommandFactory::~ScopedKeyRotationCommandFactory() {
  KeyRotationCommandFactory::ClearFactoryInstanceForTesting();
}

void ScopedKeyRotationCommandFactory::SetMock(
    std::unique_ptr<test::MockKeyRotationCommand> mock_key_rotation_command) {
  DCHECK(mock_key_rotation_command);
  mock_key_rotation_command_ = std::move(mock_key_rotation_command);
}

std::unique_ptr<KeyRotationCommand>
ScopedKeyRotationCommandFactory::CreateCommand() {
  DCHECK(mock_key_rotation_command_);
  auto temp = std::move(mock_key_rotation_command_);
  mock_key_rotation_command_ = nullptr;
  return temp;
}

}  // namespace enterprise_connectors
