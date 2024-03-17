/* jshint esversion: 8 */
(() => {
"use strict";

//-------------------------------------------------------------------------

document.addEventListener("DOMContentLoaded", () => {
  document.querySelector(".font-select").addEventListener("change", ev => {
    ev.preventDefault();
    document.body.classList.forEach(v => {
      if (v.startsWith("font-")) {
        document.body.classList.replace(v, ev.target.value);
      }
    });
  });
});

//-------------------------------------------------------------------------

})();
