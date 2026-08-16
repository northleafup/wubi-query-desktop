var invoke = window.__TAURI__.core.invoke;

var searchInput = document.getElementById("searchInput");
var searchBtn = document.getElementById("searchBtn");
var resultsSection = document.getElementById("resultsSection");
var resultsHeader = document.getElementById("resultsHeader");
var resultsBody = document.getElementById("resultsBody");
var emptyState = document.getElementById("emptyState");
var charCount = document.getElementById("charCount");

function init() {
  invoke("get_char_count").then(function (count) {
    charCount.textContent = count + " 个汉字已加载";
  }).catch(function (e) {
    charCount.textContent = "数据加载失败: " + e;
  });
}

function doSearch() {
  var input = searchInput.value.trim();
  if (!input) {
    resultsSection.style.display = "none";
    emptyState.textContent = "";
    return;
  }

  invoke("query_chars", { input: input }).then(function (results) {
    renderResults(results);
  }).catch(function (e) {
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

    var imgHtml = '<span class="info-cell">无</span>';
    if (item.has_image && item.character) {
      imgHtml = '<img id="img-' + index + '" alt="' + item.character + '" style="max-height:60px">';
    }

    tr.innerHTML =
      '<td class="index-cell">' + (index + 1) + '</td>' +
      '<td class="char-cell">' + item.character + '</td>' +
      '<td class="info-cell">拼音: ' + pinyin + '&nbsp;&nbsp;笔画: ' + strokes + '</td>' +
      '<td class="code-cell">' + (item.wubi_code || "-") + '</td>' +
      '<td class="simple-cell">' + simpleCodes + '</td>' +
      '<td class="image-cell">' + imgHtml + '</td>';

    resultsBody.appendChild(tr);

    if (item.has_image && item.character) {
      (function (imgId, character) {
        invoke("get_image_base64", { character: character }).then(function (src) {
          var img = document.getElementById(imgId);
          if (img && src) {
            img.src = src;
          }
        }).catch(function () {
          var img = document.getElementById(imgId);
          if (img) {
            img.alt = "加载失败";
          }
        });
      })("img-" + index, item.character);
    }
  });
}

searchBtn.addEventListener("click", doSearch);
searchInput.addEventListener("keydown", function (e) {
  if (e.key === "Enter") doSearch();
});

init();
