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

  document.querySelector("#ruby").addEventListener("change", ev => {
    if (ev.target.checked) {
      document.querySelectorAll(".ruby ruby").forEach(node => node.classList.remove("hide"));
      document.querySelectorAll(".ruby .ruby-base").forEach(node => node.classList.add("hide"));
    } else {
      document.querySelectorAll(".ruby ruby").forEach(node => node.classList.add("hide"));
      document.querySelectorAll(".ruby .ruby-base").forEach(node => node.classList.remove("hide"));
    }
  });

  document.querySelectorAll("ruby").forEach(node1 => {
    let base = "";
    node1.querySelectorAll("rb").forEach(node => base += node.innerText);

    const node2 = document.createElement("span");
    node2.classList.add("ruby");

    const node3 = document.createElement("span");
    node3.classList.add("ruby-base");
    node3.classList.add("hide");
    node3.innerText = base;

    node1.replaceWith(node2);
    node2.append(node1, node3);
  });
});

//-------------------------------------------------------------------------

})();
