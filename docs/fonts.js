/* jshint esversion: 8 */
(() => {
"use strict";

//-------------------------------------------------------------------------

document.addEventListener("DOMContentLoaded", () => {
  document.querySelector("#font-family").addEventListener("change", ev => {
    const node = document.querySelector(".main");
    node.classList.forEach(v => {
      if (v.startsWith("font-family-")) {
        node.classList.replace(v, ev.target.value);
      }
    });
  });

  document.querySelector("#font-weight-bold").addEventListener("change", ev => {
    if (ev.target.checked) {
      document.querySelector(".melos").classList.add("font-weight-bold");
    } else {
      document.querySelector(".melos").classList.remove("font-weight-bold");
    }
  });

  document.querySelector("#text-align-justify").addEventListener("change", ev => {
    if (ev.target.checked) {
      document.querySelector(".melos").classList.add("text-align-justify");
    } else {
      document.querySelector(".melos").classList.remove("text-align-justify");
    }
  });

  document.querySelector("#width-24em").addEventListener("change", ev => {
    if (ev.target.checked) {
      document.querySelector(".main").classList.add("width-24em");
    } else {
      document.querySelector(".main").classList.remove("width-24em");
    }
  });
});

//-------------------------------------------------------------------------

})();
