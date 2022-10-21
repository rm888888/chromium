// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_COMMON_CREDENTIAL_PROVIDER_CREDENTIAL_STORE_H_
#define IOS_CHROME_COMMON_CREDENTIAL_PROVIDER_CREDENTIAL_STORE_H_

#import <Foundation/Foundation.h>

@protocol Credential;

// Manages the storage for credentials.
@protocol CredentialStore

// All the stored credentials.
@property(nonatomic, readonly) NSArray<id<Credential>>* credentials;

// Returns the credential with matching |recordIdentifier| or nil if none.
- (id<Credential>)credentialWithRecordIdentifier:(NSString*)recordIdentifier;

@end

// Manages a mutable store for credentials.
@protocol MutableCredentialStore <CredentialStore>

// When supported by the store save data to disk. |error| will be nil if the
// save operation was successful, otherwise it will be the error generated by
// the system.
- (void)saveDataWithCompletion:(void (^)(NSError* error))completion;

// Removes all the credentials. Use |-saveDataWithCompletion:| to
// update the data on disk.
- (void)removeAllCredentials;

// Adds a credential to the memory storage. Use |-saveDataWithCompletion:| to
// update the data on disk.
- (void)addCredential:(id<Credential>)credential;

// Updates a credential in the memory storage. Use |-saveDataWithCompletion:| to
// update the data on disk.
- (void)updateCredential:(id<Credential>)credential;

// Removes a credential from the memory storage. Use |-saveDataWithCompletion:|
// to update the data on disk.
- (void)removeCredentialWithRecordIdentifier:(NSString*)recordIdentifier;

@end

#endif  // IOS_CHROME_COMMON_CREDENTIAL_PROVIDER_CREDENTIAL_STORE_H_