/**
 *  @file   Actions.cpp
 *  @brief  Actions Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Actions.h"

Pool pool(1L << 15); // 64K

std::stringstream sout;

std::string list_themes(std::filesystem::path &directory) {
  std::string files;
  files.reserve(12);
  if (std::filesystem::is_directory(directory)) {
    for (const auto &p : std::filesystem::directory_iterator(directory)) {
      if (p.path().extension() == ".css") {
        if (!files.empty()) {
          files += '&';
        }
        files += p.path().stem();
      }
    }
  }
  return (files);
}

void WriteHTMLHeader(Preferences &prefs) {

  auto prefs_end = prefs.end();

  std::string_view baseurl;
  auto prefs_it = prefs["baseurl"];
  if (prefs_it != prefs_end) {
    baseurl = prefs_it->second;
  }

  sout << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
          "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
          "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-tranitional.dtd\">\n"
          "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
          "<head>\n"
          "  <meta name=\"mobile-web-app-capable\" content=\"yes\" />\n"
          "  <meta name=\"apple-mobile-web-app-status-bar-style\" "
          "content=\"black-translucent\" />\n"
          "  <link rel=\"apple-touch-icon\" sizes=\"60x60\" href=\""
       << baseurl
       << "images/touch-icon-60x60.png\" />\n"
          "  <link rel=\"apple-touch-icon\" sizes=\"76x76\" href=\""
       << baseurl
       << "images/touch-icon-76x76.png\" />\n"
          "  <link rel=\"apple-touch-icon\" sizes=\"120x120\" href=\""
       << baseurl
       << "images/touch-icon-120x120.png\" /> \n"
          "  <link rel=\"SHORTCUT ICON\" href=\""
       << baseurl
       << "/images/adb.ico\" />\n"
          "  <link rel=\"stylesheet\" href=\""
       << baseurl << "css/main.css\" type=\"text/css\" />\n"
       << "  <link rel=\"stylesheet\" href=\"" << baseurl;
  prefs_it = prefs["plugins"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  prefs_it = prefs["scheme"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << ".css\" type=\"text/css\" />\n"
          "  <title>The Article dataBase - AdB</title>\n"
          "  <meta http-equiv=\"Content-Type\" content=\"text/html; "
          "charset=utf-8\" />\n"
          "  <meta name=\"description\" content=\"Homepage Christiaan "
          "Boersma\" "
          "/>\n"
          "  <meta name=\"keywords\" content=\"Christiaan, Boersma, AdB, "
          "papers\" />\n"
          "  <meta name=\"resource-type\" content=\"document\" />\n"
          "  <meta name=\"robots\" content=\"noimageclick\" />\n"
          "  <meta http-equiv=\"Cache-Control\" content=\"no-cache, no-store, "
          "must-revalidate\" />\n"
          "  <meta http-equiv=\"Pragma\" content=\"no-cache\" />\n"
          "  <meta http-equiv=\"Expires\" content=\"0\" />\n"
          "  <script type=\"text/javascript\" src=\""
       << baseurl
       << "script/interface.js\"></script>\n"
          "</head>\n"
          "<body onload=\"setfocus()\">\n";

  return;
}

void DisplayToolsForm(HTTP &http, [[maybe_unused]] Preferences &prefs) {

  sout << "<table class=\"layout tools\">\n"
          "  <tr>\n"
          "    <td>\n"
          "      <h1>Tools</h1>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      <button id=\"duplicates\" title=\"Find duplicates\" "
          "type=\"button\" onclick=\"document.location='"
       << http.self
       << "?action=duplicates'\">Duplicates</button> <button id=\"reindex\" "
          "title=\"Reindex database\" type=\"button\" "
          "onclick=\"document.location='"
       << http.self
       << "?action=reindex'\">Reindex</button> <button id=\"doi\" "
          "title=\"CrossRef DOI's\" type=\"button\" "
          "onclick=\"document.location='"
       << http.self
       << "?action=doicrossref'\">CrossRef DOI's</button> <button "
          "id=\"keywords\" title=\"List keywords\" type=\"button\" "
          "onclick=\"document.location='"
       << http.self
       << "?action=keywords'\">List Keywords</button> <button id=\"authors\" "
          "title=\"List authors\" type=\"button\" "
          "onclick=\"document.location='"
       << http.self
       << "?action=authors'\">List Authors</button>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      &nbsp;\n"
          "    </td>\n"
          "  </tr>\n"
          "</table>\n";
}

void DisplayData(HTTP &http, Preferences &prefs, Record &record,
                 Record &strings) {

  static std::string_view baseurl, adsurl, doiurl;

  static std::filesystem::path base_path;

  static int nauthors = -1;

  static bool initialized = false, table = false;

  static std::string search;

  if (!initialized) {

    auto prefs_end = prefs.end();

    auto prefs_it = prefs["baseurl"];
    if (prefs_it != prefs_end) {
      baseurl = prefs_it->second;
    }

    prefs_it = prefs["adsurl"];
    if (prefs_it != prefs_end) {
      adsurl = prefs_it->second;
    }

    prefs_it = prefs["base"];
    if (prefs_it != prefs_end) {
      base_path = prefs_it->second;
    }

    prefs_it = prefs["nauthors"];
    if (prefs_it != prefs_end) {
      std::from_chars(prefs_it->second.data(),
                      prefs_it->second.data() + prefs_it->second.size(),
                      nauthors);
    }

    prefs_it = prefs["table"];
    if (prefs_it != prefs_end && prefs_it->second == "true") {
      table = true;
    } else {
      prefs_it = prefs["doiurl"];
      if (prefs_it != prefs_end) {
        doiurl = prefs_it->second;
      }
    }

    auto get_end = http.get.end();

    auto get_it = http.get["match"];
    if (get_it != get_end) {
      search = "&match=";
      search.append(get_it->second);
      get_it = http.get["scheme"];
      if (get_it != get_end) {
        search += "&scheme=";
        search.append(get_it->second);
      }
    }

    initialized = true;
  }

  auto record_end = record.end();

  std::string_view id_str;
  auto record_it = record["id"];
  if (record_it != record_end) {
    id_str = record_it->second;
  } else {
    id_str = "-1";
  }

  std::string_view ADScode;
  record_it = record["ADScode"];
  if (record_it != record_end) {
    ADScode = record_it->second;
  }

  std::string Title;
  record_it = record["title"];
  if (record_it != record_end) {
    Title = Encoding::LaTeXDecode(record_it->second);
  } else {
    record_it = record["booktitle"];
    if (record_it != record_end) {
      Title = Encoding::LaTeXDecode(record_it->second);
    } else {
      Title = "No title";
    }
  }

  std::string Authors;
  record_it = record["author"];
  if (record_it != record_end) {
    Authors = BibTeX::SplitAuthors(Encoding::LaTeXDecode(record_it->second),
                                   nauthors, http.self);
  } else {
    record_it = record["editor"];
    if (record_it != record_end) {
      Authors = BibTeX::SplitAuthors(Encoding::LaTeXDecode(record_it->second),
                                     nauthors, http.self);
    }
  }

  std::string Journal;
  record_it = record["type"];
  if (record_it != record_end && record_it->second == "PHDTHESIS") {
    Journal = "PhD Thesis";
  } else {
    record_it = record["journal"];
    if (record_it != record_end) {
      Journal = Encoding::LaTeXDecode(record_it->second);
      auto strings_it = strings.mFields.find(Journal);
      if (strings_it != strings.end()) {
        Journal = strings_it->second;
      }
    } else {
      record_it = record["booktitle"];
      if (record_it != record_end) {
        Journal = Encoding::LaTeXDecode(record_it->second);
      } else {
        record_it = record["publisher"];
        if (record_it != record_end) {
          Journal = Encoding::LaTeXDecode(record_it->second);
        } else {
          Journal = "No journal";
        }
      }
    }
  }

  if (table) {
    sout << "  <tr>\n"
            "    <td >\n"
            "      <input class=\"select\" type=\"checkbox\" "
            "name=\"select\" value=\""
         << id_str
         << "\" />\n"
            "    </td>\n"
            "    <td>\n"
            "      <span class=\"title\" title=\"Display record &apos;"
         << Title << "&apos;\"><a href=\"" << http.self
         << "?action=open&id=" << id_str << search << "\">" << Title
         << "</a></span>\n"
            "    </td>\n"
            "    <td>\n"
            "      <span class=\"authors\">"
         << Authors
         << "</span>\n"
            "    </td>\n"
            "    <td>\n"
            "      <span class=\"journal\">"
         << Journal
         << "</span>\n"
            "    </td>\n"
            "    <td>\n"
            "      <span class=\"year\">";
    record_it = record["year"];
    if (record_it != record_end) {
      sout << Encoding::LaTeXDecode(record_it->second);
    }
    sout << "</span>\n"
         << "    </td>\n";
  } else {
    sout << "  <tr>\n"
            "    <td>\n"
            "      <input class=\"select\" type=\"checkbox\" "
            "name=\"select\" value=\""
         << id_str
         << "\" />\n"
            "    </td>\n"
            "    <td colspan=\"4\">\n"
            "      <span class=\"authors\">"
         << Authors
         << "</span>, <span class=\"title\" title=\"Display record &apos;"
         << Title << "&apos;\"><a href=\"" << http.self
         << "?action=open&id=" << id_str << search << "\">'" << Title
         << "'</a></span>, <span class=\"year\">";
    record_it = record["year"];
    if (record_it != record_end) {
      sout << Encoding::LaTeXDecode(record_it->second);
    }
    sout << "</span>, <span class=\"journal\">" << Journal << "</span>";
    record_it = record["volume"];
    if (record_it != record_end) {
      sout << ", <span class=\"volume\">"
           << Encoding::LaTeXDecode(record_it->second) << "</span>";
    }
    record_it = record["pages"];
    if (record_it != record_end) {
      sout << ", <span class=\"pages\">"
           << Encoding::LaTeXDecode(record_it->second) << "</span>";
    }
    record_it = record["doi"];
    if (record_it != record_end) {
      std::string doi = Encoding::LaTeXDecode(record_it->second);
      sout << " <a href=\"https://" << doiurl << "/" << doi << "\">" << doi
           << "</a>";
    }
    sout << "\n";
    sout << "    </td>\n";
  }

  sout << "    <td>\n";
  record_it = record["keywords"];
  if (record_it != record_end) {
    sout << "      "
         << BibTeX::SplitKeywords(Encoding::LaTeXDecode(record_it->second),
                                  http.self);
  }
  sout << "\n    </td>\n"
          "    <td>\n";
  record_it = record["ADSabstract"];
  if (record_it != record_end && record_it->second == "on" &&
      !ADScode.empty()) {
    sout << "      <button id=\"abstract\" title=\"ADS Abstract\" "
            "type=\"button\" onclick=\"gotoURL('https://"
         << adsurl << "/abs/" << ADScode << "/abstract')\">Abstract</button> ";
  }
  sout << "\n"
          "    </td>\n"
          "    <td>\n";

  record_it = record["archive"];
  if (record_it != record_end && record_it->second == "on") {
    std::filesystem::path PDFPath("archive");
    PDFPath /= id_str;
    PDFPath.replace_extension("pdf");
    if (std::filesystem::exists(base_path / PDFPath)) {

      sout << "      <button id=\"pdf\" title=\"PDF Archive\" "
              "type=\"button\" onclick=\"gotoURL('"
           << baseurl << "script/pdf.js/web/viewer.html?file=%2F"
           << PDFPath.string() << "&saveName=";

      if (!ADScode.empty()) {
        sout << Encoding::URLEncode(ADScode);
      } else if (!Title.empty()) {
        sout << Encoding::URLEncode(Title);
      } else {
        sout << id_str;
      }
      sout << ".pdf')\">Archive</button>";
    }
  } else if (!ADScode.empty()) {
    record_it = record["ADSfullpaper"];
    if (record_it != record_end && record_it->second == "on") {
      sout << "      <button id=\"pdf\" title=\"ADS PDF\" "
              "type=\"button\" onclick=\"gotoURL('https://"
           << adsurl << "/link_gateway/" << ADScode
           << "/PUB_PDF')\">Paper</button>";
    } else {
      record_it = record["URL"];
      if (record_it != record_end) {
        sout << "  <button id=\"pdf\" title=\"URL\" type=\"button\" "
                "onclick=\"gotoURL('"
             << record_it->second << "')\">URL</button>";
      }
    }
  }
  sout << "    </td>\n"
          "    <td>\n";
  record_it = record["comments"];
  if (record_it != record_end) {
    sout << "      <span title=\"" << record_it->second
         << "\"><img class=\"comments\" src=\"" << baseurl
         << "images/available.gif\" alt=\"Available\" /></span>";
  }
  sout << "\n"
          "    </td>\n"
          "    <td>\n"
          "      <button class=\"modify\" title=\"Modify\" "
          "type=\"button\" onclick=\"gotoURL('"
       << http.self << "?action=edit&id=" << id_str << search
       << "')\">Modify</button>\n"
          "    </td>\n"
          "  </tr>\n";
}

void DisplayDataList(HTTP &http, Preferences &prefs) {

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);

  auto get_end = http.get.end();

  auto get_it = http.get["sortby"];
  std::string_view srt;
  if (get_it != get_end) {
    std::string_view sortby = get_it->second;
    bool reverse = false;
    get_it = http.get["reverse"];
    if (get_it != get_end && get_it->second == "true") {
      reverse = true;
    } else {
      srt = "&reverse=true";
    }
    d.SortRecords(sortby, reverse);
    std::string::size_type p = http.uri.find("&sortby=");
    if (p != std::string::npos) {
      http.uri = http.uri.substr(0, p);
    }
  }

  BibTeX::Strings strings(pool);
  prefs_it = prefs["abbreviation"];
  if (prefs_it != prefs_end) {
    strings.Parse(base_path / prefs_it->second);
  }

  sout << "<form name=\"record\" action=\"" << http.self
       << "?action=delete&id=-1\" method=\"post\">\n"
          "<table class=\"layout list\">\n"
          "  <colgroup>\n"
          "    <col id=\"col1\" />\n"
          "    <col id=\"col2\" />\n"
          "    <col id=\"col3\" />\n"
          "    <col id=\"col4\" />\n"
          "    <col id=\"col5\" />\n"
          "    <col id=\"col6\" />\n"
          "    <col id=\"col7\" />\n"
          "    <col id=\"col8\" />\n"
          "    <col id=\"col9\" />\n"
          "    <col id=\"col10\" />\n"
          "  </colgroup>\n"
          "  <tr id=\"submenu\">\n"
          "    <td id=\"col1\" colspan=\"10\">\n"
          "      <button id=\"delete\" title=\"Delete selected records\" "
          "type=\"button\" onclick=\"if(confirm('Are you sure you want to "
          "delete the selected "
          "records?'))document.record.submit();\">Delete</button> <button "
          "id=\"reset\" title=\"Reset\" type=\"reset\">Reset</button>\n"
          "      <button id=\"BibTeX\" title=\"Export as BibTeX\" "
          "type=\"button\" onclick=\"document.record.action='"
       << http.self
       << "?action=exportBibTeX&id=-1';document.record.submit();\">"
          "Export as BibTeX</button> <button id=\"MSWord\" title=\"Export "
          "as MSWord\" type=\"button\" onclick=\"document.record.action='"
       << http.self
       << "?action=exportMSWord&id=-1';document.record.submit();\">"
          "Export as MSWord</button> <button id=\"text\" title=\"Export "
          "as Text\" type=\"button\" onclick=\"document.record.action='"
       << http.self
       << "?action=exportText&id=-1';document.record.submit();\">"
          "Export as Text</button>\n"
          "    </td>"
       << "\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <th id=\"select\">\n"
          "      <span title=\"Toggle, hot-key Alt + T, Ctrl + T "
          "(Apple)\"><input id=\"toggle\" type=\"checkbox\" "
          "onclick=\"javascript:Toggle();\" accesskey=\"t\" /></span>\n"
          "    </th>\n"
          "    <th id=\"title\">\n"
       << "      <span title=\"Sort by Title/Book\"><a href=\"" << http.uri
       << "&sortby=title" << srt
       << "\">Title/Booktitle</a></span>\n"
          "    </th>\n"
          "    <th id=\"authors\">\n"
          "      <span title=\"Sort by Authors\"><a href=\""
       << http.uri << "&sortby=author" << srt
       << "\">Author(s)</a></span>\n"
          "    </th>\n"
          "    <th id=\"journal\">\n"
          "      <span title=\"Sort by Journal\" ><a href=\""
       << http.uri << "&sortby=journal" << srt
       << "\">Journal</a></span>\n"
          "    </th>\n"
          "    <th id=\"year\">\n"
          "      <span title=\"Sort by Year\"><a href=\""
       << http.uri << "&sortby=year" << srt
       << "\">Year</a></span>\n"
          "    </th>\n"
          "    <th id=\"keywords\">\n"
          "      Keywords\n"
          "    </th>\n"
          "    <th id=\"asbstract\">\n"
          "      Abstract\n"
          "    </th>\n"
          "    <th id=\"pdf\">\n"
          "     PDF\n"
          "    </th>\n"
          "    <th id=\"comments\">\n"
          "      C\n"
          "    </th>\n"
          "    <th id=\"modify\">\n"
          "      &nbsp;\n"
          "    </th>\n"
          "  </tr>\n";

  size_t n_records = 0;

  std::string_view scheme;
  get_it = http.get["scheme"];
  if (get_it != get_end) {
    scheme = get_it->second;
  }

  std::string_view search;
  get_it = http.get["match"];
  if (get_it != get_end) {
    search = get_it->second;
  }

  if (!search.empty() && !scheme.empty()) {
    if (scheme == "any") {
      for (auto &record : d.vRecords) {
        for (const auto &field : record.mFields) {
          std::string v(Encoding::LaTeXDecode(field.second));
          if (std::search(v.begin(), v.end(), search.begin(), search.end(),
                          [](const char &c1, const char &c2) {
                            return (toupper(c1) == toupper(c2));
                          }) != v.end()) {
            DisplayData(http, prefs, record, strings.strings);
            ++n_records;
            break;
          }
        }
      }
    } else if (scheme == "author" && search[0] == '^') {
      search = search.substr(1);
      auto helper = [](std::string_view authors) -> std::string_view {
        std::string::size_type p = authors.find(" and ");
        if (p != std::string::npos) {
          return (authors.substr(0, p));
        } else {
          return (authors);
        }
      };
      for (auto &record : d.vRecords) {
        auto record_end = record.end();

        auto record_it = record["author"];
        if (record_it == record_end) {
          continue;
        }
        std::string v(helper(Encoding::LaTeXDecode(record_it->second)));
        if (std::search(v.begin(), v.end(), search.begin(), search.end(),
                        [](const char &c1, const char &c2) {
                          return (toupper(c1) == toupper(c2));
                        }) != v.end()) {
          DisplayData(http, prefs, record, strings.strings);
          ++n_records;
        }
      }
    } else {
      for (auto &record : d.vRecords) {
        auto record_end = record.end();

        auto record_it = record[scheme];
        if (record_it == record_end) {
          continue;
        }
        std::string v(Encoding::LaTeXDecode(record_it->second));
        if (std::search(v.begin(), v.end(), search.begin(), search.end(),
                        [](const char &c1, const char &c2) {
                          return (toupper(c1) == toupper(c2));
                        }) != v.end()) {
          DisplayData(http, prefs, record, strings.strings);
          ++n_records;
        }
      }
    }
  } else {
    for (auto &record : d.vRecords) {
      DisplayData(http, prefs, record, strings.strings);
      ++n_records;
    }
  }

  sout << "  <tr id=\"results\">\n"
          "    <td id=\"col1\" colspan=\"5\">\n"
       << "      " << n_records << " record";
  if (n_records > 1) {
    sout << "s";
  }
  sout << " found\n"
          "    </td>\n"
          "    <td id=\"col2\" colspan=\"5\">\n";
  if (!search.empty()) {
    sout << "      <p>\n"
            "        <span title=\"Search for "
         << search << " on ADS\"><a id=\"ads\" href=\"https://";
    prefs_it = prefs["adsurl"];
    if (prefs_it != prefs_end) {
      sout << prefs_it->second;
    }
    sout << "/search/q=" << search << "\">Search for' " << search
         << "' on ADS</a></span>\n"
            "      </p>\n";
  }
  sout << "    </td>\n"
          "  </tr>\n"
          "  <tr id=\"submenu\">\n"
          "    <td id=\"col1\" colspan=\"10\">\n"
          "      <button id=\"delete\" title=\"Delete selected records\" "
          "type=\"button\" onclick=\"if(confirm('Are you sure you want to "
          "delete the selected "
          "records?'))document.record.submit();\">Delete</button> <button "
          "id=\"reset\" title=\"Reset\" type=\"reset\">Reset</button>\n"
          "      <button id=\"BibTeX\" title=\"Export as BibTeX\" "
          "type=\"button\" onclick=\"document.record.action='"
       << http.self
       << "?action=exportBibTeX&id=-1';document.record.submit();\">"
          "Export as BibTeX</button> <button id=\"MSWord\" title=\"Export "
          "as MSWord\" type=\"button\" onclick=\"document.record.action='"
       << http.self
       << "?action=exportMSWord&id=-1';document.record.submit();\">"
          "Export as MSWord</button> <button id=\"text\" title=\"Export "
          "as Text\" type=\"button\" onclick=\"document.record.action='"
       << http.self
       << "?action=exportText&id=-1';document.record.submit();\">"
          "Export as Text</button>\n"
          "    </td>"
       << "\n"
          "  </tr>\n"
          "</table>\n"
          "</form>\n"
          "<script type=\"text/javascript\">\n"
          "  var CheckValue = false;\n"
          "  var FormName   = 'record';\n"
          "  var FieldName  = 'select';\n"
          "\n"
          "  function Toggle(){ \n"
          "    CheckValue = !CheckValue;\n"
          "    if(!document.forms[FormName])\n"
          "      return;\n"
          "    var objCheckBoxes = "
          "document.forms[FormName].elements[FieldName];\n"
          "    if(!objCheckBoxes)\n"
          "      return;\n"
          "    var countCheckBoxes = objCheckBoxes.length;\n"
          "    if(!countCheckBoxes)\n"
          "     objCheckBoxes.checked = CheckValue;\n"
          "    else\n"
          "      for(var i = 0; i < countCheckBoxes; i++)\n"
          "        objCheckBoxes[i].checked = CheckValue;\n"
          "   }\n"
          "</script>\n";
}

void WriteHTMLFooter() {
  sout << "</body>\n"
          "</html>";
}

void WriteOutput() { std::cout << sout.rdbuf(); }

void DisplayMenu(HTTP &http, Preferences &prefs) {

  const std::string_view options("any&author&keywords&comments&title");

  auto prefs_end = prefs.end();

  std::string newVersion, scheme;
  auto prefs_it = prefs["updates"];
  if (prefs_it != prefs_end && prefs_it->second == "true") {
    std::string latest = HTTP::Get(UPDATE_URL);
    if (latest != CURR_VERSION) {
      newVersion = " <button title=\"Update AdB to " + latest +
                   "\" type=\"button\" onclick=\"openWindow('" + http.self +
                   "?action=versionupdate', 500, 700)\">Update</button>";
    }
  }

  auto get_end = http.get.end();

  auto get_it = http.get["scheme"];
  if (get_it != get_end) {
    scheme = get_it->second;
  }

  sout << "<form name=\"search\" action=\"" << http.self
       << "?action=search\" method=\"get\">\n"
          "<table class=\"menu\">\n"
          "  <tr>\n"
          "    <td id=\"col1\">\n"
          "     <button id=\"new\" title=\"New\" type=\"button\" "
          "onclick=\"gotoURL('"
       << http.self
       << "?action=edit&id=-1')\">New</button> <button id=\"browse\" "
          "title=\"Browse "
          "all records\" type=\"button\" onclick=\"gotoURL('"
       << http.self
       << "?action=list')\">Browse</button> <button id=\"import\" "
          "title=\"Import\" type=\"button\" onclick=\"gotoURL('"
       << http.self
       << "?action=import')\">Import</button> <button id=\"tools\" "
          "title=\"Tools\" type=\"button\" onclick=\"gotoURL('"
       << http.self
       << "?action=tools')\">Tools</button> <button id=\"config\" "
          "title=\"Config\" type=\"button\" onclick=\"gotoURL('"
       << http.self
       << "?action=config')\">Config</button> <button id=\"info\" "
          "title=\"Info\" type=\"button\" onclick=\"gotoURL('"
       << http.self
       << "?action=info')\">Info</button>" /* <button id= \"logout\"
       " "title=\"Logout\" type=\"button\" onclick=\"gotoURL('"
       << http.self << "?action=logout')\">Logout</button>" <<
       newVersion << "\n" */
          "    </td>\n"
          "    <td id=\"col2\">\n"
          "      <input type=\"hidden\" name=\"action\" value=\"search\"/>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" "
          "class=\"search\" type=\"text\" name=\"match\" "
          "value=\"";
  get_it = http.get["match"];
  if (get_it != get_end) {
    sout << Encoding::HTMLEncode(get_it->second);
  }
  sout << "\" accesskey=\"s\"/> as \n"
       << HTML::Select(options, scheme, "scheme", "") << "\n"
       << "      <button id=\"search\" title=\"Search, hot-key Alt + S, Ctrl + "
          "S "
          "(Apple)\" type=\"submit\">Search</button>\n"
          "   </td>\n"
          "  </tr>\n"
          "</table>\n"
          "</form>\n";
}

