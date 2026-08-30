var tauriStatus = document.getElementById("tauri-status");

function log(msg) {
  console.log("[APP] " + msg);
  if (tauriStatus) {
    tauriStatus.textContent = (tauriStatus.textContent ? tauriStatus.textContent + " | " : "") + msg;
  }
}

// Tauri v2: invoke 在 window.__TAURI__.core.invoke
var invoke = null;
try {
  if (window.__TAURI__) {
    log("TAURI exists");
    if (window.__TAURI__.core) {
      log("core exists");
      if (window.__TAURI__.core.invoke) {
        invoke = window.__TAURI__.core.invoke;
        log("invoke found");
      }
    }
    // Tauri v2 也可能直接暴露
    if (!invoke && window.__TAURI__.invoke) {
      invoke = window.__TAURI__.invoke;
      log("invoke found at root");
    }
  } else {
    log("TAURI NOT available");
  }
} catch (e) {
  log("error: " + e.message);
}

var searchInput = document.getElementById("searchInput");
var searchBtn = document.getElementById("searchBtn");
var resultsSection = document.getElementById("resultsSection");
var resultsHeader = document.getElementById("resultsHeader");
var resultsBody = document.getElementById("resultsBody");
var emptyState = document.getElementById("emptyState");
var charCount = document.getElementById("charCount");

function init() {
  log("init called");
  invoke("get_char_count").then(function (count) {
    log("char count: " + count);
    charCount.textContent = count + " 个汉字已加载";
  }).catch(function (e) {
    log("init error: " + e);
    charCount.textContent = "数据加载失败: " + e;
  });
}

function doSearch() {
  var input = searchInput.value.trim();
  log("doSearch input: '" + input + "'");
  if (!input) {
    resultsSection.style.display = "none";
    emptyState.textContent = "";
    return;
  }

  emptyState.textContent = "查询中...";
  resultsSection.style.display = "none";

  invoke("query_chars", { input: input }).then(function (results) {
    log("query result count: " + results.length);
    renderResults(results);
  }).catch(function (e) {
    log("query error: " + e);
    emptyState.textContent = "查询出错: " + e;
    resultsSection.style.display = "none";
  });
}

function renderResults(results) {
  emptyState.textContent = "";
  resultsBody.innerHTML = "";

  if (results.length === 0) {
    resultsSection.style.display = "none";
    emptyState.textContent = "未找到匹配的汉字编码";
    return;
  }

  resultsSection.style.display = "flex";
  resultsHeader.textContent = "查询结果: 共 " + results.length + " 个汉字";

  results.forEach(function (item, index) {
    var tr = document.createElement("tr");

    var pinyin = item.pinyin || "-";
    var strokes = item.strokes || "-";
    var simpleCodes = item.simple_codes && item.simple_codes.length > 0 ? item.simple_codes.join(", ") : "-";

    var imgId = "img-" + index;
    var imgHtml = '<img id="' + imgId + '" style="max-width:80px;max-height:80px;" />';

    tr.innerHTML =
      '<td class="index-cell">' + (index + 1) + '</td>' +
      '<td class="char-cell">' + item.character + '</td>' +
      '<td class="info-cell">拼音: ' + pinyin + '&nbsp;&nbsp;笔画: ' + strokes + '</td>' +
      '<td class="code-cell">' + (item.wubi_code || "-") + '</td>' +
      '<td class="simple-cell">' + simpleCodes + '</td>' +
      '<td class="image-cell">' + imgHtml + '</td>';

    resultsBody.appendChild(tr);

    invoke("get_image_base64", { character: item.character }).then(function (dataUrl) {
      var imgEl = document.getElementById(imgId);
      if (imgEl && dataUrl) {
        imgEl.src = dataUrl;
      } else if (imgEl) {
        imgEl.outerHTML = '<span>无图片</span>';
      }
    }).catch(function (e) {
      var imgEl = document.getElementById(imgId);
      if (imgEl) {
        imgEl.outerHTML = '<span>加载失败</span>';
      }
    });
  });

  log("rendered " + results.length + " rows");
}

searchBtn.addEventListener("click", function () {
  log("button clicked");
  doSearch();
});

searchInput.addEventListener("keydown", function (e) {
  if (e.key === "Enter") {
    log("enter pressed");
    doSearch();
  }
});

log("app.js loaded");
init();

// 监听全局快捷键事件，聚焦输入框
window.__TAURI__.event.listen("focus-search", function () {
  if (searchInput) {
    searchInput.focus();
    log("focus-search event: input focused");
  }
});
