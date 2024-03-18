/* jshint esversion: 8 */
(() => {
"use strict";

//-------------------------------------------------------------------------

const sleep = delay => new Promise(resolve => setTimeout(resolve, delay));

document.addEventListener("DOMContentLoaded", async () => {
  navigator.serviceWorker.addEventListener("controllerchange", ev => {
    console.log("serviceWorker/oncontrollerchange", ev);
  });

  navigator.serviceWorker.addEventListener("message", ev => {
    console.log("serviceWorker/onmessage", ev.data);
    const method = ev.data.method;
    if (method === "push") {
      const node = document.createElement("div");
      node.innerText = JSON.stringify(ev.data);
      document.body.append(node);
    }

    // const method = ev.data.method;
  });

  navigator.serviceWorker.register("app-service-worker.js").then(registration => {
    console.log("serviceWorker/register", registration);
    registration.addEventListener("updatefound", ev => {
      console.log("serviceWorker/registration/onupdatefound", ev);
    });
  }).catch(e => {
    console.error("serviceWorker/register/error", e);
  });

  await sleep(1000);
  navigator.serviceWorker.controller.postMessage({ method: "ping" });
});

//-------------------------------------------------------------------------

})();
