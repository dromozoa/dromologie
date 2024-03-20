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

D.testVapidSign = async () => {
  // https://datatracker.ietf.org/doc/html/rfc7515#appendix-A.3
  const data_string
    = "eyJhbGciOiJFUzI1NiJ9"
    + "."
    + "eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS9pc19yb290Ijp0cnVlfQ";

  const data = Uint8Array.from(data_string, v => v.charCodeAt(0));

  const jwk = {
    "kty": "EC",
    "crv": "P-256",
    "x": "f83OJ3D2xF1Bg8vub9tLe1gHMzV76e8Tus9uPHvRVEU",
    "y": "x_FEzRu9m36HLN_tue659LNpXW6pCyStikYjKIWI5a0",
    "d": "jpsQnnGQmL-YBIffH1136cspYG6-0iY7X1fCE9-E9LI",
  };

  const key = await crypto.subtle.importKey("jwk", jwk, { name: "ECDSA", namedCurve: "P-256" }, true, [ "sign" ]);
  const sig = new Uint8Array(await crypto.subtle.sign({ name: "ECDSA", hash: "SHA-256" }, key, data));
  console.log(sig);

  const sig_hex = [...sig].map(v => v.toString(16).padStart(2, "0").toUpperCase()).join("");
  console.log(sig_hex);
};

D.testVapidVerify = async (sig_hex) => {
  const sig = Uint8Array.from(sig_hex.match(/..?/g).map(v => parseInt(v, 16)));
  console.log(sig);

  // https://datatracker.ietf.org/doc/html/rfc7515#appendix-A.3
  const data_string
    = "eyJhbGciOiJFUzI1NiJ9"
    + "."
    + "eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS9pc19yb290Ijp0cnVlfQ";

  const data = Uint8Array.from(data_string, c => c.charCodeAt(0));

  const jwk = {
    "kty": "EC",
    "crv": "P-256",
    "x": "f83OJ3D2xF1Bg8vub9tLe1gHMzV76e8Tus9uPHvRVEU",
    "y": "x_FEzRu9m36HLN_tue659LNpXW6pCyStikYjKIWI5a0",
  };

  const key = await crypto.subtle.importKey("jwk", jwk, { name: "ECDSA", namedCurve: "P-256" }, true, [ "verify" ]);
  const result = await crypto.subtle.verify({ name: "ECDSA", hash: "SHA-256" }, key, sig, data);
  console.log(result);
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
      userVisibleOnly: document.querySelector(".push .subscribe .user-visible-only").checked,
      applicationServerKey: document.querySelector(".push .subscribe .vapid").value,
    }));
    console.log(sub);
    await updatePushView();
  });

  document.querySelector(".push .unsubscribe").addEventListener("click", async ev => {
    ev.preventDefault();
    const registration = await navigator.serviceWorker.ready;
    const sub = await registration.pushManager.getSubscription();
    if (sub) {
      const result = await(sub.unsubscribe());
      console.log(result);
    }
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
