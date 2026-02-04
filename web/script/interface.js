var fields = Array(
  "title",
  "booktitle",
  "editor",
  "organization",
  "address",
  "series",
  "edition",
  "howpublished",
  "chapter",
  "school",
  "institution",
  "author",
  "journal",
  "volume",
  "number",
  "pages",
  "publisher",
  "month",
  "year",
  "summary",
  "note",
);

var toggle = new Object();

toggle["ARTICLE"] = Array(
  true,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
);
toggle["INPROCEEDINGS"] = Array(
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  false,
  false,
  false,
  false,
  true,
  false,
  true,
  false,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
);
toggle["PROCEEDINGS"] = Array(
  true,
  false,
  true,
  true,
  true,
  true,
  false,
  false,
  false,
  false,
  false,
  true,
  false,
  true,
  false,
  true,
  true,
  true,
  true,
  true,
  false,
);
toggle["BOOK"] = Array(
  true,
  true,
  false,
  false,
  false,
  true,
  true,
  false,
  true,
  false,
  false,
  true,
  false,
  false,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
  true,
);
toggle["BOOKLET"] = Array(
  true,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  true,
  false,
  false,
  false,
  true,
  false,
  true,
  true,
  true,
  false,
);
toggle["INBOOK"] = Array(
  true,
  false,
  true,
  false,
  true,
  true,
  true,
  false,
  true,
  false,
  false,
  true,
  false,
  true,
  false,
  true,
  true,
  true,
  true,
  true,
  false,
);
toggle["INCOLLECTION"] = Array(
  true,
  true,
  true,
  false,
  true,
  true,
  true,
  false,
  true,
  false,
  false,
  true,
  false,
  true,
  false,
  true,
  true,
  true,
  true,
  true,
  false,
);
toggle["MASTERTHESIS"] = Array(
  true,
  false,
  false,
  false,
  true,
  false,
  false,
  false,
  false,
  true,
  false,
  true,
  false,
  false,
  false,
  false,
  false,
  true,
  true,
  true,
  false,
);
toggle["PHDTHESIS"] = Array(
  true,
  false,
  false,
  false,
  true,
  false,
  false,
  false,
  false,
  true,
  false,
  true,
  false,
  false,
  false,
  false,
  false,
  true,
  true,
  true,
  false,
);
toggle["TECHREPORT"] = Array(
  true,
  false,
  false,
  false,
  true,
  false,
  false,
  false,
  false,
  false,
  true,
  true,
  false,
  false,
  true,
  false,
  false,
  true,
  true,
  true,
  false,
);
toggle["MISC"] = Array(
  true,
  false,
  false,
  false,
  false,
  false,
  false,
  true,
  false,
  false,
  false,
  true,
  false,
  false,
  false,
  false,
  false,
  true,
  true,
  true,
  false,
);
toggle["MANUAL"] = Array(
  true,
  false,
  false,
  true,
  true,
  false,
  true,
  false,
  false,
  false,
  false,
  true,
  false,
  false,
  false,
  false,
  false,
  true,
  true,
  true,
  false,
);
toggle["UNPUBLISHED"] = Array(
  true,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  true,
  false,
  false,
  false,
  false,
  false,
  true,
  true,
  true,
  true,
);
toggle["SOFTWARE"] = Array(
  true,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  true,
  false,
  false,
  false,
  false,
  true,
  true,
  true,
  true,
  true,
);

function evalType(obj) {
  var sel = obj.options[obj.selectedIndex].value;

  for (i = 0; i < fields.length; i++) {
    if (toggle[sel][i] == true) {
      document.forms.record[fields[i]].style.background =
        document.forms.record.type.style.background;
      document.forms.record[fields[i]].disabled = false;
    } else {
      document.forms.record[fields[i]].style.background = "#dddddd";
      document.forms.record[fields[i]].disabled = true;
    }
  }
}

var viewWindow = "";