void DisplayFooter(const std::chrono::steady_clock::time_point &t0) {

  std::chrono::system_clock::time_point clock =
      std::chrono::system_clock::now();

  time_t t = std::chrono::system_clock::to_time_t(clock);

  struct tm *timeinfo = localtime(&t);

  char year[5];

  strftime(year, 5, "%Y", timeinfo);

  std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

  float memory_usage =
      static_cast<float>(pool.size()) / static_cast<float>(pool.capacity());

  sout.setf(std::ios::fixed);
  sout.precision(1);

  sout << "<table class=\"footer\">\n"
          "  <tr>\n"
          "    <td id=\"version\">\n"
          "      " CURR_VERSION "\n"
          "    </td>\n"
          "    <td id=\"timer\">\n"
          "      page built in "
       << std::chrono::duration<float, std::milli>{t1 - t0}.count() << " ms";
  if (memory_usage > 0.0f) {
    sout << " / " << 100.0f * memory_usage << "% pool use";
  }
  sout << "\n    </td>\n"
          "    <td id=\"copyright\">\n"
          "      &#169; Christiaan Boersma 2004/"
       << year
       << "\n"
          "    </td>\n"
          "  </tr>\n"
          "</table>\n";
}

void DisplayRecord(HTTP &http, Preferences &prefs) {

  std::filesystem::path base_path;

  auto prefs_end = prefs.end();

  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    return;
  }

  std::string_view baseurl;
  prefs_it = prefs["baseurl"];
  if (prefs_it != prefs_end) {
    baseurl = prefs_it->second;
  }

  std::string_view adsurl;
  prefs_it = prefs["adsurl"];
  if (prefs_it != prefs_end) {
    adsurl = prefs_it->second;
  }

  int nauthors = -1;
  prefs_it = prefs["nauthors"];
  if (prefs_it != prefs_end) {
    std::from_chars(prefs_it->second.data(),
                    prefs_it->second.data() + prefs_it->second.size(),
                    nauthors);
  }

  auto get_end = http.get.end();

  std::string search;
  auto get_it = http.get["match"];
  if (get_it != get_end) {
    search = "&match=";
    search.append(get_it->second);
    get_it = http.get["scheme"];
    if (get_it != get_end) {
      search += "&scheme=";
      search.append(get_it->second);
    }
  }

  std::string_view id_str;
  get_it = http.get["id"];
  if (get_it != get_end) {
    id_str = get_it->second;
  } else {
    id_str = "-1";
  }

  std::string id, prev_id, next_id;

  auto records_end = d.vRecords.end();

  auto record_it = d.vRecords.begin();

  Field field_end;

  for (; record_it < records_end; record_it++) {
    field_end = record_it->end();

    auto field_it = (*record_it)["id"];
    if (field_it != field_end) {
      if ((id = field_it->second) == id_str) {
        auto next_record_it = record_it + 1;
        if (next_record_it != records_end) {
          field_it = (*next_record_it)["id"];
          if (field_it != field_end) {
            next_id = field_it->second;
          }
        }
        break;
      }
    }
    prev_id = id;
  }

  if (record_it == records_end) {
    return;
  }

  std::string ADScode;
  auto field_it = (*record_it)["ADScode"];
  if (field_it != field_end) {
    ADScode = field_it->second;
  }

  std::string Booktitle;
  field_it = (*record_it)["booktitle"];
  if (field_it != field_end) {
    Booktitle = Encoding::LaTeXDecode(field_it->second);
  }

  std::string Title;
  field_it = (*record_it)["title"];
  if (field_it != field_end) {
    Title = Encoding::LaTeXDecode(field_it->second);
  } else if (!Booktitle.empty()) {
    Title = Booktitle;
  } else {
    Title = "No title";
  }

  std::string Comments;
  field_it = (*record_it)["comments"];
  if (field_it != field_end) {
    std::string comments = Encoding::LaTeXDecode(field_it->second);
    Comments =
        "      <span title=\"" + comments + "\"><img class=\"comments\" src=\"";
    Comments = Comments.append(baseurl) +
               "images/available.gif\" alt=\"Available\" /> </span> " +
               comments;
  }

  std::string Keywords;
  field_it = (*record_it)["keywords"];
  if (field_it != field_end) {
    Keywords = BibTeX::SplitKeywords(Encoding::LaTeXDecode(field_it->second),
                                     http.self);
  }

  std::string Authors;
  field_it = (*record_it)["author"];
  if (field_it != field_end) {
    Authors = BibTeX::SplitAuthors(Encoding::LaTeXDecode(field_it->second),
                                   nauthors, http.self);
  } else {
    field_it = (*record_it)["editor"];
    if (field_it != field_end) {
      Authors = BibTeX::SplitAuthors(Encoding::LaTeXDecode(field_it->second),
                                     nauthors, http.self);
    }
  }

  std::string Journal;

  field_it = (*record_it)["type"];
  if (field_it != field_end && field_it->second == "PHDTHESIS") {
    Journal = "PhD Thesis";
  } else {
    field_it = (*record_it)["journal"];
    if (field_it != field_end) {
      Journal = Encoding::LaTeXDecode(field_it->second);
      prefs_it = prefs["abbreviation"];
      if (prefs_it != prefs_end) {
        BibTeX::Strings strings(base_path / prefs_it->second, pool);
        auto strings_it = strings.strings[Journal];
        if (strings_it != strings.strings.end()) {
          Journal = strings_it->second;
        }
      }
    } else {
      if (!Booktitle.empty()) {
        Journal = Booktitle;
      } else {
        Journal = "No journal";
      }
    }
  }

  if (!search.empty()) {
    Keywords = HTML::Highlight(Keywords, search);
    Authors = HTML::Highlight(Authors, search);
    Comments = HTML::Highlight(Comments, search);
    Title = HTML::Highlight(Title, search);
    Booktitle = HTML::Highlight(Booktitle, search);
  }

  sout << "<table class=\"layout record\">\n"
          "  <tr>\n"
          "    <td id=\"control\">\n";
  if (!prev_id.empty()) {
    sout << "       <span title=\"Previous\"><a href=\"" << http.self
         << "?action=open&id=" << prev_id
         << "\" onmouseover=\"window.status='Previous';return true\" "
            "onmouseout=\"window.status=' '\"><img src=\""
         << baseurl
         << "images/previous.gif\" alt=\"Previous\" "
            "/></a></span>";
  } else {
    sout << "       <img src=\"" << baseurl
         << "images/previous_disabled.gif\" alt=\"disabled\" />";
  }
  if (!next_id.empty()) {
    sout << " <span title=\"Next\"><a href=\"" << http.self
         << "?action=open&id=" << next_id
         << "\" onmouseover=\"window.status='Next';return true\" "
            "onmouseout=\"window.status=' '\"><img src=\""
         << baseurl << "images/next.gif\" alt=\"Next\" /></a></span>";
  } else {
    sout << "       <img src=\"" << baseurl
         << "images/next_disabled.gif\" alt=\"disabled\" />";
  }
  sout << " " << id_str << "&nbsp;\n"
       << "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      <span id=\"journal\">"
       << Journal << "</span> <span id=\"volume\">";
  field_it = (*record_it)["volume"];
  if (field_it != field_end) {
    sout << Encoding::LaTeXDecode(field_it->second);
  }
  sout << "</span> <span id=\"month\">";
  field_it = (*record_it)["month"];
  if (field_it != field_end) {
    sout << Encoding::LaTeXDecode(field_it->second);
  }
  sout << "</span> <span id=\"year\">";
  field_it = (*record_it)["year"];
  if (field_it != field_end) {
    sout << Encoding::LaTeXDecode(field_it->second);
  }
  sout << "</span> <span id=\"pages\">";
  field_it = (*record_it)["pages"];
  if (field_it != field_end) {
    sout << Encoding::LaTeXDecode(field_it->second);
  }
  sout << "</span> <span id=\"publisher\">";
  field_it = (*record_it)["publisher"];
  if (field_it != field_end) {
    sout << Encoding::LaTeXDecode(field_it->second);
  }
  sout << "</span> <span id=\"doi\">";
  field_it = (*record_it)["doi"];
  if (field_it != field_end) {
    sout << "<a href=\"https://";
    std::string doi = Encoding::LaTeXDecode(field_it->second);
    prefs_it = prefs["doiurl"];
    if (prefs_it != prefs_end) {
      sout << prefs_it->second;
    }
    sout << "/" << doi << "\">" << doi << "</a>";
  }
  sout << "</span>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      <h1>"
       << (Booktitle.empty() ? "&nbsp;" : Booktitle)
       << "</h1>\n"
          "      <h2>"
       << (Title.empty() ? "&nbsp;" : Title)
       << "</h2>\n"
          "      <span id=\"authors\">"
       << Authors
       << "</span>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      <span id=\"keywords\">"
       << Keywords << "      </span> <span ";
  field_it = (*record_it)["biblcode"];
  if (field_it != field_end) {
    sout << "id=\"Biblcode\"><b>Bibl. Code</b>: " << field_it->second;
    if (!ADScode.empty() && ADScode != field_it->second) {
      sout << "</span> <span id=\"ADSCode\"><b>ADS Code</b>: " << ADScode;
    }
  } else if (!ADScode.empty()) {
    sout << "id=\"ADSCode\"><b>ADS Code</b>: " << ADScode;
  }
  sout << "</span> <span id=\"hardcopy\">";
  field_it = (*record_it)["dossier"];
  if (field_it != field_end) {
    sout << " <b>Dossier #</b> " << Encoding::LaTeXDecode(field_it->second);
  }
  sout << "</span>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       <span id=\"adsabstract\">";
  if (!ADScode.empty()) {
    field_it = (*record_it)["ADSabstract"];
    if (field_it != field_end && field_it->second == "on") {
      sout << "<button id=\"abstract\" title=\"ADS Abstract\" "
              "type=\"button\" onclick=\"gotoURL('https://"
           << adsurl << "/abs/" << ADScode
           << "/abstract')\">Abstract</button> ";
    }
  }
  sout << "</span> <span id=\"pdf\">";
  field_it = (*record_it)["archive"];
  if (field_it != field_end && field_it->second == "on") {
    std::filesystem::path PDFPath("archive/");
    PDFPath /= id_str;
    PDFPath.replace_extension("pdf");
    std::filesystem::path FullPath(base_path);
    FullPath /= PDFPath;
    if (std::filesystem::exists(FullPath)) {
      sout << "  <button id=\"pdf\" title=\"PDF Archive\" "
              "type=\"button\" onclick=\"gotoURL('"
           << baseurl << "script/pdf.js/web/viewer.html?file=%2F"
           << PDFPath.string() << "')\">Archive</button>";
    }
  } else if (!ADScode.empty()) {
    field_it = (*record_it)["ADSfullpaper"];
    if (field_it != field_end && field_it->second == "on") {
      sout << "  <button id=\"pdf\" title=\"ADS PDF\" "
              "type=\"button\" onclick=\"gotoURL('https://"
           << adsurl << "/link_gateway/" << ADScode
           << "/PUB_PDF')\">Paper</button>";
    }
  }
  sout << "</span> <span id=\"url\">";
  field_it = (*record_it)["URL"];
  if (field_it != field_end) {
    sout << "  <button id=\"pdf\" title=\"URL\" type=\"button\" "
         << "onclick=\"gotoURL('" << field_it->second << "')\">URL</button>";
  }
  sout << "</span>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      <p id=\"comments\">\n"
          "        "
       << Comments
       << "\n"
          "      </p>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td id=\"submenu\">\n"
          "      <button id=\"bibtex\" title=\"Export BibTeX\" "
          "type=\"button\" onclick=\"gotoURL('"
       << http.self << "?action=exportBibTeX&id=" << id_str << search
       << "')\">Export as BibTeX</button> <button id=\"msword\" "
          "title=\"Export MSWord\" type=\"button\" onclick=\"gotoURL('"
       << http.self << "?action=exportMSWord&id=" << id_str << search
       << "')\">Export as MSWord</button> <button id=\"text\" "
          "title=\"Export Text\" type=\"button\" onclick=\"gotoURL('"
       << http.self << "?action=exportText&id=" << id_str << search
       << "')\">Export as Text</button> &mdash; <button id=\"modify\" "
          "title=\"Modify\" type=\"button\" onclick=\"gotoURL('"
       << http.self << "?action=edit&id=" << id_str << search
       << "')\">Modify</button> <button id=\"delete\" title=\"Delete this "
          "record\" type=\"button\" onclick=\"if(confirm('Are you sure you "
          "want to delete this record?'))document.location='"
       << http.self << "?action=delete&id=" << id_str << search
       << "';\">Delete</button>\n"
          "    </td>\n"
          "  </tr>\n"
          "</table>\n";
}

