/* jshint esversion: 8 */
(() => {
"use strict";

//-------------------------------------------------------------------------

document.addEventListener("DOMContentLoaded", async () => {
  const editNode = document.querySelector(".edit");

  // KeyboardEvent
  editNode.addEventListener("keydown", async ev => {
    console.log("keydown", ev);
  });
  editNode.addEventListener("keyup", async ev => {
    // console.log("keyup", ev);
  });

  // InputEvent
  editNode.addEventListener("beforeinput", async ev => {
    console.log("beforeinput", ev);
    // console.log("beforeinput", ev, ev.target.outerHTML);
    if (ev.inputType === "insertParagraph") {
      ev.preventDefault();
    }
  });
  editNode.addEventListener("input", async ev => {
    console.log("input", ev);
    // console.log("input", ev, ev.target.outerHTML);
    document.querySelector(".output").innerText = ev.target.outerHTML;
  });

  // CompositionEvent
  editNode.addEventListener("compositionstart", async ev => {
    // console.log("compositionstart", ev);
  });
  editNode.addEventListener("compositionupdate", async ev => {
    // console.log("compositionupdate", ev);
  });
  editNode.addEventListener("compositionend", async ev => {
    // console.log("compositionend", ev);
  });

  // ClipboardEvent
  editNode.addEventListener("cut", async ev => {
    // console.log("cut", ev);
  });
  editNode.addEventListener("copy", async ev => {
    // console.log("copy", ev);
  });
  editNode.addEventListener("paste", async ev => {
    // console.log("paste", ev);
  });

  // FocusEvent
  editNode.addEventListener("focus", async ev => {
    // console.log("focus", ev);
  });
  editNode.addEventListener("blur", async ev => {
    // console.log("blur", ev);
  });
  editNode.addEventListener("focusin", async ev => {
    // console.log("focusin", ev);
  });
  editNode.addEventListener("focusout", async ev => {
    // console.log("focusout", ev);
  });

  document.querySelector(".output").innerText = editNode.outerHTML;
});

//-------------------------------------------------------------------------

})();
