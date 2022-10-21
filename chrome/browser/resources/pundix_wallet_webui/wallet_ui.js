// // Copyright (c) 2011 The Chromium Authors. All rights reserved.
// // Use of this source code is governed by a BSD-style license that can be
// // found in the LICENSE file.

// // Note: The handle* functions below are called internally on promise
// // resolution, unlike the other return* functions, which are called
// // asynchronously by the host.


// // </if>
// //import './strings.m.js';
// //import {addWebUIListener, sendWithPromise} from 'chrome://resources/js/cr.m.js';
// //import {$} from 'chrome://resources/js/util.m.js';


// function addNumbers(){
//   var item1 = document.getElementById('first').value;
//   var item2 = document.getElementById('second').value;
//   sendWithPromise('requestVariationInfo', [item1,item2])
//       .then(getResult);
// }
// function getResult(result){
//   document.getElementById('result').textContent = result;
// }
// /* All the work we do onload. */
// function onLoadWork() {

//   //addWebUIListener('return-os-version', returnOsVersion);
//   document.getElementById('equal').onclick = addNumbers;

 
//   sendWithPromise('requestVersionInfo');
//   sendWithPromise('requestVariationInfo', ['9','10'])
//       .then(getResult);
//   // chrome.send('requestVersionInfo');
//   // const includeVariationsCmd = location.search.includes("show-variations-cmd");
//   // sendWithPromise('requestVariationInfo', includeVariationsCmd)
//   //     .then(handleVariationInfo);
//   // sendWithPromise('requestPathInfo').then(handlePathInfo);

  
// }

// document.addEventListener('DOMContentLoaded', onLoadWork);

//update on 20220311
// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Note: The handle* functions below are called internally on promise
// resolution, unlike the other return* functions, which are called
// asynchronously by the host.

// <if expr="is_ios">
import 'chrome://resources/js/ios/web_ui.js';
// </if>

import './strings.m.js';
import {addWebUIListener, sendWithPromise} from 'chrome://resources/js/cr.m.js';
import {$} from 'chrome://resources/js/util.m.js';

/**
 * Promise resolution handler for variations list and command line equivalent.
 * @param {{variationsList: !Array<string>, variationsCmd: string=}}
 */
function handleVariationInfo({variationsList, variationsCmd}) {
  $('variations-section').hidden = !variationsList.length;
  for (const item of variationsList) {
    $('variations-list').appendChild(document.createTextNode(item));
    $('variations-list').appendChild(document.createElement('br'));
  }

  if (variationsCmd) {
    $('variations-cmd-section').hidden = !variationsCmd;
    $('variations-cmd').textContent = variationsCmd;
  }
}

/**
 * Promise resolution handler for the executable and profile paths to display.
 * @param {string} execPath The executable path to display.
 * @param {string} profilePath The profile path to display.
 */
function handlePathInfo({execPath, profilePath}) {
  $('executable_path').textContent = execPath;
  $('profile_path').textContent = profilePath;
}

// <if expr="chromeos or is_win">
/**
 * Callback from the backend with the OS version to display.
 * @param {string} osVersion The OS version to display.
 */
function returnOsVersion(osVersion) {
  $('os_version').textContent = osVersion;
}
// </if>

// <if expr="chromeos">
/**
 * Callback from the backend with the firmware version to display.
 * @param {string} firmwareVersion
 */
function returnOsFirmwareVersion(firmwareVersion) {
  $('firmware_version').textContent = firmwareVersion;
}

/**
 * Callback from the backend with the ARC version to display.
 * @param {string} arcVersion The ARC version to display.
 */
function returnARCVersion(arcVersion) {
  $('arc_version').textContent = arcVersion;
  $('arc_holder').hidden = !arcVersion;
}
//send the message to C++
function addNumbers() {
  var item1 = document.getElementById("f1").value;
  var item2 = document.getElementById("f2").value;
  sendWithPromise("addNumbers", [item1,item2])
      .then(getResult);
  alert("47329478jfdsjfldjsf");
}
//receive the c++ message
function getResult(res){
   document.getElementById('result').value = res;
}
/**
 * Callback from chromeosInfoPrivate with the value of the customization ID.
 * @param {!{customizationId: string}} response
 */
function returnCustomizationId(response) {
  if (!response.customizationId) {
    return;
  }
  $('customization_id_holder').hidden = false;
  $('customization_id').textContent = response.customizationId;
}
// </if>

/* All the work we do onload. */
function onLoadWork() {
  // <if expr="chromeos or is_win">
  addWebUIListener('return-os-version', returnOsVersion);
  // </if>
  // <if expr="chromeos">
  addWebUIListener('return-os-firmware-version', returnOsFirmwareVersion);
  addWebUIListener('return-arc-version', returnARCVersion);
  // </if>

  //onclick
  document.getElementById("equal").onclick = function(){
    var item1 = document.getElementById("f1").value;
    var item2 = document.getElementById("f2").value;
    sendWithPromise("addNumbers", [item1,item2])
      .then(getResult);
    alert("47329478jfdsjfldjsf");
  };

  chrome.send('requestVersionInfo');
  const includeVariationsCmd = location.search.includes("show-variations-cmd");
  sendWithPromise('requestVariationInfo', includeVariationsCmd)
      .then(handleVariationInfo);
  sendWithPromise('requestPathInfo').then(handlePathInfo);

  //test for web
  sendWithPromise("addNumbers", ["1","6"])
      .then(getResult);
    alert("47329478jfdsjfldjsf");
  // <if expr="chromeos">
  $('arc_holder').hidden = true;
  chrome.chromeosInfoPrivate.get(['customizationId'], returnCustomizationId);
  // </if>

  if ($('sanitizer').textContent !== '') {
    $('sanitizer-section').hidden = false;
  }
}

document.addEventListener('DOMContentLoaded', onLoadWork);
