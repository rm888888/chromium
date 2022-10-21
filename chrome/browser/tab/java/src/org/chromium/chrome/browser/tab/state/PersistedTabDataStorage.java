// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.tab.state;

import org.chromium.base.Callback;
import org.chromium.base.supplier.Supplier;

import java.nio.ByteBuffer;
import java.util.List;

/**
 * Storage for {@link PersistedTabData}
 */
public interface PersistedTabDataStorage {
    /**
     * @param tabId identifier for the {@link Tab}
     * @param tabDataId unique identifier representing the type {@link PersistedTabData}
     * @param dataSupplier {@link Supplier} for serialized {@link PersistedTabData}
     */
    void save(int tabId, String tabDataId, Supplier<ByteBuffer> dataSupplier);

    /**
     * @param tabId identifier for the {@link Tab}
     * @param tabDataId unique identifier representing the type of {@link PersistedTabData}
     * @param callback to pass back the seraizliaed {@link PersistedTabData} in
     */
    void restore(int tabId, String tabDataId, Callback<ByteBuffer> callback);

    /**
     * @param tabId identifier for the {@link Tab}
     * @param tabDataId unique identifier representing the type of {@link PersistedTabData}
     * @return serialized {@link PersitsedTabData}
     */
    ByteBuffer restore(int tabId, String tabDataId);

    /**
     * @param tabId identifier for the {@link Tab}
     * @param tabDataId unique identifier representing the type of {@link PersistedTabData}
     */
    void delete(int tabId, String tabDataId);

    /**
     * @return unique tag appended to the end of metrics for Uma
     */
    String getUmaTag();

    /**
     * Identifies and deletes stored {@link Tab} data for Tabs which no longer exist.
     * This can occur, for example, if a {@link Tab} is closed and then the app crashes,
     * before the {@link PersistedTabData} stored for that {@link Tab} is cleaned up.
     * @param tabIds {@link Tab} identifiers corresponding to current live Tabs -
     * no stored {@link PersistedTabData} corresponding to these Tabs will be removed.
     * @param dataId identifier for {@link PersistedTabData} which is stored. Each
     * {@link PersistedTabData} has a unique identifier in the database.
     */
    void performMaintenance(List<Integer> tabIds, String dataId);
}