// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Note: The handle* functions below are called internally on promise
// resolution, unlike the other return* functions, which are called
// asynchronously by the host.

// <if expr="is_ios">
// import 'chrome://resources/js/ios/web_ui.js';
// // </if>

import './strings.m.js';
import {addWebUIListener, sendWithPromise} from 'chrome://resources/js/cr.m.js';
import {$} from 'chrome://resources/js/util.m.js';

function buttonclick(){
  sendWithPromise('buttonclick');
  console.log("clicked!");
}
function getMessage(dict){
  var result = dict["text"];
  console.log(result);
  document.getElementById('numbs').textContent = result;
}
/* All the work we do onload. */
function onLoadWork() {
  addWebUIListener("getMessage",getMessage);
  document.getElementById("btn").onclick = buttonclick;
}

document.addEventListener('DOMContentLoaded', onLoadWork);
