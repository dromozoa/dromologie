/* jshint esversion: 8 */
(() => {
"use strict";

//-------------------------------------------------------------------------

const sleep = delay => new Promise(resolve => setTimeout(resolve, delay));

const log = (...message) => {
  console.log(...message);

  const node = document.createElement("div");
  node.classList.add("logger-log");
  node.innerText = message[0];
  document.querySelector("#logger").append(node);
};

const error = (...message) => {
  console.error(...message);
  const node = document.createElement("div");
  node.classList.add("logger-error");
  node.innerText = message[0];
  document.querySelector("#logger").append(node);
};

document.addEventListener("DOMContentLoaded", async () => {
  addEventListener("error", ev => {
    error("検出: 大域エラー", ev);
  });

  addEventListener("unhandledrejection", ev => {
    error("検出: 見過ごされた拒否", ev.reason);
  });

  navigator.serviceWorker.addEventListener("controllerchange", ev => {
    log("変更: サービスワーカーコントローラー", ev);
  });

  navigator.serviceWorker.addEventListener("message", ev => {
    log("受信: サービスワーカーメッセージ", ev.data);
    // const method = ev.data.method;
    // if (method === "push") {
    // }
  });

  const registration = await navigator.serviceWorker.register("service-worker.js");
  registration.addEventListener("updatefound", ev => {
    log("更新: サービスワーカー登録", ev);
  });
});

//-------------------------------------------------------------------------

})();
