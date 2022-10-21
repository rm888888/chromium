// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// ReadLaterUI is a Mojo WebUI controller and therefore needs mojo defined to
// finish running its tests.
import 'chrome://resources/mojo/mojo/public/js/mojo_bindings_lite.js';

import 'chrome://read-later.top-chrome/side_panel/bookmarks_list.js';

import {BookmarkFolderElement, FOLDER_OPEN_CHANGED_EVENT} from 'chrome://read-later.top-chrome/side_panel/bookmark_folder.js';
import {BookmarksApiProxyImpl} from 'chrome://read-later.top-chrome/side_panel/bookmarks_api_proxy.js';
import {BookmarksListElement, LOCAL_STORAGE_OPEN_FOLDERS_KEY} from 'chrome://read-later.top-chrome/side_panel/bookmarks_list.js';
import {flush} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js';

import {assertEquals, assertTrue} from 'chrome://webui-test/chai_assert.js';
import {flushTasks} from 'chrome://webui-test/test_util.js';

import {TestBookmarksApiProxy} from './test_bookmarks_api_proxy.js';

suite('SidePanelBookmarksListTest', () => {
  let bookmarksList: BookmarksListElement;
  let bookmarksApi: TestBookmarksApiProxy;

  const folders: chrome.bookmarks.BookmarkTreeNode[] = [
    {
      id: '0',
      parentId: 'root',
      title: 'Bookmarks bar',
      children: [
        {
          id: '3',
          parentId: '0',
          title: 'Child bookmark',
          url: 'http://child/bookmark/',
        },
        {
          id: '4',
          parentId: '0',
          title: 'Child folder',
          children: [
            {
              id: '5',
              parentId: '4',
              title: 'Nested bookmark',
              url: 'http://nested/bookmark/',
            },
          ],
        }
      ],
    },
    {
      id: '1',
      parentId: 'root',
      title: 'Other bookmarks',
      children: [],
    },
    {
      id: '2',
      title: 'Mobile bookmarks',
      children: [],
    },
  ];

  function getFolderElements(root: HTMLElement): BookmarkFolderElement[] {
    return Array.from(root.shadowRoot!.querySelectorAll('bookmark-folder'));
  }

  function getBookmarkElements(root: HTMLElement): HTMLElement[] {
    return Array.from(root.shadowRoot!.querySelectorAll('.bookmark'));
  }

  setup(async () => {
    window.localStorage[LOCAL_STORAGE_OPEN_FOLDERS_KEY] = undefined;
    document.body.innerHTML = '';

    bookmarksApi = new TestBookmarksApiProxy();
    bookmarksApi.setFolders(JSON.parse(JSON.stringify(folders)));
    BookmarksApiProxyImpl.setInstance(bookmarksApi);

    bookmarksList = document.createElement('bookmarks-list');
    document.body.appendChild(bookmarksList);

    await flushTasks();
  });

  test('GetsAndShowsFolders', () => {
    assertEquals(1, bookmarksApi.getCallCount('getFolders'));
    assertEquals(folders.length, getFolderElements(bookmarksList).length);
  });

  test('UpdatesChangedBookmarks', () => {
    const rootFolderIndex = 0;
    const bookmarkIndex = 0;

    const changedBookmark = folders[rootFolderIndex]!.children![bookmarkIndex]!;
    bookmarksApi.callbackRouter.onChanged.callListeners(changedBookmark.id, {
      title: 'New title',
      url: 'http://new/url',
    });

    const folderElement = getFolderElements(bookmarksList)[rootFolderIndex] as
        BookmarkFolderElement;
    const bookmarkElement = getBookmarkElements(folderElement)[bookmarkIndex]!;
    assertEquals('New title', bookmarkElement.textContent);
  });

  test('UpdatesReorderedChildren', () => {
    // Reverse the children of Bookmarks bar.
    const children = folders[0]!.children!;
    const reverseOrder = children.map(child => child.id).reverse();
    bookmarksApi.callbackRouter.onChildrenReordered.callListeners(
        folders[0]!.id, {childIds: reverseOrder});
    flush();

    const rootFolderElement = getFolderElements(bookmarksList)[0]!;
    const childFolder = getFolderElements(rootFolderElement)[0]!;
    const childBookmark = getBookmarkElements(rootFolderElement)[0]!;
    assertTrue(
        !!(childFolder.compareDocumentPosition(childBookmark) &
           Node.DOCUMENT_POSITION_FOLLOWING));
  });

  test('AddsCreatedBookmark', async () => {
    bookmarksApi.callbackRouter.onCreated.callListeners('999', {
      id: '999',
      title: 'New bookmark',
      index: 0,
      parentId: '4',
      url: '//new/bookmark',
    });
    flush();

    const rootFolderElement = getFolderElements(bookmarksList)[0]!;
    const childFolder = getFolderElements(rootFolderElement)[0]!;
    childFolder.shadowRoot!.querySelector<HTMLElement>(
                               '.row')!.click();  // Open folder.
    await flushTasks();
    const childFolderBookmarks = getBookmarkElements(childFolder);
    assertEquals(2, childFolderBookmarks.length);
    assertEquals('New bookmark', childFolderBookmarks[0]!.textContent);
  });

  test('AddsCreatedBookmarkForNewFolder', () => {
    // Create a new folder without a children array.
    bookmarksApi.callbackRouter.onCreated.callListeners('1000', {
      id: '1000',
      title: 'New folder',
      index: 0,
      parentId: '0',
    });
    flush();

    // Create a new bookmark within that folder.
    bookmarksApi.callbackRouter.onCreated.callListeners('1001', {
      id: '1001',
      title: 'New bookmark in new folder',
      index: 0,
      parentId: '1000',
      url: 'http://google.com',
    });
    flush();

    const rootFolderElement = getFolderElements(bookmarksList)[0]!;
    const newFolder = getFolderElements(rootFolderElement)[0]!;
    assertEquals(1, newFolder.folder.children!.length);
  });

  test('MovesBookmarks', () => {
    const movedBookmark = folders[0]!.children![1]!.children![0]!;
    bookmarksApi.callbackRouter.onMoved.callListeners(movedBookmark.id, {
      index: 0,
      parentId: folders[0]!.id,                   // Moving to bookmarks bar.
      oldParentId: folders[0]!.children![1]!.id,  // Moving from child folder.
      oldIndex: 0,
    });
    flush();

    const bookmarksBarFolder = getFolderElements(bookmarksList)[0]!;
    const movedBookmarkElement = getBookmarkElements(bookmarksBarFolder)[0]!;
    assertEquals('Nested bookmark', movedBookmarkElement.textContent);

    const childFolder = getFolderElements(bookmarksBarFolder)[0]!;
    const childFolderBookmarks = getBookmarkElements(childFolder);
    assertEquals(0, childFolderBookmarks.length);
  });

  test('MovesBookmarksIntoNewFolder', () => {
    // Create a new folder without a children array.
    bookmarksApi.callbackRouter.onCreated.callListeners('1000', {
      id: '1000',
      title: 'New folder',
      index: 0,
      parentId: '0',
    });
    flush();

    const movedBookmark = folders[0]!.children![1]!.children![0]!;
    bookmarksApi.callbackRouter.onMoved.callListeners(movedBookmark.id, {
      index: 0,
      parentId: '1000',
      oldParentId: folders[0]!.children![1]!.id,
      oldIndex: 0,
    });
    flush();

    const bookmarksBarFolder = getFolderElements(bookmarksList)[0]!;
    const newFolder = getFolderElements(bookmarksBarFolder)[0]!;
    assertEquals(1, newFolder.folder.children!.length);
  });

  test('DefaultsToFirstFolderBeingOpen', () => {
    assertEquals(
        JSON.stringify([folders[0]!.id]),
        window.localStorage[LOCAL_STORAGE_OPEN_FOLDERS_KEY]);
  });

  test('UpdatesLocalStorageOnFolderOpenChanged', () => {
    bookmarksList.dispatchEvent(new CustomEvent(FOLDER_OPEN_CHANGED_EVENT, {
      bubbles: true,
      composed: true,
      detail: {
        id: folders[0]!.id,
        open: false,
      }
    }));
    assertEquals(
        JSON.stringify([]),
        window.localStorage[LOCAL_STORAGE_OPEN_FOLDERS_KEY]);

    bookmarksList.dispatchEvent(new CustomEvent(FOLDER_OPEN_CHANGED_EVENT, {
      bubbles: true,
      composed: true,
      detail: {
        id: '5001',
        open: true,
      }
    }));
    assertEquals(
        JSON.stringify(['5001']),
        window.localStorage[LOCAL_STORAGE_OPEN_FOLDERS_KEY]);
  });

  test('MovesFocusBetweenFolders', () => {
    const folderElements = getFolderElements(bookmarksList);

    function dispatchArrowKey(key: string) {
      bookmarksList.dispatchEvent(new KeyboardEvent('keydown', {key}));
    }

    function assertActiveElement(index: number) {
      assertEquals(
          folderElements[index], bookmarksList.shadowRoot!.activeElement);
    }

    // Move focus to the first folder.
    folderElements[0]!.moveFocus(1);
    assertActiveElement(0);

    // One ArrowDown key should still keep focus in the first folder since the
    // folder has children.
    dispatchArrowKey('ArrowDown');
    assertActiveElement(0);

    // Two ArrowsDown to eventually make it to the second folder.
    dispatchArrowKey('ArrowDown');
    dispatchArrowKey('ArrowDown');
    assertActiveElement(1);

    // One ArrowsDown to eventually make it to the third folder.
    dispatchArrowKey('ArrowDown');
    assertActiveElement(2);

    // One ArrowsDown to loop back to the first folder.
    dispatchArrowKey('ArrowDown');
    assertActiveElement(0);

    // One ArrowUp to loop back to the last folder.
    dispatchArrowKey('ArrowUp');
    assertActiveElement(2);

    // One ArrowUp to loop back to the second folder.
    dispatchArrowKey('ArrowUp');
    assertActiveElement(1);
  });

  test('CutsCopyPastesBookmark', async () => {
    const folderElement = getFolderElements(bookmarksList)[0]!;
    const bookmarkElement = getBookmarkElements(folderElement)[0]!;

    bookmarkElement.dispatchEvent(new KeyboardEvent(
        'keydown', {key: 'x', ctrlKey: true, bubbles: true, composed: true}));
    const cutId = await bookmarksApi.whenCalled('cutBookmark');
    assertEquals('3', cutId);

    bookmarkElement.dispatchEvent(new KeyboardEvent(
        'keydown', {key: 'c', ctrlKey: true, bubbles: true, composed: true}));
    const copiedId = await bookmarksApi.whenCalled('copyBookmark');
    assertEquals('3', copiedId);

    bookmarkElement.dispatchEvent(new KeyboardEvent(
        'keydown', {key: 'v', ctrlKey: true, bubbles: true, composed: true}));
    let [pastedId, pastedDestinationId] =
        await bookmarksApi.whenCalled('pasteToBookmark');
    assertEquals('0', pastedId);
    assertEquals('3', pastedDestinationId);
  });
});