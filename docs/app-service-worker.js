/* jshint esversion: 8 */
(() => {
"use strict";

//-------------------------------------------------------------------------

addEventListener("install", ev => {
  console.log("oninstall", ev);
  ev.waitUntil(skipWaiting());
});

addEventListener("activate", ev => {
  console.log("onactivate", ev);
  ev.waitUntil(clients.claim());
});

addEventListener("fetch", ev => {
  ev.respondWith((async () => {
    return await fetch(ev.request);
  })());
});

addEventListener("message", ev => {
  console.log("onmessage", ev.data);
  const method = ev.data.method;
  if (method === "ping") {
    ev.source.postMessage({ method: "pong" });
  }
});

addEventListener("push", async ev => {
  console.log("onpush", ev.data, ev.data.json());
  const clients = await globalThis.clients.matchAll();
  clients.forEach(client => client.postMessage({ method: "push", body: ev.data.json() }));
  // TODO 通知を表示する
});

//-------------------------------------------------------------------------

})();
