/* jshint esversion: 8 */
(() => {
"use strict";

//-------------------------------------------------------------------------

addEventListener("install", ev => {
  console.log("install", ev);
  ev.waitUntil(skipWaiting());
});

addEventListener("activate", ev => {
  console.log("activate", ev);
  ev.waitUntil(clients.claim());
});

addEventListener("fetch", ev => {
  ev.respondWith((async () => {
    return await fetch(ev.request);
  })());
});

addEventListener("message", ev => {
  console.log("message", ev.data);
});

addEventListener("push", async ev => {
  console.log("push", ev);
  const clients = await globalThis.clients.matchAll();
  console.log("push", clients);
  clients.forEach(client => client.postMessage({ method: "push", body: ev.data }));
});

//-------------------------------------------------------------------------

})();
