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
//import {$} from 'chrome://resources/js/util.m.js';
var messenger_show = 'messenger_show';
var whatsapp_show = 'whatsapp_show';
var discord_show = 'discord_show';
var dune_show = 'dune_show';
var apeboard_show = 'apeboard_show';
var nftbank_show = 'nftbank_show';
var messenger_top = 'messenger_top';
var whatsapp_top = 'whatsapp_top';
var discord_top = 'discord_top';
var dune_top = 'dune_top';
var apeboard_top = 'apeboard_top';
var nftbank_top = 'nftbank_top';
/* All the work we do onload. */
function initEvent(){
  document.getElementById('messenger_top').onclick = socialbuttonclick;
  document.getElementById('messenger_show').onclick = socialbuttonclick;
  document.getElementById('whatsapp_top').onclick = socialbuttonclick;
  document.getElementById('whatsapp_show').onclick = socialbuttonclick;
  document.getElementById('discord_top').onclick = socialbuttonclick;
  document.getElementById('discord_show').onclick = socialbuttonclick;
  document.getElementById('dune_top').onclick = web3buttonclick;
  document.getElementById('dune_show').onclick = web3buttonclick;
  document.getElementById('apeboard_top').onclick = web3buttonclick;
  document.getElementById('apeboard_show').onclick = web3buttonclick;
  document.getElementById('nftbank_top').onclick = web3buttonclick;
  document.getElementById('nftbank_show').onclick = web3buttonclick;
}

/* All the work we do onload. */
function socialbuttonclick(){
var obj = event.target;
if(obj.type === 'image'){
   var top = obj.id;
   sendWithPromise('clicked',top,true);
   sortup('#divs',obj);
}else if(obj.type === 'checkbox'){
  var show = obj.id;
   sendWithPromise('clicked',show,obj.checked);
}
}

function web3buttonclick(){
var obj = event.target;
if(obj.type === 'image'){
   var top = obj.id;
   sendWithPromise('clicked',top,true);
   sortup('#divs_1',obj);
}else if(obj.type === 'checkbox'){
  var show = obj.id;
   sendWithPromise('clicked',show,obj.checked);
}
}

function sortup(parentname,obj){
var arr = $(parentname).find('div').toArray();// 把三个div放进数组里面
var index = arr.findIndex(o=>o.id === obj.id);
  var temp = arr[index];
  arr.splice(index,1);
  arr.unshift(temp);
  $(parentname).html(arr);
}

function initsort(parentname,obj,index){
  var arr = $(parentname).find('div').toArray();// 把三个div放进数组里面
  var ix = arr.findIndex(o=>o.id === obj.id);
   if(index === ix){
    $(parentname).html(arr);
    return;
  }
  var temp = arr[ix];
  var temp2 = arr[index];
  arr.splice(index,1,temp);
  arr.unshift(temp2);
  $(parentname).html(arr);
}

function initProfile(id,value){
if(id === messenger_show)
  document.getElementById(messenger_show).checked = value;
else if(id === whatsapp_show)
  document.getElementById(whatsapp_show).checked = value;
else if(id === discord_show)
  document.getElementById(discord_show).checked = value;
else if(id === dune_show)
  document.getElementById(dune_show).checked = value;
else if(id === apeboard_show)
  document.getElementById(apeboard_show).checked = value;
else if(id === nftbank_show)
  document.getElementById(nftbank_show).checked = value;
else if(id === meesenger_top){
  var temp = document.getElementById(meesenger_top);
  initsort('#divs',temp,value);
}
else if(id === whatsapp_top){
  var temp = document.getElementById(whatsapp_top);
  initsort('#divs',temp,value);
}
else if(id === discord_top){
  var temp = document.getElementById(discord_top);
 initsort('#divs',temp,value);
}
else if(id === dune_top){
  var temp = document.getElementById(dune_top);
  initsort('#divs_1',temp,value);
}
else if(id === apeboard_top){
  var temp = document.getElementById(apeboard_top);
  initsort('#divs_1',temp,value);
}
else if(id === nftbank_top){
  var temp = document.getElementById(nftbank_top);
  initsort('#divs_1',temp,value);
}
}
/* All the work we do onload. */
function onLoadWork() {
  initEvent();
  addWebUIListener('profile',dict=>{
  console.log('console.log'+dict.messenger_show);
  console.log('console.log'+dict.whatsapp_show);
  initProfile(messenger_show,dict.messenger_show);
  initProfile(whatsapp_show,dict.whatsapp_show);
  initProfile(discord_show,dict.discord_show);
  initProfile(dune_show,dict.dune_show);
  initProfile(apeboard_show,dict.apeboard_show);
  initProfile(nftbank_show,dict.nftbank_show);
  initProfile(messenger_top,dict.messenger_top);
  initProfile(whatsapp_top,dict.whatsapp_top);
  initProfile(discord_top,dict.discord_top);
  initProfile(dune_top,dict.dune_top);
  initProfile(apeboard_top,dict.apeboard_top);
  initProfile(nftbank_top,dict.nftbank_top);
 });
  sendWithPromise('getProfileInfo');
}

document.addEventListener('DOMContentLoaded', onLoadWork);