std::vector<std::string> split_on_comma(std::string_view sv) {

  std::vector<std::string> v;

  if (sv.empty()) {
    return (v);
  }

  size_t b = 0, e = 0;
  const char *p = sv.data(), *q = p;
  while (*p) {
    if (*p == ',') {
      v.emplace_back(Encoding::LaTeXDecode(std::string_view(q + b, e - b)));
      b = e = 0;
      q = ++p;
      continue;
    } else if (isspace(*p)) {
      if ((e - b) == 0) {
        ++b;
        ++e;
      }
    } else {
      e = p - q + 1;
    }
    ++p;
  }
  if (e > b) {
    v.emplace_back(Encoding::LaTeXDecode(std::string_view(q + b, e - b)));
  }

  return (v);
}

std::vector<std::string> split_on_and(std::string_view sv) {

  std::vector<std::string> v;

  const char *b = sv.data(), *e;

  int len, e_len, b_len = strlen(b);
  while ((e = strstr(b, " and "))) {
    e_len = strlen(e);
    len = b_len - e_len;
    v.emplace_back(Encoding::LaTeXDecode(std::string_view(b, len)));
    b = e + 5;
    b_len -= (len + 5);
  }
  b_len = strlen(b);
  if (b_len) {
    v.emplace_back(Encoding::LaTeXDecode(std::string_view(b, b_len)));
  }

  return (v);
}