function openWindow(url, width, height) {
  if (!viewWindow.closed && viewWindow.location) {
    viewWindow.resizeTo(width, height);
    viewWindow.location.href = url;
  } else {
    viewWindow = window.open(
      url,
      "viewWindow",
      "toolbar=no, location=no, directories=no, status=no, menubar=no, scrollbars=no, resizable=no, copyhistory=no, width=" +
        width +
        ", height=" +
        height,
    );
    if (!viewWindow.opener) viewWindow.opener = self;
  }
  if (window.focus) viewWindow.focus();
}

String.prototype.trim = function () {
  return this.replace(/^\s+|\s+$/g, "");
};

var selected = -1;
var values = Array();
var valid = true;
var keepfocus = false;

function checkinput(obj) {
  if (!valid) return false;

  values.length = 0;

  start = obj.value.lastIndexOf(",");

  if (-1 == start) start = 0;
  else ++start;

  needle = obj.value
    .substr(start, obj.value.length - start)
    .trim()
    .toLowerCase();

  j = 0;
  for (i = 0; i < choices.length; i++) {
    if (
      needle.length > 0 &&
      choices[i].substr(0, needle.length).toLowerCase() == needle
    )
      values[j++] = choices[i];
  }

  selectbox = document.getElementById("autocomplete");

  if (j > 0 && values.length > 0) {
    selectbox.style.visibility = "visible";
    str = "";
    for (i = 0; i < j; i++)
      str +=
        '<div style="cursor:pointer;" id="autofill_' +
        i +
        '" onmouseover="dim(selected);selected=' +
        i +
        ';highlight(selected);" onclick="setinput(\'' +
        values[i] +
        "');\">" +
        values[i] +
        "</div>";
    selectbox.innerHTML = str;
  } else selectbox.style.visibility = "hidden";

  selected = -1;
}

function setinput(value) {
  selectbox = document.getElementById("autocomplete");

  obj = document.forms.record.keywords;

  start = obj.value.lastIndexOf(",");

  if (-1 == start) start = 0;
  else ++start;

  obj.value = obj.value.substr(0, start) + (start > 0 ? " " : "") + value;

  obj.focus();

  keepfocus = false;

  selectbox.style.visibility = "hidden";
}

function checkchar(e) {
  switch (e.keyCode) {
    case 9:
    case 13:
      if (selected != -1) {
        setinput(values[selected]);
        selected = -1;
        return false;
      }
      break;
    case 38:
      dim(selected);
      selected--;
      if (selected < 0) selected = values.length - 1;
      highlight(selected);
      valid = false;
      return false;
      break;
    case 40:
      dim(selected);
      selected++;
      if (selected == values.length) selected = 0;
      valid = false;
      highlight(selected);
      return false;
      break;
    default:
      valid = true;
      return true;
  }
}

function highlight(id) {
  obj = document.getElementById("autofill_" + id);
  obj.style.background = "#abc3de";
}

function dim(id) {
  if (selected < 0) return;

  obj = document.getElementById("autofill_" + id);
  obj.style.background = "";
}

function hide() {
  if (keepfocus) return true;

  obj = document.getElementById("autocomplete");
  obj.style.visibility = "hidden";

  select = -1;
}

function gotoURL(url) {
  window.document.location = url;
}

function loginfunc(e) {
  var key;
  if (window.event) key = window.event.keyCode;
  else if (e.which) key = e.which;

  if (e.target) src = e.target;
  else if (e.srcElement) src = e.srcElement;

  if (key == 13) {
    if (src.name == "password") document.forms.login.submit();
    else if (src.name == "username") document.forms.login.password.focus();
    return false;
  }

  return true;
}

function setfocus() {
  if (!window.navigator.standalone) {
    if (document.forms.login) document.forms.login.username.focus();
    else document.forms.search.match.focus();
  }
}

if (window.navigator.standalone) {
  document.addEventListener(
    "DOMContentLoaded",
    function () {
      var a = document.getElementsByTagName("a");
      for (var i = 0; i < a.length; i++) {
        a[i].onclick = function () {
          window.location = this.getAttribute("href");
          return false;
        };
      }
    },
    false,
  );
}
