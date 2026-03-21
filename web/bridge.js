// bridge.js — SAGEDash WebView2 브릿지 공통 모듈
//
// 사용법:
//   <script src="bridge.js"></script>
//   SageBridge.on('native:ready', function(payload) { ... });
//   SageBridge.send('web:request-summary', {});
//
// 메시지 포맷: { "type": "...", "version": "1.0", "payload": { ... } }

(function(global) {
  'use strict';

  var BRIDGE_VERSION = '1.0';
  var _handlers = {};

  function on(type, handler) {
    _handlers[type] = handler;
  }

  function send(type, payload) {
    var msg = JSON.stringify({
      type: type,
      version: BRIDGE_VERSION,
      payload: payload || {}
    });
    window.chrome.webview.postMessage(msg);
  }

  function _dispatch(msg) {
    var handler = _handlers[msg.type];
    if (handler) {
      handler(msg.payload);
    }
  }

  window.chrome.webview.addEventListener('message', function(e) {
    var msg;
    try {
      msg = JSON.parse(e.data);
    } catch (err) {
      return;
    }
    _dispatch(msg);
  });

  global.SageBridge = { on: on, send: send };

}(window));