void DisplayRecordForm(HTTP &http, Preferences &prefs) {

  auto get_end = http.get.end();

  auto get_it = http.get["id"];
  std::string_view id_str;
  if (get_it != get_end) {
    id_str = get_it->second;
  }

  std::string search;
  get_it = http.get["match"];
  if (get_it != get_end) {
    search = get_it->second;
  }

  std::string_view scheme;
  get_it = http.get["scheme"];
  if (get_it != get_end) {
    scheme = get_it->second;
  }

  if (!search.empty()) {
    search = "&match=" + search + "&scheme=";
    search.append(scheme);
  }

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    return;
  }

  Record *record_it;

  std::string query_ads, query_doi;
  if (id_str != "-1") {
    record_it = &(*d.GetRecord(id_str));
  } else {
    record_it = &(http.post);
    query_ads = " <button id=\"adsquery\" title=\"Query\" type=\"button\" "
                "onclick=\"document.record.action='" +
                http.self +
                "?action=queryADS&id=-1';document.record.submit();\">Query "
                "ADS</button> ";
    query_doi = " <button id=\"doiquery\" title=\"Query\" type=\"button\" "
                "onclick=\"document.record.action='" +
                http.self +
                "?action=queryDOI&id=-1';document.record.submit();\">Query "
                "DOI</button> ";
  }

  auto field_end = record_it->end();

  std::string_view type;
  auto field_it = (*record_it)["type"];
  if (field_it != field_end) {
    type = field_it->second;
  }

  std::string_view options =
      "ARTICLE&INPROCEEDINGS&PROCEEDINGS&BOOK&BOOKLET&INBOOK&"
      "INCOLLECTION&"
      "MASTERTHESIS&PHDTHESIS&TECHREPORT&MISC&MANUAL&UNPUBLISHED&"
      "SOFTWARE";

  auto uniq = d.UniqueValuesForKey("keywords", split_on_comma);

  std::string JSArray(Javascript::Array(uniq));

  sout << "<script type=\"text/javascript\">\n"
          "\n"
          "  var choices = "
       << JSArray
       << "\n"
          "\n"
          "</script>\n"
          "\n"
          "<form name=\"record\" action=\""
       << http.self << "?action=update&id=" << id_str << search
       << "\" method=\"post\" enctype=\"multipart/form-data\">\n"
          "<input name=\"id\" type=\"hidden\" value=\""
       << id_str
       << "\" />\n"
          "<table class=\"layout edit\">\n"
          "  <tr>\n"
          "    <td id=\"submenu\" colspan=\"2\">\n"
          "      <button id=\"save\" title=\"Save\" "
          "type=\"submit\">Save</button> <button id=\"reset\" "
          "title=\"Reset\" "
          "type=\"reset\">Reset</button> <button id=\"delete\" "
          "title=\"Delete "
          "this record\" type=\"button\" "
          "onclick=\"javascript:if(confirm('Are "
          "you sure you want to delete this record?'))document.location='"
       << http.self << "?action=delete&id=" << id_str
       << "';\">Delete</button>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h1>Modify</h1>\n"
          "      <h2>AdB</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       id\n"
          "    </td>\n"
          "    <td>\n"
          "      "
       << id_str << (id_str == "-1" ? "<i>(new)</i>" : "")
       << "\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "      <td>\n"
          "       Bibl. Code\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"biblcode\" class=\"config\" "
          "name=\"biblcode\" type=\"text\" value=\"";
  field_it = (*record_it)["biblcode"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       ADS Code\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"ADScode\" class=\"config\" "
          "name=\"ADScode\" type=\"text\" value=\"";
  field_it = (*record_it)["ADScode"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />" << query_ads
       << "\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       DOI Code\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"doi\" class=\"config\" name=\"doi\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["doi"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />" << query_doi
       << "\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Keyword(s)\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"keywords\" class=\"config\" "
          "name=\"keywords\" type=\"text\" value=\"";
  field_it = (*record_it)["keywords"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" onkeydown=\"return checkchar(event);\" "
          "onkeyup=\"checkinput(this);\" onblur=\"hide();\" "
          "autocomplete=\"off\" />\n"
          "      <div class=\"autocomplete\" id=\"autocomplete\" "
          "onmouseover=\"keepfocus=true;\" onmouseout=\"keepfocus=false\">\n"
          "      </div>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>BiBTeX</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Type\n"
          "    </td>\n"
          "    <td>\n"
          "      "
       << HTML::Select(options, type, "type", "evalType(this)")
       << "\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Title\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"title\" class=\"config\" "
          "name=\"title\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["title"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Book title\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"booktitle\" class=\"config\" "
          "name=\"booktitle\" type=\"text\" value=\"";
  field_it = (*record_it)["booktitle"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Editor\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"editor\" class=\"config\" "
          "name=\"editor\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["editor"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Organization\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"organization\" class=\"config\" "
          "name=\"organization\" type=\"text\" value=\"";
  field_it = (*record_it)["organization"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Address\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"address\" class=\"config\" "
          "name=\"address\" type=\"text\" value=\"";
  field_it = (*record_it)["address"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Series\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"series\" class=\"config\" "
          "name=\"series\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["series"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Edition\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"edition\" class=\"config\" "
          "name=\"edition\" type=\"text\" value=\"";
  field_it = (*record_it)["edition"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       How published\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"howpublished\" class=\"config\" "
          "name=\"howpublished\" type=\"text\" value=\"";
  field_it = (*record_it)["howpublished"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Chapter\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"chapter\" class=\"config\" "
          "name=\"chapter\" type=\"text\" value=\"";
  field_it = (*record_it)["chapter"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       School\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"school\" class=\"config\" "
          "name=\"school\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["school"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Institution\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"institution\" class=\"config\" "
          "name=\"institution\" type=\"text\" value=\"";
  field_it = (*record_it)["institution"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Author(s)\n"
          "    </td>\n"
          "    <td>\n"
          "      <textarea autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"authors\" class=\"config\" "
          "name=\"author\">";
  field_it = (*record_it)["author"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "</textarea>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Journal\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"journal\" class=\"config\" "
          "name=\"journal\" type=\"text\" value=\"";
  field_it = (*record_it)["journal"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Volume\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"volume\" class=\"config\" "
          "name=\"volume\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["volume"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Number\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"number\" class=\"config\" "
          "name=\"number\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["number"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Pages\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"pages\" class=\"config\" "
          "name=\"pages\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["pages"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Publisher\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"publisher\" class=\"config\" "
          "name=\"publisher\" type=\"text\" value=\"";
  field_it = (*record_it)["publisher"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Month\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"month\" class=\"config\" "
          "name=\"month\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["month"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Year\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"year\" class=\"config\" name=\"year\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["year"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Summary\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"summary\" class=\"config\" "
          "name=\"summary\" type=\"text\" value=\"";
  field_it = (*record_it)["summary"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Note\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"note\" class=\"config\" name=\"note\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["note"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>ADS</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       ADS Services\n"
          "    </td>\n"
          "    <td>\n"
          "      <input name=\"ADSabstract\" type=\"checkbox\"";
  field_it = (*record_it)["ADSabstract"];
  if (field_it != field_end && field_it->second == "on") {
    sout << " checked=\"checked\"";
  }
  sout << " /> Abstract (<i>link</i>) <input name=\"ADSfullpaper\" "
          "type=\"checkbox\"";
  field_it = (*record_it)["ADSfullpaper"];
  if (field_it != field_end && field_it->second == "on") {
    sout << " checked=\"checked\"";
  }
  sout << " /> PDF (<i>link</i>)\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>PDF</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       URL\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"url\" class=\"config\" name=\"URL\" "
          "type=\"text\" value=\"";
  field_it = (*record_it)["URL"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Archive\n"
          "    </td>\n"
          "    <td>\n"
          "      <input name=\"archive\" type=\"checkbox\"";
  field_it = (*record_it)["archive"];
  if (field_it != field_end && field_it->second == "on") {
    sout << " checked=\"checked\"";
  }
  sout << " /> Archive";
  if (field_it != field_end && field_it->second == "on") {
    std::filesystem::path PDFPath(base_path / "archive" / id_str);
    PDFPath.replace_extension("pdf");
    sout << "d <i>" + HTML::Filesize(PDFPath);
  } else {
    sout << " (<i>downloads from ADS, or, when specified uses Upload "
            "file</i>)\n";
  }
  sout << "</i>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Upload\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"pdf\" class=\"config\" name=\"pdf\" "
          "type=\"file\" accept=\"application/pdf\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>Hardcopy</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Dossier\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" id=\"dossier\" class=\"config\" "
          "name=\"dossier\" type=\"text\" value=\"";

  field_it = (*record_it)["dossier"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>Extra</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "       Comments\n"
          "    </td>\n"
          "    <td>\n"
          "      <textarea id=\"comments\" class=\"config\" rows=\"5\" "
          "name=\"comments\">";

  field_it = (*record_it)["comments"];
  if (field_it != field_end) {
    sout << Encoding::HTMLEncode(field_it->second);
  }
  sout << "</textarea>\n"
          "    </td>\n"
          "  </tr>\n";

  static constexpr const char *usedKeys[] = {
      "id",           "biblcode",    "ADScode",      "doi",          "keywords",
      "type",         "author",      "title",        "booktitle",    "editor",
      "organization", "address",     "edition",      "howpublished", "chapter",
      "school",       "institution", "author",       "journal",      "volume",
      "number",       "pages",       "publisher",    "month",        "year",
      "summary",      "note",        "ADSfullpaper", "ADSabstract",  "URL",
      "dossier",      "comments",    "archive",      "series"};

  for (const auto *s : usedKeys) {
    field_it = (*record_it)[s];
    if (field_it != field_end) {
      record_it->mFields.erase(field_it);
    }
  }

  for (const auto &field_it : record_it->mFields) {

    sout << "  <tr>\n"
            "    <td>\n"
         << field_it.first
         << "\n"
            "    </td>\n"
            "    <td>\n"
            "      <input autocorrect=\"off\" autocapitalize=\"off\" "
            "autocomplete=\"off\" id=\""
         << field_it.first << "\" class=\"config\" name=\"" << field_it.first
         << "\" type=\"text\" value=\"" << Encoding::HTMLEncode(field_it.second)
         << "\" /> <button id=\"del\" title=\"Del\" type=\"button\" "
            "onclick=\"delExtraKeyValuePair(this);\">Del</button>\n"
            "    </td>\n"
            "  </tr>\n";
  }

  sout << "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <button id=\"add\" title=\"Add\" type=\"button\" "
          "onclick=\"addExtraKeyValuePair();\">Add</button>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td id=\"submenu\" colspan=\"2\">\n"
          "      <button id=\"save\" title=\"Save\" "
          "type=\"submit\">Save</button> <button id=\"reset\" "
          "title=\"Reset\" "
          "type=\"reset\">Reset</button> <button id=\"delete\" "
          "title=\"Delete "
          "this record\" type=\"button\" "
          "onclick=\"javascript:if(confirm('Are "
          "you sure you want to delete this record?'))document.location='"
       << http.self << "?action=delete&id=" << id_str
       << "';\">Delete</button>\n"
          "    </td>\n"
          "  </tr>\n"
          "</table>\n"
          "</form>\n\n"
          "\n"
          "<script type=\"text/javascript\">\n"
          "\n"
          "  evalType(document.forms.record.type);\n"
          "\n"
          "  function addExtraKeyValuePair(){\n"
          "    var key    = prompt('Please provide a key', 'your key');\n"
          "    if(key != '' && key != null){\n"
          "      var tables = document.getElementsByTagName('table');\n"
          "      var table  = tables[tables.length - 2];\n"
          "      var row    = "
          "table.tBodies[0].insertBefore(document.createElement('tr'), "
          "table.tBodies[0].rows[table.tBodies[0].rows.length - 2]);\n"
          "      var td     = "
          "row.appendChild(document.createElement('td'));\n"
          "      var text   = "
          "td.appendChild(document.createTextNode(key));\n"
          "      var td     = "
          "row.appendChild(document.createElement('td'));\n"
          "      td.innerHTML = '<input autocorrect=\"off\" "
          "autocapitalize=\"off\" autocomplete=\"off\" class=\"config\" "
          "name=\"' + key + '\" type=\"text\" value=\"\"></input> <button "
          "title=\"Del\" type=\"button\" "
          "onclick=\"delExtraKeyValuePair(this);\">Del</button>';\n"
          "      return(true);\n"
          "  }\n"
          "}\n"
          "\n"
          "function delExtraKeyValuePair(element){\n"
          "  "
          "element.parentNode.parentNode.parentNode.removeChild(element."
          "parentNode.parentNode);\n"
          "}\n"
          "\n"
          "</script>\n";
}

void Update(HTTP &http, Preferences &prefs) {

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  auto post_end = http.post.end();
  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    return;
  }

  auto post_it = http.post["id"];
  if (post_it == post_end) {
    DisplayRecord(http, prefs);
  }

  d.SetRecord(http.post, post_it->second);
  d.Commit();

  http.get.mFields.at("id") = http.post.mFields.at("id");

  std::filesystem::path TargetPath = base_path / "archive" / post_it->second;
  TargetPath.replace_extension("pdf");

  post_it = http.post["archive"];
  if (post_it != post_end && post_it->second == "on") {
    if (http.file_data.empty() && !std::filesystem::exists(TargetPath)) {
      std::string command(base_path.string() + "csh/AdBArchiver.csh " +
                          base_path.string() + " " + "'https://");
      prefs_it = prefs["adsurl"];
      if (prefs_it != prefs_end) {
        command = command.append(prefs_it->second);
      }
      command += "/pdf/";
      prefs_it = prefs["AScode"];
      if (prefs_it != prefs_end) {
        command = command.append(prefs_it->second);
      }
      command += "' " + TargetPath.string();

      if (system(command.c_str()) == -1) {
        return;
      }

      if (std::filesystem::exists(TargetPath) &&
          std::filesystem::file_size(TargetPath) == 0) {
        if (!std::filesystem::remove(TargetPath)) {
          return;
        }
      }
    } else if (!http.file_data.empty()) {
      std::ofstream ofstr(TargetPath, std::ios::binary);

      ofstr.write(http.file_data.data(), http.file_data.length());

      ofstr.close();
    }
  } else if (std::filesystem::exists(TargetPath)) {

    if (!std::filesystem::remove(TargetPath)) {
      return;
    }
  }
  DisplayRecord(http, prefs);
}

void QueryADS(HTTP &http, Preferences &prefs) {

  auto post_end = http.post.end();

  std::string_view ADScode;
  auto post_it = http.post["ADScode"];
  if (post_it != post_end) {
    ADScode = post_it->second;
  }

  auto prefs_end = prefs.end();

  std::string_view adstoken;
  auto prefs_it = prefs["adstoken"];
  if (prefs_it != prefs_end) {
    adstoken = prefs_it->second;
  }

  std::string_view pem;
  prefs_it = prefs["pem"];
  if (prefs_it != prefs_end) {
    pem = prefs_it->second;
  }

  std::string post("{\"bibcode\":[\"");
  post = post.append(ADScode) + "\"]}";

  std::string authorization("Authorization: Bearer ");
  authorization.append(adstoken);

  std::string content_type("Content-Type: application/json");

  std::vector<std::string_view> headers = {authorization, content_type};

  std::string bibtex = http.SecurePost(ADS_API_URL, post, headers, 443, pem);

  rapidjson::Document json;
  if (json.Parse(bibtex.c_str()).HasParseError() || !json.HasMember("export")) {
    pool.begin();
    pool << ADScode << " (not found)";
    http.post.mFields["ADScode"] = pool.sv();
    DisplayRecordForm(http, prefs);
    return;
  }

  size_t size = 0;

  http.post = BibTeX::Parse(json["export"].GetString(), size, pool);

  http.post.mFields["ADScode"] = http.post.mFields["biblcode"];

  DisplayRecordForm(http, prefs);
}

void QueryDOI(HTTP &http, Preferences &prefs) {

  std::string_view doi;
  auto post_it = http.post["doi"];
  if (post_it != http.post.end()) {
    doi = post_it->second;
  }

  auto prefs_end = prefs.end();

  std::string url;
  auto prefs_it = prefs["doilookupurl"];
  if (prefs_it != prefs_end) {
    url = prefs_it->second;
  }
  url += "/v1/works/" + Encoding::URLEncode(doi) + "/transform";

  std::string_view pem;
  post_it = prefs["pem"];
  if (post_it != prefs_end) {
    pem = post_it->second;
  }

  std::vector<std::string_view> headers = {
      "Accept: text/bibliography; style=bibtex"};

  std::string bibtex(http.SecureGet(url, headers, 443, pem));

  size_t size = 0;

  http.post = BibTeX::Parse(bibtex, size, pool);

  auto node = http.post.mFields.extract("DOI");
  if (!node.empty()) {
    node.key() = "doi";
    http.post.mFields.insert(std::move(node));
  } else {
    pool.begin();
    pool << doi << " (not found)";
    http.post.mFields["doi"] = pool.sv();
  }

  DisplayRecordForm(http, prefs);
}

void DisplayInfo([[maybe_unused]] HTTP &http, Preferences &prefs) {

  time_t clock = time(nullptr);

  struct tm *timeinfo = localtime(&clock);

  char year[5];

  strftime(year, 5, "%Y", timeinfo);

  std::string_view baseurl;
  auto prefs_it = prefs["baseurl"];
  if (prefs_it != prefs.end()) {
    baseurl = prefs_it->second;
  }

  sout << "<table class=\"layout info\">\n"
          "  <tr>\n"
          "    <td id=\"col1\">\n"
          "      <h1>Info</h1>\n"
          "      <h2>General</h2>\n"
          "    </td>\n"
          "    <td id=\"col2\" rowspan=\"2\">\n"
       << "      <img src=\"" << baseurl
       << "images/logo.png\" alt=\"logo\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <p id=\"disclaimer\">\n"
          "        AdB is an article database writen in C/C++ and "
          "developed by "
          "Dr.&nbsp;C.&nbsp;Boersma. This is "
       << CURR_VERSION ". The latest version can be downloaded <a href=\""
       << std::string(LATEST_URL)
       << "\">here</a>. The developer can be contacted at <a "
          "href=\"mailto:" CONTACT "\">" CONTACT "</a>\n"
          "      </p>\n"
          "      <p id=\"copyright\">\n"
          "        Christiaan Boersma 2004 - "
       << year
       << "\n"
          "      </p>\n"
          "    </td>\n"
          "  </tr>\n"
          "</table>\n";
}

void DisplaySplash([[maybe_unused]] HTTP &http, Preferences &prefs) {

  auto prefs_it = prefs["splash"];
  if (prefs_it != prefs.end() && prefs_it->second == "true") {
    sout << "<table class=\"layout splash\">\n"
            "  <tr>\n"
            "    <td id=\"image\">\n"
            "    </td>\n"
            "  </tr>\n"
            "</table>\n";

  } else {
    DisplayDataList(http, prefs);
  }
}

void DisplayConfigForm(HTTP &http, Preferences &prefs) {

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  std::string_view plugins;
  prefs_it = prefs["plugins"];
  if (prefs_it != prefs_end) {
    plugins = prefs_it->second;
  }

  std::string_view scheme;
  prefs_it = prefs["scheme"];
  if (prefs_it != prefs_end) {
    scheme = prefs_it->second;
  }

  sout << "<form name=\"config\" action=\"" << http.self
       << "?action=config\" method=\"post\">\n"
          "<input type=\"hidden\" name=\"save\" value=\"true\" />\n"
          "<table class=\"layout config\">\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h1>Configuration</h1>\n"
          "      <h2>General</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td width=\"20%\">\n"
          "      Base directory \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"base\" "
          "value=\""
       << base_path.string()
       << "\" /> \n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td width=\"20%\">\n"
          "      Database file \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"data\" "
          "value=\"";
  prefs_it = prefs["data"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> ";
  if (prefs_it != prefs_end) {
    std::filesystem::path DataPath(base_path / prefs_it->second);
    sout << HTML::Filesize((DataPath));
  }
  sout << "\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td width=\"20%\">\n"
          "      Abbreviation file \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"abbreviation\" value=\"";
  prefs_it = prefs["abbreviation"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> ";
  if (prefs_it != prefs_end) {
    std::filesystem::path AbbreviationPath(base_path / prefs_it->second);
    sout << HTML::Filesize(AbbreviationPath);
  }
  sout << "\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Themes directory \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"plugins\" "
          "value=\""
       << plugins
       << "\" /> \n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Base URL \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"baseurl\" "
          "value=\"";
  prefs_it = prefs["baseurl"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Check for updates\n"
          "    </td>\n"
          "    <td>\n"
          "      <input name=\"updates\" type=\"checkbox\" value=\"true\"";
  prefs_it = prefs["updates"];
  if (prefs_it != prefs_end && prefs_it->second == "true") {
    sout << " checked=\"checked\"";
  }
  sout << " />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Administrator\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"administrator\" value=\"";
  prefs_it = prefs["administrator"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>ADS</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Url\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"adsurl\" "
          "value=\"";
  prefs_it = prefs["adsurl"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> <i>no leading http(s)</i>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Token\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"adstoken\" value=\"";
  prefs_it = prefs["adstoken"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> <a "
          "href=\"https://ui.adsabs.harvard.edu/user/account/login\"><i>get "
          "your token</i></a>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>DOI</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Url for display\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"doiurl\" "
          "value=\"";
  prefs_it = prefs["doiurl"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> <i>no leading http(s)</i>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "     Url for lookup\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"doilookupurl\" value=\"";
  prefs_it = prefs["doilookupurl"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> <i>no leading http(s)</i>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      CrossRef score\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"doicrossrefscore\" value=\"";
  prefs_it = prefs["doicrossrefscore"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>SSL</h2>\n"
          "    </td>\n"
          "  <tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      PEM\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" name=\"pem\" "
          "value=\"";
  prefs_it = prefs["pem"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>Look</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Splash screen at startup\n"
          "    </td>\n"
          "    <td>\n"
          "      <input name=\"splash\" type=\"checkbox\" value=\"true\"";
  prefs_it = prefs["splash"];
  if (prefs_it != prefs_end && prefs_it->second == "true") {
    sout << " checked=\"checked\"";
  }
  sout << "/>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Theme\n"
          "    </td>\n"
          "    <td>\n";
  if (!plugins.empty()) {
    std::filesystem::path plugin_path(base_path / plugins);
    sout << HTML::Select(list_themes(plugin_path), scheme, "scheme", "");
  } else {
    sout << "no plugins available";
  }
  sout << "\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Number authors to Display\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"nauthors\" value=\"";
  prefs_it = prefs["nauthors"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> <i>-1 is all</i>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      View as Table\n"
          "    </td>\n"
          "    <td>\n"
          "      <input name=\"table\" type=\"checkbox\" value=\"true\"";
  prefs_it = prefs["table"];
  if (prefs_it != prefs_end && prefs_it->second == "true") {
    sout << " checked=\"checked\"";
  }
  sout << " />\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>Export</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td width=\"20%\">\n"
          "      Key code\n"
          "    </td>\n"
          "    <td>\n"
          "      <input type=\"radio\" name=\"key\" value=\"biblcode\"";
  prefs_it = prefs["key"];
  if (prefs_it != prefs_end && prefs_it->second == "biblcode") {
    sout << " checked=\"checked\"";
  }
  sout << "\" /> Bibl.\n"
          "      <input type=\"radio\" name=\"key\" value=\"ADScode\"";
  if (prefs_it != prefs_end && prefs_it->second == "ADScode") {
    sout << " checked=\"checked\"";
  }
  sout << "\" /> ADS\n"
          "      <input type=\"radio\" name=\"key\" value=\"doi\"";
  if (prefs_it != prefs_end && prefs_it->second == "doi") {
    sout << " checked=\"checked\"";
  }
  sout << "\" /> DOI\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td width=\"20%\">\n"
          "      BibTeX file \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"bibtex\" "
          "value=\"";
  prefs_it = prefs["bibtex"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> ";
  if (prefs_it != prefs_end) {
    std::filesystem::path BibTeXPath(base_path / prefs_it->second);
    sout << HTML::Filesize(BibTeXPath);
  }
  sout << "\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td width=\"20%\">\n"
          "      MSWord file \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"msword\" "
          "value=\"";
  prefs_it = prefs["msword"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> ";
  if (prefs_it != prefs_end) {
    std::filesystem::path MSWordPath(base_path / prefs_it->second);
    sout << HTML::Filesize(MSWordPath);
  }
  sout << "\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td width=\"20%\">\n"
          "      Text file \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"text\" "
          "value=\"";
  prefs_it = prefs["text"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> ";
  if (prefs_it != prefs_end) {
    std::filesystem::path TextPath(base_path / prefs_it->second);
    sout << HTML::Filesize(TextPath);
  }
  sout << "\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Translate abbreviations\n"
          "    </td>\n"
          "    <td>\n"
          "      <input name=\"translate\" type=\"checkbox\" value=\"true\"";
  prefs_it = prefs["translate"];
  if (prefs_it != prefs_end && prefs_it->second == "true") {
    sout << " checked=\"checked\"";
  }

  sout << "\" /> BibTeX\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td colspan=\"2\">\n"
          "      <h2>Security</h2>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Credential expiration period\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"expire\" "
          "value=\"";
  prefs_it = prefs["expire"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\"/> seconds\n"
          "    </td>\n"
          "  </tr>\n"
          "   <tr>\n"
          "     <td colspan=\"2\">\n"
          "       &nbsp;\n"
          "     </td>\n"
          "   </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Username \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"text\" "
          "name=\"username\" value=\"";
  prefs_it = prefs["username"];
  if (prefs_it != prefs_end) {
    sout << prefs_it->second;
  }
  sout << "\" /> \n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      New password \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"password\" "
          "name=\"password\" /> \n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      Confirm password \n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"config\" type=\"password\" "
          "name=\"confirmpassword\" /> \n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td id=\"submenu\" colspan=\"2\">\n"
          "      <button title=\"Save config\" type=\"button\" "
          "onclick=\"if(document.config.password.value==''||document."
          "config."
          "password.value==document.config.confirmpassword.value)document."
          "config.submit();else{alert('Confirmation password does not "
          "match "
          "new "
          "password, please try again');}\">Save</button> <button "
          "title=\"Undo "
          "changes\" type=\"reset\" onmouseover=\"window.status='Undo "
          "changes';return true\">Undo</button>\n"
          "    </td>\n"
          "  </tr>\n"
          "</table>\n"
          "</form>\n";
}

void DisplayImportForm(HTTP &http, [[maybe_unused]] Preferences &prefs) {
  sout << "<form name=\"importBibTeX\" action=\"" << http.self
       << "?action=importBibTeX\" enctype=\"multipart/form-data\" "
          "method=\"post\">\n"
          "<table class=\"layout import\">\n"
          "<tr>\n"
          "  <td colspan=\"2\">\n"
          "    <h1>Import</h1>\n"
          "    <h2>BibTeX</h2> \n"
          "  </td>\n"
          "</tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      BibTeX File\n"
          "    </td>\n"
          "    <td>\n"
          "      <input autocorrect=\"off\" autocapitalize=\"off\" "
          "autocomplete=\"off\" class=\"file\" type=\"file\" "
          "name=\"upload\" />\n"
          "    </td>\n"
          "  </tr>\n"
          "<tr>\n"
          "  <td id=\"submenu\" colspan=\"2\">\n"
          "    <button id=\"importBibTeX\" title=\"Import\" "
          "type=\"submit\">Import BibTeX</button> <button "
          "id=\"reset\" "
          "title=\"Reset\" type=\"reset\">Reset</button>\n"
          "  </td>\n"
          "</tr>\n"
          "</table>\n"
          "</form>\n";
}

void DisplayReindexForm([[maybe_unused]] HTTP &http, Preferences &prefs) {

  sout << "<table class=\"layout reindex\">\n"
          "  <tr>\n"
          "    <td id=\"col1\" colspan=\"3\">\n"
          "      <h1>Reindex</h1>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <th id=\"old\">\n"
          "      old\n"
          "    </th>\n"
          "    <th id=\"new\">\n"
          "      new\n"
          "    </th>\n"
          "    <th id=\"pdf\">\n"
          "      PDF\n"
          "    </th>\n"
          "  </tr>\n";

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    return;
  }

  if (!d.ReindexRecords([&base_path](Record &record, long id) {
        sout << "  <tr>\n"
                "    <td>\n"
             << record["id"]->second
             << "\n"
                "    </td>\n"
                "    <td>\n"
             << id
             << "\n"
                "    </td>\n"
                "    <td>\n";

        std::filesystem::path SourcePath(base_path / "archive" /
                                         record["id"]->second);
        SourcePath.replace_extension("pdf");

        auto record_it = record["archive"];
        if (record_it != record.end() && record_it->second == "on" &&
            std::filesystem::exists(SourcePath)) {
          std::filesystem::path DestinationPath(base_path / "archive/" /
                                                std::to_string(id));
          DestinationPath.replace_extension(".pdf");
          std::filesystem::rename(SourcePath, DestinationPath);
          sout << "      yes\n";
        } else {
          sout << "      no\n";
        }

        sout << "    </td>\n"
                "  </tr>\n";

        return true;
      })) {
    return;
  }

  d.Commit();

  sout << "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td>\n"
          "      &nbsp;\n"
          "    </td>\n"
          "    <td>\n"
          "      &nbsp;\n"
          "    </td>\n"
          "    <td>\n"
          "      &nbsp;\n"
          "    </td>\n"
          "  </tr>\n"
          "</table>\n";
}

void DisplayKeywords(HTTP &http, Preferences &prefs) {

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    return;
  }

  auto uniq = d.UniqueValuesForKey("keywords", split_on_comma);

  size_t n_uniq = uniq.size();

  sout << "<table class=\"layout keywords\">\n"
          "  <tr>\n"
          "    <td id=\"col1\" colspan=\"3\">\n"
          "      <h1>Keywords"
       << " (" << n_uniq
       << ")</h1>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <th id=\"col1\">\n"
          "      Keyword\n"
          "    </th>\n"
          "    <th id=\"col2\">\n"
          "      Keyword\n"
          "    </th>\n"
          "    <th id=\"col3\">\n"
          "      Keyword\n"
          "    </th>\n"
          "  </tr>\n";

  unsigned long n_per_col = 0, col_per_row = 0;

  if (n_uniq > 20) {
    col_per_row = 3;
  } else if (n_uniq > 10) {
    col_per_row = 2;
  } else {
    col_per_row = 1;
  }

  if (n_uniq < 10) {
    n_per_col = n_uniq;
  } else if (n_uniq > 30) {
    n_per_col = static_cast<unsigned long>(
        ceil(static_cast<double>(n_uniq) / static_cast<double>(col_per_row)));
  }

  std::string Keyword;
  for (unsigned i = 0; i < n_per_col; i++) {
    sout << "  <tr>\n";
    for (unsigned j = 0; j < 3; j++) {
      Keyword = "&nbsp;";
      if (j < col_per_row) {
        size_t offset = (j * n_per_col) + i;
        if (offset < n_uniq) {
          Keyword = Encoding::LaTeXDecode(uniq.at(offset));
          Keyword = "<a href=\"" + http.self +
                    "?action=search&match=" + Encoding::URLEncode(Keyword) +
                    "&scheme=keywords\">" + Keyword + "</a>";
        }
      }
      sout << "    <td>\n"
           << "      " << Keyword << "\n"
           << "    </td>\n";
    }
    sout << "  </tr>\n";
  }
  sout << "</table>\n";
}

void DisplayAuthors(HTTP &http, Preferences &prefs) {

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    return;
  }

  auto uniq = d.UniqueValuesForKey("author", split_on_and);

  size_t n_uniq = uniq.size();

  sout << "<table class=\"layout authors\">\n"
          "  <tr>\n"
          "    <td id=\"col1\" colspan=\"3\">\n"
          "      <h1>Authors"
       << " (" << n_uniq
       << ")</h1>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <th id=\"col1\">\n"
          "      Keyword\n"
          "    </th>\n"
          "    <th id=\"col2\">\n"
          "      Keyword\n"
          "    </th>\n"
          "    <th id=\"col3\">\n"
          "      Keyword\n"
          "    </th>\n"
          "  </tr>\n";

  unsigned long n_per_col = 0, col_per_row = 0;

  if (n_uniq > 20) {
    col_per_row = 3;
  } else if (n_uniq > 10) {
    col_per_row = 2;
  } else {
    col_per_row = 1;
  }

  if (n_uniq < 10) {
    n_per_col = n_uniq;
  } else if (n_uniq > 30) {
    n_per_col = static_cast<unsigned long>(
        ceil(static_cast<double>(n_uniq) / static_cast<double>(col_per_row)));
  }

  std::string Author;
  for (unsigned i = 0; i < n_per_col; i++) {
    sout << "  <tr>\n";
    for (unsigned j = 0; j < 3; j++) {
      Author = "&nbsp;";
      if (j < col_per_row) {
        size_t offset = (j * n_per_col) + i;
        if (offset < n_uniq) {
          Author = Encoding::LaTeXDecode(uniq.at(offset));
          Author = "<a href=\"" + http.self +
                   "?action=search&match=" + Encoding::URLEncode(Author) +
                   "&scheme=author\">" + Author + "</a>";
        }
      }
      sout << "    <td>\n"
              "      "
           << Author
           << "\n"
              "    </td>\n";
    }
    sout << "  </tr>\n";
  }
  sout << "</table>\n";
}

void DisplayDuplicatesForm(HTTP &http, Preferences &prefs) {

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    return;
  }

  auto duplicates = d.DuplicateRecordsForKey("biblcode");

  sout << "<form name=\"record\" action=\"" << http.self
       << "?action=delete&id=-1\" method=\"post\">\n"
          "<table class=\"layout duplicates\">\n"
          "  <tr>\n"
          "   <td colspan=\"10\">\n"
          "     <h1>Duplicates</h1>"
       << "   </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <td id=\"submenu\" colspan=\"10\">\n"
          "      <button id=\"delete\" title=\"Delete selected "
          "records\" "
          "type=\"button\" onclick=\"if(confirm('Are you sure you "
          "want to "
          "delete the selected "
          "records?'))document.record.submit();\">Delete</button> "
          "<button "
          "id=\"reset\" title=\"Reset\" "
          "type=\"reset\">Reset</button>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <th>\n"
          "      <span title=\"Toggle, hot-key Alt + T, Ctrl + T "
          "(Apple)\"><input type=\"checkbox\" "
          "onclick=\"javascript:Toggle();\" accesskey=\"t\" "
          "/></span> \n"
          "    </th>\n"
          "    <th>\n"
          "      Title/Booktitle\n"
          "    </th>\n"
          "    <th>\n"
          "      Authors\n"
          "    </th>\n"
          "    <th>\n"
          "      Journal\n"
          "    </th>\n"
          "    <th>\n"
          "      Year\n"
          "    </th>\n"
          "    <th>\n"
          "      Keyword(s)\n"
          "    </th>\n"
          "    <th>\n"
          "      Abstract\n"
          "    </th>\n"
          "    <th>\n"
          "      PDF\n"
          "    </th>\n"
          "    <th>\n"
          "      C\n"
          "    </th>\n"
          "    <th>\n"
          "      &nbsp;\n"
          "    </th>\n"
          "  </tr>\n";

  BibTeX::Strings strings(pool);
  prefs_it = prefs["abbreviation"];
  if (prefs_it != prefs_end) {
    strings.Parse(base_path / prefs_it->second);
  }

  for (auto &g : duplicates) {
    for (auto &r : g) {
      DisplayData(http, prefs, r, strings.strings);
    }
    sout << "  <tr>\n"
            "    <td class=\"divider\" colspan=\"10\">&nbsp;</td>\n"
            "  </tr>\n";
  }

  sout << "  <tr id=\"results\">\n"
          "    <td id=\"col1\" colspan=\"10\">\n"
       << duplicates.size() << " duplicat"
       << (duplicates.size() == 1 ? "e" : "es")
       << " found\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr id=\"submenu\">\n"
          "    <td colspan=\"10\">\n"
          "      <button id=\"delete\" title=\"Delete selected "
          "records\" "
          "type=\"button\" onclick=\"if(confirm('Are you sure "
          "you want to "
          "delete the selected "
          "records?'))document.record.submit();\">Delete</"
          "button> <button "
          "id=\"reset\" title=\"Reset\" "
          "type=\"reset\">Reset</button>\n"
          "    </td>\n"
          "  </tr>\n"
          "</table>\n"
          "</form>\n"
          "<script type=\"text/javascript\">\n"
          "  var CheckValue = false;\n"
          "  var FormName   = 'record';\n"
          "  var FieldName  = 'select';\n"
          "\n"
          "  function Toggle(){ \n"
          "    CheckValue = !CheckValue;\n"
          "    if(!document.forms[FormName])\n"
          "      return;\n"
          "    var objCheckBoxes = "
          "document.forms[FormName].elements[FieldName];\n"
          "    if(!objCheckBoxes)\n"
          "      return;\n"
          "    var countCheckBoxes = objCheckBoxes.length;\n"
          "    if(!countCheckBoxes)\n"
          "     objCheckBoxes.checked = CheckValue;\n"
          "    else\n"
          "      for(var i = 0; i < countCheckBoxes; i++)\n"
          "        objCheckBoxes[i].checked = CheckValue;\n"
          "   }\n"
          "</script>\n";
}

void DisplayDOICrossrefForm(HTTP &http, Preferences &prefs) {

  sout << "<table class=\"layout dois\">\n"
          "  <tr>\n"
          "    <td id=\"col1\" colspan=\"2\">\n"
          "      <h1>CrossRef DOIs</h1>\n"
          "    </td>\n"
          "  </tr>\n"
          "  <tr>\n"
          "    <th id=\"id\">\n"
          "      id\n"
          "    </th>\n"
          "    <th id=\"doi\">\n"
          "      doi\n"
          "    </th>\n"
          "  </tr>\n";

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    return;
  }

  float need_score;
  prefs_it = prefs["doicrossrefscore"];
  if (prefs_it != prefs_end) {
    need_score = strtof(prefs_it->second.data(), nullptr);
  }

  std::string_view doilookupurl;
  prefs_it = prefs["doilookupurl"];
  if (prefs_it != prefs_end) {
    doilookupurl = prefs_it->second;
  }

  std::string_view pem;
  prefs_it = prefs["pem"];
  if (prefs_it != prefs_end) {
    pem = prefs_it->second;
  }

  std::string doi;
  for (auto &record_it : d.vRecords) {

    auto record_end = record_it.end();

    auto field_it = record_it["doi"];
    if (field_it != record_end) {
      continue;
    }

    std::string_view id_str;
    field_it = record_it["id"];
    if (field_it != record_end) {
      id_str = field_it->second;
    }

    field_it = record_it["doicrossrefstatus"];
    if (field_it != record_end) {
      sout << "  <tr>\n"
              "    <td>\n"
              "      <a href=\""
           << http.self << "?action=open&id=" << id_str << "\">" << id_str
           << "</a>"
              "\n"
              "    </td>\n"
              "    <td>\n"
           << field_it->second
           << "\n"
              "    </td>\n"
              "  </tr>\n";
      continue;
    }

    std::string_view author;
    field_it = record_it["author"];
    if (field_it != record_end) {
      author = Encoding::LaTeXDecode(field_it->second);
      std::string::size_type pos = author.find(", ");
      if (pos != std::string::npos) {
        author = author.substr(0, pos);
      }
    }
    std::string url(doilookupurl);
    url += "/v1/"
           "works?rows=1&sort=score&order=desc&query.bibliographic=";
    field_it = record_it["title"];
    if (field_it != record_end) {
      url += Encoding::URLEncode(field_it->second);
    }
    url += "&query.author=";
    url = url.append(author) + "&filter=from-pub-date:";
    field_it = record_it["year"];
    if (field_it != record_end) {
      url.append(field_it->second);
    }
    url += ",until-pub-date:";
    if (field_it != record_end) {
      url.append(field_it->second);
    }
    url += "&select=DOI,score&mailto=" CONTACT;

    std::string crossref(http.SecureGet(url, {}, 443, pem));

    sout << "  <tr>\n"
            "    <td>\n"
            "      <a href=\""
         << http.self << "?action=open&id=" << id_str << "\">" << id_str
         << "</a>"
            "\n"
            "    </td>\n"
            "    <td>\n";

    rapidjson::Document json;
    if (json.Parse(crossref.c_str()).HasParseError()) {
      pool.begin();
      pool << "JSON parse error (" << GetParseError_En(json.GetParseError())
           << ')';
      record_it.mFields["doicrossrefstatus"] = pool.sv();
      sout << record_it["doicrossrefstatus"]->second
           << "\n"
              "    </td>\n"
              "  </tr>\n";
      continue;
    }

    if (!json.HasMember("message") || !json["message"].IsObject() ||
        !json["message"].HasMember("total-results") ||
        json["message"]["total-results"].GetInt() == 0) {
      record_it.mFields["doicrossrefstatus"] =
          "no matching crossref found (no results)";
      sout << record_it["doicrossrefstatus"]->second
           << "\n"
              "    </td>\n"
              "  </tr>\n";
      continue;
    }

    if (!json["message"].HasMember("items") ||
        !json["message"]["items"].IsArray() ||
        json["message"]["items"].Size() == 0 ||
        !json["message"]["items"][0].HasMember("DOI") ||
        json["message"]["items"][0]["DOI"].GetStringLength() == 0 ||
        !json["message"]["items"][0].HasMember("score")) {
      record_it.mFields["doicrossrefstatus"] =
          "no matching crossref found (malformed message)";
      sout << record_it["doicrossrefstatus"]->second
           << "\n"
              "    </td>\n"
              "  </tr>\n";
      continue;
    }

    doi = json["message"]["items"][0]["DOI"].GetString();

    float score = json["message"]["items"][0]["score"].GetFloat();
    if (score < need_score) {
      pool.begin();
      pool.setf(std::ios::fixed);
      pool.precision(1);
      pool << "score too low [" << doi << "] (" << score << '<' << need_score
           << ')';
      record_it.mFields["doicrossrefstatus"] = pool.sv();
      sout << record_it["doicrossrefstatus"]->second
           << "\n"
              "    </td>\n"
              "  </tr>\n";
      continue;
    }
    pool.begin();
    pool << '{' << doi << '}';
    record_it.mFields["doi"] = pool.sv();

    sout.setf(std::ios::fixed);
    sout.precision(1);
    sout << doi << " (score: " << score << ")"
         << "\n"
            "    </td>\n"
            "  </tr>\n";
  }
  sout << "</table>\n";

  d.Commit();
}

void Delete(HTTP &http, Preferences &prefs) {

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    return;
  }

  auto helper = [&d, &base_path](std::string_view id_str) {
    if (!d.RemoveRecord(id_str)) {
      return false;
    }

    std::filesystem::path TargetPath(base_path / "archive" / id_str);
    TargetPath.replace_extension("pdf");

    if (std::filesystem::exists(TargetPath)) {
      std::filesystem::remove(TargetPath);
    }
    return (true);
  };

  std::string_view id_str = http.get["id"]->second;
  if (id_str != "-1") {
    if (!helper(id_str)) {
      return;
    }
  } else {
    auto post_it = http.post["select"];
    if (post_it != http.post.end()) {
      std::vector<std::string> vIdentifiers = split_on_comma(post_it->second);

      for (const auto &id_str : vIdentifiers) {
        if (!helper(id_str)) {
          return;
        }
      }
    }
  }

  d.Commit();

  DisplayDataList(http, prefs);
}

void Import(HTTP &http, Preferences &prefs) {

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    return;
  }

  size_t size = 0;

  auto record = BibTeX::Parse(http.file_data, size, pool);
  while (record.mFields.size()) {
    d.SetRecord(record);
    record = BibTeX::Parse(http.file_data, size, pool);
  }

  d.Commit();

  DisplayDataList(http, prefs);
}

void Export(HTTP &http, Preferences &prefs) {

  auto get_it = http.get["action"];
  if (get_it == http.get.end()) {
    return;
  }

  std::string format(get_it->second.substr(6));

  sout << "<table class=\"layout export\">\n"
          "<tr>\n"
          "  <td>\n"
          "    <h1>Export</h1>\n"
          "    <h2>"
       << format
       << "</h2> \n"
          "  </td>\n"
          "  <td id=\"submenu\">\n"
          "    <button id=\"download\" title=\"Download File\" "
          "type=\"button\" onclick=\"gotoURL('";

  for (char &c : format) {
    c = tolower(c);
  }

  auto prefs_end = prefs.end();

  std::filesystem::path base_path;
  auto prefs_it = prefs["base"];
  if (prefs_it != prefs_end) {
    base_path = prefs_it->second;
  }

  std::filesystem::path file;
  prefs_it = prefs[format];
  if (prefs_it != prefs_end) {
    file = prefs_it->second;
  }

  sout << base_path.string() << file.string()
       << "')\">Download File</button>\n"
          "  </td>\n"
          "</tr>\n";

  std::ofstream ofstr(base_path / file);
  if (ofstr.fail()) {
    return;
  }

  prefs_it = prefs["data"];
  if (prefs_it == prefs_end) {
    return;
  }

  Database d(base_path / prefs_it->second, pool);
  if (!d.Good()) {
    ofstr.close();
    return;
  }

  prefs_it = prefs["abbreviation"];
  BibTeX::Strings strings(pool);
  if (prefs_it != prefs_end) {
    strings.Parse(base_path / prefs_it->second);
  }

  int n_export = 0;

  std::string_view id_str = http.get["id"]->second;
  if (id_str != "-1") {
    if (format == "bibtex") {
      std::string_view key;
      auto prefs_it = prefs["key"];
      if (prefs_it != prefs_end) {
        key = prefs_it->second;
      }
      d.ExportRecord(id_str, ofstr, key, BibTeX::Export);
      ++n_export;
    } else if (format == "msword") {
      MSWord::Header(ofstr);
      std::string_view key;
      auto prefs_it = prefs["key"];
      if (prefs_it != prefs_end) {
        key = prefs_it->second;
      }
      MSWord::ExportContext ctx = {strings.strings, key};
      d.ExportRecord(id_str, ofstr, ctx, MSWord::Export);
      MSWord::Footer(ofstr);
      ++n_export;
    } else if (format == "text") {
      d.ExportRecord(id_str, ofstr, strings.strings, Text::Export);
      ++n_export;
    } else {
      return;
    }
  } else {
    if (format == "msword") {
      MSWord::Header(ofstr);
    }
    auto post_it = http.post["select"];
    if (post_it != http.post.end()) {
      std::vector<std::string> vIdentifiers = split_on_comma(post_it->second);
      for (const auto &id_str : vIdentifiers) {
        if (format == "bibtex") {
          std::string_view key;
          auto prefs_it = prefs["key"];
          if (prefs_it != prefs_end) {
            key = prefs_it->second;
          }
          d.ExportRecord(id_str, ofstr, key, BibTeX::Export);
          ++n_export;
        } else if (format == "msword") {
          std::string_view key;
          auto prefs_it = prefs["key"];
          if (prefs_it != prefs_end) {
            key = prefs_it->second;
          }
          MSWord::ExportContext ctx = {strings.strings, key};
          d.ExportRecord(id_str, ofstr, ctx, MSWord::Export);
          ++n_export;
        } else if (format == "text") {
          d.ExportRecord(id_str, ofstr, strings.strings, Text::Export);
          ++n_export;
        } else {
          break;
        }
      }
      if (format == "msword") {
        MSWord::Footer(ofstr);
      }
    }
  }

  ofstr.close();

  sout << "  <tr>\n"
          "    <td>\n"
          "      Exported "
       << n_export << " record(s) to `" << base_path.string() << file.string()
       << "'\n"
          "    </td>\n"
          "    <td id=\"submenu\">\n"
          "      <button id=\"download\" title=\"Download File\" "
          "type=\"button\" onclick=\"gotoURL('"
       << base_path.string() << file.string()
       << "')\">Download File</button>\n"
          "    </td>\n"
          "  </tr>\n"
          "</table>\n";
}
