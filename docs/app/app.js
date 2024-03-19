/* jshint esversion: 8 */
(() => {
"use strict";

if (!globalThis.dromologie) {
  globalThis.dromologie = {};
}
const D = globalThis.dromologie;
if (D.includeGuard) {
  return;
}
D.includeGuard = true;

//-------------------------------------------------------------------------

D.Logging = class {
  log(...messages) {
    console.log(...messages);
  }

  error(...messages) {
    console.error(...messages);
  }
};

//-------------------------------------------------------------------------

const logging = new D.Logging();
const notifications = [];

const updateNotificationView = () => {
  document.querySelector(".notification .permission").innerText = Notification.permission;
  document.querySelector(".notification .max-actions").innerText = Notification.maxActions;
};

const updatePushView = async () => {
  const registration = await navigator.serviceWorker.ready;
  const sub = await registration.pushManager.getSubscription();
  document.querySelector(".push .endpoint").innerText = sub ? sub.endpoint : "";
  document.querySelector(".push .expirationTime").innerText = sub ? sub.expirationTime : "";
  document.querySelector(".push .subscriptionId").innerText = sub ? sub.subscriptionId: "";
  document.querySelector(".push .json").innerText = sub ? JSON.stringify(sub.toJSON()) : "";
};

document.addEventListener("DOMContentLoaded", async () => {
  document.querySelector(".notification .request-permission").addEventListener("click", async ev => {
    ev.preventDefault();
    await Notification.requestPermission();
    updateNotificationView();
  });
  updateNotificationView();

  document.querySelector(".notification .create").addEventListener("submit", ev => {
    ev.preventDefault();
    const icon = document.querySelector(".notification .create .icon").value;
    const title = document.querySelector(".notification .create .title").value;
    const body = document.querySelector(".notification .create .body").value;
    notifications.push(new Notification(title, {
      icon: icon,
      body: body,
    }));
  });

  document.querySelector(".push .subscribe").addEventListener("submit", async ev => {
    ev.preventDefault();
    const registration = await navigator.serviceWorker.ready;
    const sub = await(registration.pushManager.subscribe({
      userVisibleOnly: true,
      applicationServerKey: document.querySelector(".push .subscribe .vapid").value,
    }));
    console.log(sub);
    await updatePushView();
  });

  addEventListener("error", ev => {
    logging.error("error", ev);
  });

  addEventListener("unhandledrejection", ev => {
    logging.error("unhandledrejection", ev);
  });

  navigator.serviceWorker.addEventListener("controllerchange", ev => {
    logging.log("controllerchange", ev);
  });

  navigator.serviceWorker.addEventListener("message", ev => {
    logging.log("message", ev);
  });

  const registration = await navigator.serviceWorker.register("service-worker.js");
  registration.addEventListener("updatefound", ev => {
    logging.log("updatefound", ev);
  });
  await updatePushView();
});

//-------------------------------------------------------------------------

})();
