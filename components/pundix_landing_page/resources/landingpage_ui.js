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

function persehtml(address,htmlstr){
  // escapeHTMLPolicy = trustedTypes.createPolicy("default", {
  //           createHTML: input => input,
  //       });
  // var html = escapeHTMLPolicy.createHTML(htmlstr);
  var parser = new DOMParser();
  var htmlDoc = parser.parseFromString(htmlstr, 'text/html');
  var elements = htmlDoc.getElementsByClassName('search-result');
  var cars = htmlDoc.getElementsByClassName('card');
  for (let index = 0; index < elements.length; index++) {
    document.getElementsByClassName('main-content').item(0).appendChild(elements.item(index).cloneNode(true));
    }
  if(cars.length !== 0)
    document.getElementsByClassName('main-content').item(0).appendChild(cars.item(0).cloneNode(true));
    if(elements.length === 0){
  // document.getElementById('btn').textContent = "info";
  var newdiv = document.createElement('div');
  newdiv.id="not_found";
  newdiv.textContent = 'Address '+address+' did not find any data yet';
  document.getElementsByClassName('main-content').item(0).appendChild(newdiv);
  }
}
function persehtml_trusted(address,htmlstr){
  document.body.innerHTML = trustedTypes.emptyHTML;
  var parser = new DOMParser();
  var htmlDoc = parser.parseFromString(htmlstr, 'text/html');
  var elements = htmlDoc.getElementsByClassName('search-result');
  var cars = htmlDoc.getElementsByClassName('card');
  for (let index = 0; index < elements.length; index++) {
    document.body.appendChild(elements.item(index).cloneNode(true));
    }
  if(cars.length !== 0)
    document.body.appendChild(cars.item(0).cloneNode(true));
    if(elements.length === 0){
  // document.getElementById('btn').textContent = "info";
  var newdiv = document.createElement('div');
  newdiv.id="not_found";
  newdiv.textContent = 'Address '+address+' did not find any data yet';
  document.body.appendChild(newdiv);
  }
}
/* All the work we do onload. */
function onLoadWork() {
 addWebUIListener('search-results',dict=>{
  console.log('console.log'+dict.address);
  console.log('console.log'+dict.result);
  persehtml(dict.address,dict.result);
 });
 chrome.send('getSearchResult');
}

document.addEventListener('DOMContentLoaded', onLoadWork);
