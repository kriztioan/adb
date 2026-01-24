/**
 *  @file   Actions.cpp
 *  @brief  Actions Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Actions.h"

std::string list_themes(const std::string &directory) {
  std::string files;
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

  std::string_view baseurl(prefs.preferences.mFields.at("baseurl"));

  std::cout
      << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
      << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
         "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-tranitional.dtd\">\n"
      << "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
      << "<head>\n"
      << "  <meta name=\"apple-mobile-web-app-capable\" content=\"yes\" />\n"
      << "  <meta name=\"apple-mobile-web-app-status-bar-style\" "
         "content=\"black-translucent\" />\n"
      << "  <link rel=\"apple-touch-icon\" sizes=\"60x60\" href=\"" << baseurl
      << "images/touch-icon-60x60.png\" />\n"
      << "  <link rel=\"apple-touch-icon\" sizes=\"76x76\" href=\"" << baseurl
      << "images/touch-icon-76x76.png\" />\n"
      << "  <link rel=\"apple-touch-icon\" sizes=\"120x120\" href=\"" << baseurl
      << "images/touch-icon-120x120.png\" /> \n"
      << "  <link rel=\"SHORTCUT ICON\" href=\"" << baseurl
      << "/images/adb.ico\" />\n"
      << "  <link rel=\"stylesheet\" href=\"" << baseurl
      << "css/main.css\" type=\"text/css\" />\n"
      << "  <link rel=\"stylesheet\" href=\"" << baseurl
      << prefs.preferences.mFields.at("plugins")
      << prefs.preferences.mFields.at("scheme")
      << ".css\" type=\"text/css\" />\n"
      << "  <title>The Article dataBase - AdB</title>\n"
      << "  <meta http-equiv=\"Content-Type\" content=\"text/html; "
         "charset=utf-8\" />\n"
      << "  <meta name=\"description\" content=\"Homepage Christiaan "
         "Boersma\" "
         "/>\n"
      << "  <meta name=\"keywords\" content=\"Christiaan, Boersma, AdB, "
         "papers\" />\n"
      << "  <meta name=\"resource-type\" content=\"document\" />\n"
      << "  <meta name=\"robots\" content=\"noimageclick\" />\n"
      << "<meta http-equiv=\"Cache-Control\" content=\"no-cache, no-store, "
         "must-revalidate\" />\n"
      << "<meta http-equiv=\"Pragma\" content=\"no-cache\" />\n"
      << "<meta http-equiv=\"Expires\" content=\"0\" />\n"
      << "  <script type=\"text/javascript\" src=\"" << baseurl
      << "script/interface.js\"></script>\n"
      << "</head>\n"
      << "<body onload=\"setfocus()\">\n";

  return;
}

void DisplayToolsForm(HTTP &http, [[maybe_unused]] Preferences &prefs) {

  std::cout
      << "<table class=\"layout tools\">\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      <h1>Tools</h1>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      <button id=\"duplicates\" title=\"Find duplicates\" "
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
      << http.self << "?action=authors'\">List Authors</button>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      &nbsp;\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "</table>\n";
}

void DisplayData(HTTP &http, Preferences &prefs, Record &record,
                 Record &strings) {

  std::string search;
  auto it = http.get.mFields.find("match"), not_found = http.get.mFields.end();
  if (it != not_found) {
    search = "&amp;match=" + it->second +
             "&amp;scheme=" + http.get.mFields["scheme"];
  }

  std::string_view Type;
  it = record.mFields.find("type");
  if (it != not_found && !it->second.empty()) {
    Type = it->second;
  }

  std::string id_str(record.mFields.at("id"));

  std::string ADScode;
  it = record.mFields.find("ADScode");
  if (it != not_found && !it->second.empty()) {
    ADScode = it->second;
  }

  std::string Editors;
  it = record.mFields.find("editor");
  if (it != not_found && !it->second.empty()) {
    Editors = Coders::LaTeXDecode(it->second);
  }

  std::string Booktitle;
  it = record.mFields.find("booktitle");
  if (it != not_found && !it->second.empty()) {
    Booktitle = Coders::LaTeXDecode(it->second);
  }

  std::string ADSabstract;
  it = record.mFields.find("ADSabstract");
  if (it != not_found && it->second == "on") {
    if (!ADScode.empty()) {
      ADSabstract = "      <button id=\"abstract\" title=\"ADS Abstract\" "
                    "type=\"button\" onclick=\"gotoURL('http://" +
                    prefs.preferences.mFields["adsurl"] + "/abs/" + ADScode +
                    "/abstract')\">Abstract</button> ";
    }
  }

  std::string ADSfullpaper;
  it = record.mFields.find("ADSfullpaper");
  if (it != not_found && it->second == "on") {
    if (!ADScode.empty()) {
      ADSfullpaper = "      <button id=\"pdf\" title=\"ADS PDF\" "
                     "type=\"button\" onclick=\"gotoURL('http://" +
                     prefs.preferences.mFields["adsurl"] + "/link_gateway/" +
                     ADScode + "/PUB_PDF')\">Paper</button>";
    }
  }

  std::string URL;
  it = record.mFields.find("URL");
  if (it != not_found && !it->second.empty()) {
    ADSfullpaper = "  <button id=\"pdf\" title=\"URL\" type=\"button\" "
                   "onclick=\"gotoURL('" +
                   it->second + "')\">URL</button>";
  }

  it = record.mFields.find("archive");
  if (it != not_found && it->second == "on") {
    std::filesystem::path PDFPath("archive/" + id_str + ".pdf");
    std::filesystem::path FullPath(prefs.preferences.mFields.at("base"));
    FullPath += PDFPath;
    if (std::filesystem::exists(FullPath)) {
      ADSfullpaper = "  <button id=\"pdf\" title=\"PDF Archive\" "
                     "type=\"button\" onclick=\"gotoURL('" +
                     prefs.preferences.mFields.at("baseurl") +
                     "script/pdf.js/web/viewer.html?file=%2F" +
                     PDFPath.string() + "')\">Archive</button>";
    }
  }

  std::string Title;
  it = record.mFields.find("title");
  if (it != not_found && !it->second.empty()) {
    Title = Coders::LaTeXDecode(it->second);
  } else if (!Booktitle.empty()) {
    Title = Booktitle;
  } else {
    Title = "No title";
  }

  std::string Comments;
  it = record.mFields.find("comments");
  if (it != not_found && !it->second.empty()) {
    Comments = "      <span title=\"" + it->second +
               "\"><img class=\"comments\" src=\"" +
               prefs.preferences.mFields.at("baseurl") +
               "images/available.gif\" alt=\"Available\" /></span>";
  }

  std::string Keywords;
  it = record.mFields.find("keywords");
  if (it != not_found && !it->second.empty()) {
    Keywords =
        BibTeX::SplitKeywords(Coders::LaTeXDecode(it->second), http.self);
  }

  std::string Authors;
  it = record.mFields.find("author");
  if (it != not_found && !it->second.empty()) {
    Authors = BibTeX::SplitAuthors(
        Coders::LaTeXDecode(it->second),
        strtol(prefs.preferences.mFields["nauthors"].c_str(), nullptr, 10),
        http.self);
  } else if (!Editors.empty()) {
    Authors = BibTeX::SplitAuthors(
        Editors,
        strtol(prefs.preferences.mFields["nauthors"].c_str(), nullptr, 10),
        http.self);
  }

  std::string Journal;
  if (Type == "PHDTHESIS") {
    Journal = "PhD Thesis";
  } else {
    it = record.mFields.find("journal");
    if (it != not_found && !it->second.empty()) {
      Journal = Coders::LaTeXDecode(it->second);
      auto iter = strings.mFields.find(Journal);
      if (iter != strings.mFields.end()) {
        Journal = iter->second;
      }
    } else {
      if (!Booktitle.empty()) {
        Journal = Booktitle;
      } else {
        Journal = "No journal";
      }
    }
  }

  if (prefs.preferences.mFields.contains("table") &&
      prefs.preferences.mFields.at("table") == "true") {
    std::cout << "  <tr>\n"
              << "    <td >\n"
              << "      <input class=\"select\" type=\"checkbox\" "
                 "name=\"select\" value=\""
              << id_str << "\" />\n"
              << "    </td>\n"
              << "    <td>\n"
              << "      <span class=\"title\" title=\"Display entry &apos;"
              << Title << "&apos;\"><a href=\"" << http.self
              << "?action=open&amp;id=" << id_str << search << "\">" << Title
              << "</a></span>\n"
              << "    </td>\n"
              << "    <td>\n"
              << "      <span class=\"authors\">" << Authors << "</span>\n"
              << "    </td>\n"
              << "    <td>\n"
              << "      <span class=\"journal\">" << Journal << "</span>\n"
              << "    </td>\n"
              << "    <td>\n"
              << "      <span class=\"year\">"
              << Coders::LaTeXDecode(record.mFields["year"]) << "</span>\n"
              << "    </td>\n";
  } else {

    std::string Volume;
    it = record.mFields.find("volume");
    if (it != not_found && !it->second.empty()) {
      Volume = Coders::LaTeXDecode(it->second);
      Volume = ", <span class=\"volume\">" + Volume + "</span>";
    }

    std::string Pages;
    it = record.mFields.find("pages");
    if (it != not_found && !it->second.empty()) {
      Pages = Coders::LaTeXDecode(it->second);
      Pages = ", <span class=\"pages\">" + Pages + "</span>";
    }

    std::string DOI;
    it = record.mFields.find("doi");
    if (it != not_found && !it->second.empty()) {
      DOI = Coders::LaTeXDecode(it->second);
      DOI = " <a href=\"http://" + prefs.preferences.mFields["doiurl"] + "/" +
            DOI + "\">doi:" + DOI + "</a>";
    }

    std::cout << "  <tr>\n"
              << "    <td>\n"
              << "      <input class=\"select\" type=\"checkbox\" "
                 "name=\"select\" value=\""
              << id_str << "\" />\n"
              << "    </td>\n"
              << "    <td colspan=\"4\">\n"
              << "      <span class=\"authors\">" << Authors
              << "</span>, <span class=\"title\" title=\"Display entry &apos;"
              << Title << "&apos;\"><a href=\"" << http.self
              << "?action=open&amp;id=" << id_str << search << "\">&quot;"
              << Title << "&quot;</a></span>, <span class=\"year\">"
              << Coders::LaTeXDecode(record.mFields["year"])
              << "</span>, <span class=\"journal\">" << Journal << "</span>"
              << Volume << Pages << DOI << "\n"
              << "    </td>\n";
  }

  std::cout << "    <td>\n"
            << Keywords << "\n"
            << "    </td>\n"
            << "    <td>\n"
            << ADSabstract << "\n"
            << "    </td>\n"
            << "    <td>\n"
            << ADSfullpaper << "\n"
            << "    </td>\n"
            << "    <td>\n"
            << Comments << "\n"
            << "    </td>\n"
            << "    <td>\n"
            << "      <button class=\"modify\" title=\"Modify\" "
               "type=\"button\" onclick=\"gotoURL('"
            << http.self << "?action=edit&amp;id=" << id_str << search
            << "')\">Modify</button>\n"
            << "    </td>\n"
            << "  </tr>\n";
}

void DisplayDataList(HTTP &http, Preferences &prefs) {

  auto it = http.get.mFields.find("reverse"),
       not_found = http.get.mFields.end();

  bool reverse =
      (it != not_found) && (http.get.mFields.at("reverse") == "true");

  std::string::size_type p = http.uri.find("&sortby=");
  if (p != std::string::npos) {
    http.uri = http.uri.substr(0, p);
  }

  std::string srt = reverse ? "" : "&amp;reverse=true";
  std::cout << "<form name=\"entry\" action=\"" << http.self
            << "?action=delete&amp;id=-1\" method=\"post\">\n"
            << "<table class=\"layout list\">\n"
            << "  <colgroup>\n"
            << "    <col id=\"col1\" />\n"
            << "    <col id=\"col2\" />\n"
            << "    <col id=\"col3\" />\n"
            << "    <col id=\"col4\" />\n"
            << "    <col id=\"col5\" />\n"
            << "    <col id=\"col6\" />\n"
            << "    <col id=\"col7\" />\n"
            << "    <col id=\"col8\" />\n"
            << "    <col id=\"col9\" />\n"
            << "    <col id=\"col10\" />\n"
            << "  </colgroup>\n"
            << "  <tr id=\"submenu\">\n"
            << "    <td id=\"col1\" colspan=\"10\">\n"
            << "      <button id=\"delete\" title=\"Delete selected entries\" "
               "type=\"button\" onclick=\"if(confirm('Are you sure you want to "
               "delete the selected "
               "entries?'))document.entry.submit();\">Delete</button> <button "
               "id=\"reset\" title=\"Reset\" type=\"reset\">Reset</button>\n"
            << "      <button id=\"BibTeX\" title=\"Export as BibTeX\" "
               "type=\"button\" onclick=\"document.entry.action='"
            << http.self
            << "?action=exportBibTeX&amp;id=-1';document.entry.submit();\">"
               "Export as BibTeX</button> <button id=\"MSWord\" title=\"Export "
               "as MSWord\" type=\"button\" onclick=\"document.entry.action='"
            << http.self
            << "?action=exportMSWord&amp;id=-1';document.entry.submit();\">"
               "Export as MSWord</button> <button id=\"text\" title=\"Export "
               "as Text\" type=\"button\" onclick=\"document.entry.action='"
            << http.self
            << "?action=exportText&amp;id=-1';document.entry.submit();\">"
               "Export as Text</button>\n"
            << "    </td>" << "\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <th id=\"select\">\n"
            << "      <span title=\"Toggle, hot-key Alt + T, Ctrl + T "
               "(Apple)\"><input id=\"toggle\" type=\"checkbox\" "
               "onclick=\"javascript:Toggle();\" accesskey=\"t\" /></span>\n"
            << "    </th>\n"
            << "    <th id=\"title\">\n"
            << "      <span title=\"Sort by Title/Book\"><a href=\"" << http.uri
            << "&amp;sortby=title" << srt << "\">Title/Booktitle</a></span>\n"
            << "    </th>\n"
            << "    <th id=\"authors\">\n"
            << "      <span title=\"Sort by Authors\"><a href=\"" << http.uri
            << "&amp;sortby=author" << srt << "\">Author(s)</a></span>\n"
            << "    </th>\n"
            << "    <th id=\"journal\">\n"
            << "      <span title=\"Sort by Journal\" ><a href=\"" << http.uri
            << "&amp;sortby=journal" << srt << "\">Journal</a></span>\n"
            << "    </th>\n"
            << "    <th id=\"year\">\n"
            << "      <span title=\"Sort by Year\"><a href=\"" << http.uri
            << "&amp;sortby=year" << srt << "\">Year</a></span>\n"
            << "    </th>\n"
            << "    <th id=\"keywords\">\n"
            << "      Keywords\n"
            << "    </th>\n"
            << "    <th id=\"asbstract\">\n"
            << "      Abstract\n"
            << "    </th>\n"
            << "    <th id=\"pdf\">\n"
            << "     PDF\n"
            << "    </th>\n"
            << "    <th id=\"comments\">\n"
            << "      C\n"
            << "    </th>\n"
            << "    <th id=\"modify\">\n"
            << "      &nbsp;\n"
            << "    </th>\n"
            << "  </tr>\n";

  std::string base(prefs.preferences.mFields.at("base"));
  Database d(base + prefs.preferences.mFields.at("data"));

  it = http.get.mFields.find("sortby");
  if (it != not_found) {
    d.SortRecords(it->second.c_str(), reverse);
  }

  Strings s(base + prefs.preferences.mFields.at("abbreviation"));

  size_t entries = 0;

  std::string scheme;
  it = http.get.mFields.find("scheme");
  if (it != not_found) {
    scheme = it->second;
  }

  std::string search;
  it = http.get.mFields.find("match");
  if (it != not_found) {
    search = it->second;
  }

  std::string ADS;
  if (!scheme.empty()) {

    ADS = "      <span title=\"Search for " + search +
          " on ADS\"><a id=\"ads\" target=\"_blank\" href=\"https://" +
          prefs.preferences.mFields["adsurl"] + "/search/q=" + search +
          "\">Search for " + search + "&apos; on ADS</a></span><br />";

    if (scheme == "any") {
      for (auto &r : d.vRecords) {
        for (const auto &field : r.mFields) {
          std::string v(Coders::LaTeXDecode(field.second));
          if (std::search(v.begin(), v.end(), search.begin(), search.end(),
                          [](const char &c1, const char &c2) {
                            return (toupper(c1) == toupper(c2));
                          }) != v.end()) {
            DisplayData(http, prefs, r, s.strings);
            ++entries;
            break;
          }
        }
      }
    } else if (scheme == "author" && search[0] == '^') {
      search = search.substr(1);
      auto helper = [](std::string_view authors) {
        std::string::size_type p = authors.find(" and ");
        if (p != std::string::npos) {
          return (authors.substr(0, p));
        } else {
          return (authors);
        }
      };
      for (auto &r : d.vRecords) {
        it = r.mFields.find("author");
        if (it == not_found) {
          continue;
        }
        std::string_view v(helper(Coders::LaTeXDecode(it->second)));
        if (std::search(v.begin(), v.end(), search.begin(), search.end(),
                        [](const char &c1, const char &c2) {
                          return (toupper(c1) == toupper(c2));
                        }) != v.end()) {
          DisplayData(http, prefs, r, s.strings);
          ++entries;
        }
      }
    } else {
      for (auto &r : d.vRecords) {
        it = r.mFields.find(scheme);
        if (it == not_found) {
          continue;
        }
        std::string v(Coders::LaTeXDecode(it->second));
        if (std::search(v.begin(), v.end(), search.begin(), search.end(),
                        [](const char &c1, const char &c2) {
                          return (toupper(c1) == toupper(c2));
                        }) != v.end()) {
          DisplayData(http, prefs, r, s.strings);
          ++entries;
        }
      }
    }
  } else {
    for (auto &r : d.vRecords) {
      DisplayData(http, prefs, r, s.strings);
      ++entries;
    }
  }

  std::cout << "  <tr id=\"results\">\n"
            << "    <td id=\"col1\" colspan=\"5\">\n"
            << entries << " entr" << (entries == 1 ? "y" : "ies") << " found\n"
            << "    </td>\n"
            << "    <td id=\"col2\" colspan=\"5\">\n"
            << ADS << "\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr id=\"submenu\">\n"
            << "    <td id=\"col1\" colspan=\"10\">\n"
            << "      <button id=\"delete\" title=\"Delete selected entries\" "
               "type=\"button\" onclick=\"if(confirm('Are you sure you want to "
               "delete the selected "
               "entries?'))document.entry.submit();\">Delete</button> <button "
               "id=\"reset\" title=\"Reset\" type=\"reset\">Reset</button>\n"
            << "      <button id=\"BibTeX\" title=\"Export as BibTeX\" "
               "type=\"button\" onclick=\"document.entry.action='"
            << http.self
            << "?action=exportBibTeX&amp;id=-1';document.entry.submit();\">"
               "Export as BibTeX</button> <button id=\"MSWord\" title=\"Export "
               "as MSWord\" type=\"button\" onclick=\"document.entry.action='"
            << http.self
            << "?action=exportMSWord&amp;id=-1';document.entry.submit();\">"
               "Export as MSWord</button> <button id=\"text\" title=\"Export "
               "as Text\" type=\"button\" onclick=\"document.entry.action='"
            << http.self
            << "?action=exportText&amp;id=-1';document.entry.submit();\">"
               "Export as Text</button>\n"
            << "    </td>" << "\n"
            << "  </tr>\n"
            << "</table>\n"
            << "</form>\n"
            << "<script type=\"text/javascript\">\n"
            << "  var CheckValue = false;\n"
            << "  var FormName   = 'entry';\n"
            << "  var FieldName  = 'select';\n"
            << "\n"
            << "  function Toggle(){ \n"
            << "    CheckValue = !CheckValue;\n"
            << "    if(!document.forms[FormName])\n"
            << "      return;\n"
            << "    var objCheckBoxes = "
               "document.forms[FormName].elements[FieldName];\n"
            << "    if(!objCheckBoxes)\n"
            << "      return;\n"
            << "    var countCheckBoxes = objCheckBoxes.length;\n"
            << "    if(!countCheckBoxes)\n"
            << "     objCheckBoxes.checked = CheckValue;\n"
            << "    else\n"
            << "      for(var i = 0; i < countCheckBoxes; i++)\n"
            << "        objCheckBoxes[i].checked = CheckValue;\n"
            << "   }\n"
            << "</script>\n";
}

void WriteHTMLFooter() {
  std::cout << "</body>\n"
            << "</html>\n";
}

void DisplayMenu(HTTP &http, Preferences &prefs) {

  const std::string_view options("any&author&keywords&comments&title");

  std::string newVersion;
  if (prefs.preferences.mFields["updates"] == "true") {
    std::string latest = HTTP::Get(UPDATE_URL);
    if (latest != CURR_VERSION) {
      newVersion = " <button title=\"Update AdB to " + latest +
                   "\" type=\"button\" onclick=\"openWindow('" + http.self +
                   "?action=versionupdate', 500, 700)\">Update</button>";
    }
  }

  std::cout
      << "<form name=\"search\" action=\"" << http.self
      << "?action=search\" method=\"get\">\n"
      << "<table class=\"menu\">\n"
      << "  <tr>\n"
      << "    <td id=\"col1\">\n"
      << "     <button id=\"new\" title=\"New\" type=\"button\" "
         "onclick=\"gotoURL('"
      << http.self
      << "?action=edit&amp;id=-1')\">New</button> <button id=\"browse\" "
         "title=\"Browse "
         "all entries\" type=\"button\" onclick=\"gotoURL('"
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
      << http.self << "?action=info')\">Info</button>" /* <button id= \"logout\"
                   " "title=\"Logout\" type=\"button\" onclick=\"gotoURL('"
                   << http.self << "?action=logout')\">Logout</button>" <<
                   newVersion << "\n" */
      << "    </td>\n"
      << "    <td id=\"col2\">\n"
      << "      <input type=\"hidden\" name=\"action\" value=\"search\"/>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" "
         "class=\"search\" type=\"text\" name=\"match\" "
         "value=\""
      << Coders::HTMLEncode(http.get.mFields["match"])
      << "\" accesskey=\"s\"/> as \n"
      << HTML::Select(options, http.get.mFields["scheme"], "scheme", "") << "\n"
      << "<button id=\"search\" title=\"Search, hot-key Alt + S, Ctrl + S "
         "(Apple)\" type=\"submit\">Search</button>\n"
      << "   </td>\n"
      << "  </tr>\n"
      << "</table>\n"
      << "</form>\n";
}

void DisplayFooter(const std::chrono::steady_clock::time_point &t0) {

  std::chrono::system_clock::time_point clock =
      std::chrono::system_clock::now();

  time_t t = std::chrono::system_clock::to_time_t(clock);

  struct tm *timeinfo = localtime(&t);

  char year[5];

  strftime(year, 5, "%Y", timeinfo);

  std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

  std::cout.setf(std::ios::fixed);
  std::cout.precision(1);

  std::cout << "<table class=\"footer\">\n"
            << "  <tr>\n"
            << "    <td id=\"version\">\n"
            << "      " << CURR_VERSION << "\n"
            << "    </td>\n"
            << "    <td id=\"timer\">\n"
            << "      page built in "
            << std::chrono::duration<float, std::milli>{t1 - t0}.count()
            << " ms\n"
            << "    </td>\n"
            << "    <td id=\"copyright\">\n"
            << "&#169; Christiaan Boersma 2004/" << year << "\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "</table>\n"
            << "\n";
}

void DisplayEntry(HTTP &http, Preferences &prefs) {

  std::string search;
  auto it = http.get.mFields.find("match");

  if (it != http.get.mFields.end()) {
    search = "&amp;match=" + it->second +
             "&amp;scheme=" + http.get.mFields["scheme"];
  }

  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);
  if (!d.good()) {
    return;
  }

  std::string id_str(http.get.mFields.at("id")), id, prev_id, next_id;
  auto record = d.vRecords.begin();
  for (; record < d.vRecords.end(); record++) {
    if ((id = record->mFields.at("id")) == id_str) {
      auto r = record + 1;
      if (r != d.vRecords.end()) {
        next_id = r->mFields.at("id");
      }
      break;
    }
    prev_id = id;
  }

  if (record == d.vRecords.end()) {
    return;
  }

  auto not_found = record->mFields.end();

  std::string_view Type;
  it = record->mFields.find("type");
  if (it != not_found && !it->second.empty()) {
    Type = it->second;
  }

  std::string ADScode;
  it = record->mFields.find("ADScode");
  if (it != not_found && !it->second.empty()) {
    ADScode = it->second;
  }

  std::string Editors;
  it = record->mFields.find("editor");
  if (it != not_found && !it->second.empty()) {
    Editors = Coders::LaTeXDecode(it->second);
  }

  std::string Booktitle;
  it = record->mFields.find("booktitle");
  if (it != not_found && !it->second.empty()) {
    Booktitle = Coders::LaTeXDecode(it->second);
  }

  std::string ADSabstract;
  it = record->mFields.find("ADSabstract");
  if (it != not_found && it->second == "on") {
    if (!ADScode.empty()) {
      ADSabstract = "      <button id=\"abstract\" title=\"ADS Abstract\" "
                    "type=\"button\" onclick=\"gotoURL('http://" +
                    prefs.preferences.mFields["adsurl"] + "/abs/" + ADScode +
                    "/abstract')\">Abstract</button> ";
    }
  }

  std::string ADSfullpaper;
  it = record->mFields.find("ADSfullpaper");
  if (it != not_found && it->second == "on") {
    if (!ADScode.empty()) {
      ADSfullpaper = "      <button id=\"pdf\" title=\"ADS PDF\" "
                     "type=\"button\" onclick=\"gotoURL('http://" +
                     prefs.preferences.mFields["adsurl"] + "/link_gateway/" +
                     ADScode + "/PUB_PDF')\">Paper</button>";
    }
  }

  std::string URL;
  it = record->mFields.find("URL");
  if (it != not_found && !it->second.empty()) {
    ADSfullpaper = "  <button id=\"pdf\" title=\"URL\" type=\"button\" "
                   "onclick=\"gotoURL('" +
                   it->second + "')\">URL</button>";
  }

  it = record->mFields.find("archive");
  if (it != not_found && it->second == "on") {
    std::filesystem::path PDFPath("archive/" + id_str + ".pdf");
    std::filesystem::path FullPath(prefs.preferences.mFields.at("base"));
    FullPath += PDFPath;
    if (std::filesystem::exists(FullPath)) {
      ADSfullpaper = "  <button id=\"pdf\" title=\"PDF Archive\" "
                     "type=\"button\" onclick=\"gotoURL('" +
                     prefs.preferences.mFields.at("baseurl") +
                     "script/pdf.js/web/viewer.html?file=%2F" +
                     PDFPath.string() + "')\">Archive</button>";
    }
  }

  std::string Title;
  it = record->mFields.find("title");
  if (it != not_found && !it->second.empty()) {
    Title = Coders::LaTeXDecode(it->second);
  } else if (!Booktitle.empty()) {
    Title = Booktitle;
  } else {
    Title = "No title";
  }

  std::string Comments;
  it = record->mFields.find("comments");
  if (it != not_found && !it->second.empty()) {
    Comments = "      <span title=\"" + it->second +
               "\"><img class=\"comments\" src=\"" +
               prefs.preferences.mFields.at("baseurl") +
               "images/available.gif\" alt=\"Available\" /></span>";
  }

  std::string Keywords;
  it = record->mFields.find("keywords");
  if (it != not_found && !it->second.empty()) {
    Keywords =
        BibTeX::SplitKeywords(Coders::LaTeXDecode(it->second), http.self);
  }

  std::string Authors;
  it = record->mFields.find("author");
  if (it != not_found && !it->second.empty()) {
    Authors = BibTeX::SplitAuthors(
        Coders::LaTeXDecode(it->second),
        strtol(prefs.preferences.mFields["nauthors"].c_str(), nullptr, 10),
        http.self);
  } else if (!Editors.empty()) {
    Authors = BibTeX::SplitAuthors(
        Editors,
        strtol(prefs.preferences.mFields["nauthors"].c_str(), nullptr, 10),
        http.self);
  }

  std::string Journal;
  if (Type == "PHDTHESIS") {
    Journal = "PhD Thesis";
  } else {
    it = record->mFields.find("journal");
    if (it != not_found && !it->second.empty()) {
      Journal = Coders::LaTeXDecode(it->second);
      Strings s(prefs.preferences.mFields["base"] +
                prefs.preferences.mFields["abbreviation"]);
      auto iter = s.strings.mFields.find(Journal);
      if (iter != s.strings.mFields.end()) {
        Journal = iter->second;
      }
    } else {
      if (!Booktitle.empty()) {
        Journal = Booktitle;
      } else {
        Journal = "No journal";
      }
    }
  }

  std::string Hardcopy;
  it = record->mFields.find("dossier");
  if (it != not_found && !it->second.empty()) {
    Hardcopy = " <b> Dossier #</b> " + Coders::LaTeXDecode(it->second);
  }

  std::string Year;
  it = record->mFields.find("year");
  if (it != not_found && !it->second.empty()) {
    Year = Coders::LaTeXDecode(it->second);
  }

  std::string Month;
  it = record->mFields.find("month");
  if (it != not_found && !it->second.empty()) {
    Month = Coders::LaTeXDecode(it->second);
  }

  std::string Publisher;
  it = record->mFields.find("publisher");
  if (it != not_found && !it->second.empty()) {
    Publisher = Coders::LaTeXDecode(it->second);
  }

  std::string Biblcode;
  it = record->mFields.find("biblcode");
  if (it != not_found && !it->second.empty()) {
    if (!ADScode.empty() && ADScode != Biblcode) {
      ADScode = "       <b>ADS Code</b>: " + ADScode + " ";
    }
    Biblcode = "       <b>Bibl. Code</b>: " + it->second + " ";
  }

  std::string Vol;
  it = record->mFields.find("volume");
  if (it != not_found && !it->second.empty()) {
    Vol = Coders::LaTeXDecode(it->second);
  }

  std::string Pages;
  it = record->mFields.find("pages");
  if (it != not_found && !it->second.empty()) {
    Pages = Coders::LaTeXDecode(it->second);
  }

  std::string DOI;
  it = record->mFields.find("doi");
  if (it != not_found && !it->second.empty()) {
    DOI = Coders::LaTeXDecode(it->second);
    DOI = " <a href=\"http://" + prefs.preferences.mFields["doiurl"] + "/" +
          DOI + "\">doi:" + DOI + "</a>";
  }

  if (!search.empty()) {
    Keywords = HTML::Highlight(Keywords, search);
    Authors = HTML::Highlight(Authors, search);
    Comments = HTML::Highlight(Comments, search);
    Title = HTML::Highlight(Title, search);
    Booktitle = HTML::Highlight(Booktitle, search);
  }

  if (!prev_id.empty()) {
    prev_id =
        "       <span title=\"Previous\"><a href=\"" + http.self +
        "?action=open&amp;id=" + prev_id +
        "\" onmouseover=\"window.status='Previous';return true\" "
        "onmouseout=\"window.status=' '\"><img src=\"" +
        prefs.preferences.mFields["baseurl"] +
        "images/previous.gif\" alt=\"Previous\" border=\"0\" /></a></span>";
  } else {
    prev_id = "       <img src=\"" + prefs.preferences.mFields["baseurl"] +
              "images/previous_disabled.gif\" alt=\"disabled\" />";
  }

  if (!next_id.empty()) {
    next_id = "       <span title=\"Next\"><a href=\"" + http.self +
              "?action=open&amp;id=" + next_id +
              "\" onmouseover=\"window.status='Next';return true\" "
              "onmouseout=\"window.status=' '\"><img src=\"" +
              prefs.preferences.mFields["baseurl"] +
              "images/next.gif\" alt=\"Next\" border=\"0\" /></a></span>";
  } else {
    next_id = "       <img src=\"" + prefs.preferences.mFields["baseurl"] +
              "images/next_disabled.gif\" alt=\"disabled\" />";
  }

  std::cout << "<table class=\"layout entry\">\n"
            << "  <tr>\n"
            << "    <td id=\"control\">\n"
            << prev_id << next_id << " " << id_str << "\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <td>\n"
            << "      <span id=\"journal\">" << Journal
            << "</span> <span id=\"volume\">" << Vol
            << "</span> <span id=\"month\">" << Month
            << "</span> <span id=\"year\">" << Year
            << "</span> <span id=\"pages\">" << Pages
            << "</span> <span id=\"publisher\">" << Publisher
            << "</span> <span id=\"doi\">" << DOI << "</span>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <td>\n"
            << "      <h1>" << (Booktitle.empty() ? "&nbsp;" : Booktitle)
            << "</h1>\n"
            << "      <h2>" << (Title.empty() ? "&nbsp;" : Title) << "</h2>\n"
            << "      <span id=\"authors\">" << Authors << "</span>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <td>\n"
            << "      <span id=\"keywords\">" << Keywords
            << "</span> <span id=\"Biblcode\">" << Biblcode
            << "</span> <span id=\"ADSCode\">" << ADScode
            << "</span> <span id=\"hardcopy\">" << Hardcopy << "</span>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <td>\n"
            << "       <span id=\"adsabstract\">" << ADSabstract
            << "</span> <span id=\"pdf\">" << ADSfullpaper
            << "</span> <span id=\"url\">" << URL << "</span>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <td>\n"
            << "      <p id=\"comments\">\n"
            << "        " << Comments << "\n"
            << "      </p>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <td id=\"submenu\">\n"
            << "      <button id=\"bibtex\" title=\"Export BibTeX\" "
               "type=\"button\" onclick=\"gotoURL('"
            << http.self << "?action=exportBibTeX&amp;id=" << id_str << search
            << "')\">Export as BibTeX</button> <button id=\"msword\" "
               "title=\"Export MSWord\" type=\"button\" onclick=\"gotoURL('"
            << http.self << "?action=exportMSWord&amp;id=" << id_str << search
            << "')\">Export as MSWord</button> <button id=\"text\" "
               "title=\"Export Text\" type=\"button\" onclick=\"gotoURL('"
            << http.self << "?action=exportText&amp;id=" << id_str << search
            << "')\">Export as Text</button> &mdash; <button id=\"modify\" "
               "title=\"Modify\" type=\"button\" onclick=\"gotoURL('"
            << http.self << "?action=edit&amp;id=" << id_str << search
            << "')\">Modify</button> <button id=\"delete\" title=\"Delete this "
               "entry\" type=\"button\" onclick=\"if(confirm('Are you sure you "
               "want to delete this entry?'))document.location='"
            << http.self << "?action=delete&amp;id=" << id_str << search
            << "';\">Delete</button>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "</table>\n";
}

std::vector<std::string> split_on_comma(std::string str) {

  std::vector<std::string> v;

  size_t b = 0, e = 0;
  const char *p = str.c_str(), *q = p;
  while (*p) {
    if (*p == ',') {
      v.emplace_back(Coders::LaTeXDecode(std::string(q + b, e - b)));
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
    v.emplace_back(Coders::LaTeXDecode(std::string(q + b, e - b)));
  }

  return (v);
};

std::vector<std::string> split_on_and(std::string str) {

  std::vector<std::string> vStr;

  const char *b = str.c_str(), *e;

  int len, e_len, b_len = strlen(b);
  while ((e = strstr(b, " and "))) {
    e_len = strlen(e);
    len = b_len - e_len;
    vStr.emplace_back(Coders::LaTeXDecode(std::string(b, len)));
    b = e + 5;
    b_len -= (len + 5);
  }
  b_len = strlen(b);
  if (b_len) {
    vStr.emplace_back(Coders::LaTeXDecode(std::string(b, b_len)));
  }

  return (vStr);
}

void DisplayEntryForm(HTTP &http, Preferences &prefs) {

  std::string query_ads, query_doi, id_str(http.get.mFields.at("id")),
      search(http.get.mFields["match"]), scheme(http.get.mFields["scheme"]);

  if (!search.empty()) {
    search = "&amp;match=" + search + "&amp;scheme=" + scheme;
  }

  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);
  if (!d.good()) {
    return;
  }

  std::unordered_map<std::string, std::string> mFields;
  if (id_str != "-1") {
    for (auto &r : d.vRecords) {
      if (r.mFields.at("id") == id_str) {
        mFields = r.mFields;
        break;
      }
    }
  } else {
    mFields = http.post.mFields;
    query_ads = " <button id=\"adsquery\" title=\"Query\" type=\"button\" "
                "onclick=\"document.entry.action='" +
                http.self +
                "?action=queryADS&amp;id=-1';document.entry.submit();\">Query "
                "ADS</button> ";
    query_doi = " <button id=\"doiquery\" title=\"Query\" type=\"button\" "
                "onclick=\"document.entry.action='" +
                http.self +
                "?action=queryDOI&amp;id=-1';document.entry.submit();\">Query "
                "DOI</button> ";
  }

  std::string type(mFields["type"]),
      options(
          "ARTICLE&INPROCEEDINGS&PROCEEDINGS&BOOK&BOOKLET&INBOOK&INCOLLECTION&"
          "MASTERTHESIS&PHDTHESIS&TECHREPORT&MISC&MANUAL&UNPUBLISHED&SOFTWARE");

  auto uniq = d.UniqueValuesForKey("keywords", split_on_comma);

  std::string JSArray(Javascript::Array(uniq));

  std::cout
      << "<script type=\"text/javascript\">\n"
      << "\n"
      << "  var choices = " << JSArray << "\n"
      << "\n"
      << "</script>\n"
      << "\n"
      << "<form name=\"entry\" action=\"" << http.self
      << "?action=update&amp;id=" << id_str << search
      << "\" method=\"post\" enctype=\"multipart/form-data\">\n"
      << "<input name=\"id\" type=\"hidden\" value=\"" << id_str << "\" />\n"
      << "<table class=\"layout edit\">\n"
      << "  <tr>\n"
      << "    <td id=\"submenu\" colspan=\"2\">\n"
      << "      <button id=\"save\" title=\"Save\" "
         "type=\"submit\">Save</button> <button id=\"reset\" "
         "title=\"Reset\" "
         "type=\"reset\">Reset</button> <button id=\"delete\" "
         "title=\"Delete "
         "this entry\" type=\"button\" "
         "onclick=\"javascript:if(confirm('Are "
         "you sure you want to delete this entry?'))document.location='"
      << http.self << "?action=delete&amp;id=" << id_str
      << "';\">Delete</button>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h1>Modify</h1>\n"
      << "      <h2>AdB</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       id\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      " << id_str << (id_str == "-1" ? "<i>(new)</i>" : "") << "\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "      <td>\n"
      << "       Bibl. Code\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"biblcode\" class=\"config\" "
         "name=\"biblcode\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["biblcode"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       ADS Code\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"ADScode\" class=\"config\" "
         "name=\"ADScode\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["ADScode"]) << "\" />" << query_ads << "\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       DOI Code\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"doi\" class=\"config\" name=\"doi\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["doi"]) << "\" />" << query_doi << "\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Keyword(s)\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"keywords\" class=\"config\" "
         "name=\"keywords\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["keywords"])
      << "\" onkeydown=\"return checkchar(event);\" "
         "onkeyup=\"checkinput(this);\" onblur=\"hide();\" "
         "autocomplete=\"off\" />\n"
      << "      <div class=\"autocomplete\" id=\"autocomplete\" "
         "onmouseover=\"keepfocus=true;\" onmouseout=\"keepfocus=false\">\n"
      << "      </div>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>BiBTeX</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Type\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      " << HTML::Select(options, type, "type", "evalType(this)")
      << "\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Title\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"title\" class=\"config\" "
         "name=\"title\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["title"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Book title\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"booktitle\" class=\"config\" "
         "name=\"booktitle\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["booktitle"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Editor\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"editor\" class=\"config\" "
         "name=\"editor\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["editor"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Organization\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"organization\" class=\"config\" "
         "name=\"organization\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["organization"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Address\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"address\" class=\"config\" "
         "name=\"address\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["address"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Series\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"series\" class=\"config\" "
         "name=\"series\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["series"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Edition\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"edition\" class=\"config\" "
         "name=\"edition\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["edition"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       How published\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"howpublished\" class=\"config\" "
         "name=\"howpublished\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["howpublished"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Chapter\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"chapter\" class=\"config\" "
         "name=\"chapter\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["chapter"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       School\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"school\" class=\"config\" "
         "name=\"school\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["school"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Institution\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"institution\" class=\"config\" "
         "name=\"institution\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["institution"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Author(s)\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <textarea autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"authors\" class=\"config\" "
         "name=\"author\">"
      << Coders::HTMLEncode(mFields["author"]) << "</textarea>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Journal\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"journal\" class=\"config\" "
         "name=\"journal\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["journal"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Volume\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"volume\" class=\"config\" "
         "name=\"volume\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["volume"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Number\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"number\" class=\"config\" "
         "name=\"number\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["number"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Pages\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"pages\" class=\"config\" "
         "name=\"pages\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["pages"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Publisher\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"publisher\" class=\"config\" "
         "name=\"publisher\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["publisher"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Month\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"month\" class=\"config\" "
         "name=\"month\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["month"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Year\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"year\" class=\"config\" name=\"year\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["year"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Summary\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"summary\" class=\"config\" "
         "name=\"summary\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["summary"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Note\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"note\" class=\"config\" name=\"note\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["note"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>ADS</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       ADS Services\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input name=\"ADSabstract\" type=\"checkbox\""
      << (mFields["ADSabstract"] == "on" ? " checked=\"checked\"" : "")
      << " /> Abstract (<i>link</i>) <input name=\"ADSfullpaper\" "
         "type=\"checkbox\""
      << (mFields["ADSfullpaper"] == "on" ? " checked=\"checked\"" : "")
      << " /> PDF (<i>link</i>)\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>PDF</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       URL\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"url\" class=\"config\" name=\"URL\" "
         "type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["URL"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Archive\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input name=\"archive\" type=\"checkbox\""
      << (mFields["archive"] == "on" ? " checked=\"checked\"" : "")
      << " /> Archive"
      << (mFields["archive"] == "on"
              ? "d <i>" + HTML::Filesize(prefs.preferences.mFields["base"] +
                                         "archive/" + id_str + ".pdf")
              : " (<i>downloads from ADS, or, when specified uses Upload "
                "file</i>)\n")
      << "</i>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Upload\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"pdf\" class=\"config\" name=\"pdf\" "
         "type=\"file\" accept=\"application/pdf\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>Hardcopy</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Dossier\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" id=\"dossier\" class=\"config\" "
         "name=\"dossier\" type=\"text\" value=\""
      << Coders::HTMLEncode(mFields["dossier"]) << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>Extra</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "       Comments\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <textarea id=\"comments\" class=\"config\" rows=\"5\" "
         "name=\"comments\">"
      << Coders::HTMLEncode(mFields["comments"]) << "</textarea>\n"
      << "    </td>\n"
      << "  </tr>\n";

  static constexpr const char *usedKeys[] = {
      "id",           "biblcode",    "ADScode",      "doi",          "keywords",
      "type",         "author",      "title",        "booktitle",    "editor",
      "organization", "address",     "edition",      "howpublished", "chapter",
      "school",       "institution", "author",       "journal",      "volume",
      "number",       "pages",       "publisher",    "month",        "year",
      "summary",      "note",        "ADSfullpaper", "ADSabstract",  "URL",
      "dossier",      "comments",    "archive",      "series"};

  for (const auto *s : usedKeys) {
    auto f = mFields.find(s);
    if (f != mFields.end()) {
      mFields.erase(f);
    }
  }

  for (const auto &field : mFields) {

    std::cout << "  <tr>\n"
              << "    <td>\n"
              << field.first << "\n"
              << "    </td>\n"
              << "    <td>\n"
              << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
                 "autocomplete=\"off\" id=\""
              << field.first << "\" class=\"config\" name=\"" << field.first
              << "\" type=\"text\" value=\"" << Coders::HTMLEncode(field.second)
              << "\" /> <button id=\"del\" title=\"Del\" type=\"button\" "
                 "onclick=\"delExtraKeyValuePair(this);\">Del</button>\n"
              << "    </td>\n"
              << "  </tr>\n";
  }

  std::cout
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <button id=\"add\" title=\"Add\" type=\"button\" "
         "onclick=\"addExtraKeyValuePair();\">Add</button>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td id=\"submenu\" colspan=\"2\">\n"
      << "      <button id=\"save\" title=\"Save\" "
         "type=\"submit\">Save</button> <button id=\"reset\" title=\"Reset\" "
         "type=\"reset\">Reset</button> <button id=\"delete\" title=\"Delete "
         "this entry\" type=\"button\" onclick=\"javascript:if(confirm('Are "
         "you sure you want to delete this entry?'))document.location='"
      << http.self << "?action=delete&amp;id=" << id_str
      << "';\">Delete</button>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "</table>\n"
      << "</form>\n\n"
      << "\n"
      << "<script type=\"text/javascript\">\n"
      << "\n"
      << "  evalType(document.forms.entry.type);\n"
      << "\n"
      << "  function addExtraKeyValuePair(){\n"
      << "    var key    = prompt('Please provide a key', 'your key');\n"
      << "    if(key != '' && key != null){\n"
      << "      var tables = document.getElementsByTagName('table');\n"
      << "      var table  = tables[tables.length - 2];\n"
      << "      var row    = "
         "table.tBodies[0].insertBefore(document.createElement('tr'), "
         "table.tBodies[0].rows[table.tBodies[0].rows.length - 2]);\n"
      << "      var td     = row.appendChild(document.createElement('td'));\n"
      << "      var text   = td.appendChild(document.createTextNode(key));\n"
      << "      var td     = row.appendChild(document.createElement('td'));\n"
      << "      td.innerHTML = '<input autocorrect=\"off\" "
         "autocapitalize=\"off\" autocomplete=\"off\" class=\"config\" "
         "name=\"' + key + '\" type=\"text\" value=\"\"></input> <button "
         "title=\"Del\" type=\"button\" "
         "onclick=\"delExtraKeyValuePair(this);\">Del</button>';\n"
      << "      return(true);\n"
      << "  }\n"
      << "}\n"
      << "\n"
      << "function delExtraKeyValuePair(element){\n"
      << "  "
         "element.parentNode.parentNode.parentNode.removeChild(element."
         "parentNode.parentNode);\n"
      << "}\n"
      << "\n"
      << "</script>\n";
}

void Update(HTTP &http, Preferences &prefs) {

  if (http.post.mFields.empty()) {
    return;
  }

  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);
  if (!d.good()) {
    return;
  }

  d.SetRecord(http.post, http.get.mFields.at("id"));
  d.Commit();
  http.get.mFields.at("id") = http.post.mFields.at("id");

  std::filesystem::path TargetPath(prefs.preferences.mFields["base"] +
                                   "archive/" + http.post.mFields.at("id") +
                                   ".pdf");

  if (http.post.mFields["archive"] == "on") {

    if (http.file_data.empty() && !std::filesystem::exists(TargetPath)) {

      std::string cmd(
          prefs.preferences.mFields["base"] + "csh/AdBArchiver.csh " +
          prefs.preferences.mFields["base"] + " " + "'https://" +
          prefs.preferences.mFields["adsurl"] + "/pdf/" +
          http.post.mFields["ADScode"] + "' " + TargetPath.string());

      if (system(cmd.c_str()) == -1) {
        return;
      }

      if (std::filesystem::exists(TargetPath) &&
          std::filesystem::file_size(TargetPath) == 0) {
        if (!std::filesystem::remove(TargetPath)) {
          return;
        }
      }
    } else if (!http.file_data.empty()) {
      std::ofstream ofstr(TargetPath.c_str(), std::ios::binary);

      ofstr.write(http.file_data.c_str(), http.file_data.length());

      ofstr.close();
    }
  } else if (std::filesystem::exists(TargetPath)) {

    if (!std::filesystem::remove(TargetPath)) {
      return;
    }
  }

  DisplayEntry(http, prefs);
}

void QueryADS(HTTP &http, Preferences &prefs) {

  std::string post = "{\"bibcode\":[\"" + http.post.mFields["ADScode"] + "\"]}",
              authorization = "Authorization: Bearer " +
                              prefs.preferences.mFields["adstoken"],
              content_type = "Content-Type: application/json";

  std::vector<std::string> headers = {authorization, content_type};

  std::string bibtex =
      http.SecurePost(ADS_API_URL, post, headers, 443,
                      prefs.preferences.mFields["pem"].c_str());

  rapidjson::Document json;
  if (json.Parse(bibtex.c_str()).HasParseError() || !json.HasMember("export")) {
    http.post.mFields["ADScode"] += " (not found)";
    DisplayEntryForm(http, prefs);
    return;
  }

  size_t size = 0;

  http.post = BibTeX::Parse(json["export"].GetString(), size);

  http.post.mFields["ADScode"] = http.post.mFields["biblcode"];

  DisplayEntryForm(http, prefs);
}

void QueryDOI(HTTP &http, Preferences &prefs) {

  std::string doi(http.post.mFields["doi"]),
      url(prefs.preferences.mFields["doilookupurl"] + "/v1/works/" +
          Coders::URLEncode(doi) + "/transform");

  std::vector<std::string> headers = {
      "Accept: text/bibliography; style=bibtex"};

  std::string bibtex(http.SecureGet(url, headers, 443,
                                    prefs.preferences.mFields["pem"].c_str()));

  size_t size = 0;
  http.post = BibTeX::Parse(bibtex, size);

  auto it = http.post.mFields.find("doi");
  if (it == http.post.mFields.end()) {
    http.post.mFields["doi"] = doi;
  }

  DisplayEntryForm(http, prefs);
}

void DisplayInfo([[maybe_unused]] HTTP &http, Preferences &prefs) {

  time_t clock = time(nullptr);

  struct tm *timeinfo = localtime(&clock);

  char year[5];

  strftime(year, 5, "%Y", timeinfo);

  std::cout << "<table class=\"layout info\">\n"
            << "  <tr>\n"
            << "    <td id=\"col1\">\n"
            << "      <h1>Info</h1>\n"
            << "      <h2>General</h2>\n"
            << "    </td>\n"
            << "    <td id=\"col2\" rowspan=\"2\">\n"
            << "      <img src=\"" << prefs.preferences.mFields["baseurl"]
            << "images/logo.png\" alt=\"logo\" />\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <td colspan=\"2\">\n"
            << "      <p id=\"disclaimer\">\n"
            << "        AdB is an article database writen in C/C++ and "
               "developed by "
               "Dr. C. Boersma. This is "
            << CURR_VERSION
            << ". The latest version can be downloaded <a href=\"" +
                   std::string(LATEST_URL) +
                   "\">here</a>. The developer can be contacted at <a "
                   "href=\"mailto:"
            << CONTACT << "\">" << CONTACT << "</a>\n"
            << "      </p>\n"
            << "      <p id=\"copyright\">\n"
            << "        Christiaan Boersma 2004 - " << year << "\n"
            << "      </p>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "</table>\n";
}

void DisplaySplash([[maybe_unused]] HTTP &http, Preferences &prefs) {

  if (prefs.preferences.mFields["splash"] == "true") {
    std::cout << "<table class=\"layout splash\">\n"
              << "  <tr>\n"
              << "    <td id=\"image\">\n"
              << "    </td>\n"
              << "  </tr>\n"
              << "</table>\n";

  } else {
    DisplayDataList(http, prefs);
  }
}

void DisplayConfigForm(HTTP &http, Preferences &prefs) {

  std::string splash, translate, table, updates;

  if (prefs.preferences.mFields["splash"] == "true") {
    splash = " checked=\"checked\"";
  }

  if (prefs.preferences.mFields["translate"] == "true") {
    translate = " checked=\"checked\"";
  }

  if (prefs.preferences.mFields["table"] == "true") {
    table = " checked=\"checked\"";
  }

  if (prefs.preferences.mFields["updates"] == "true") {
    updates = " checked=\"checked\"";
  }

  std::cout
      << "<form name=\"config\" action=\"" << http.self
      << "?action=config\" method=\"post\">\n"
      << "<input type=\"hidden\" name=\"save\" value=\"true\" />\n"
      << "<table class=\"layout config\">\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h1>Configuration</h1>\n"
      << "      <h2>General</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td width=\"20%\">\n"
      << "      Base directory \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" name=\"base\" "
         "value=\""
      << prefs.preferences.mFields["base"] << "\" /> \n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td width=\"20%\">\n"
      << "      Database file \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" name=\"data\" "
         "value=\""
      << prefs.preferences.mFields["data"] << "\" /> "
      << HTML::Filesize(prefs.preferences.mFields["base"] +
                        prefs.preferences.mFields["data"])
      << "\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td width=\"20%\">\n"
      << "      Abbreviation file \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"abbreviation\" value=\""
      << prefs.preferences.mFields["abbreviation"] << "\" /> "
      << HTML::Filesize(prefs.preferences.mFields["base"] +
                        prefs.preferences.mFields["abbreviation"])
      << "\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Themes directory \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"plugins\" "
         "value=\""
      << prefs.preferences.mFields["plugins"] << "\" /> \n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Base URL \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"baseurl\" "
         "value=\""
      << prefs.preferences.mFields["baseurl"] << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Check for updates\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input name=\"updates\" type=\"checkbox\" value=\"true\""
      << updates << " />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Administrator\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"administrator\" value=\""
      << prefs.preferences.mFields["administrator"] << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>ADS</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Url\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"adsurl\" "
         "value=\""
      << prefs.preferences.mFields["adsurl"] << "\" /> <i>no leading http</i>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Token\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"adstoken\" value=\""
      << prefs.preferences.mFields["adstoken"]
      << "\" /> <a "
         "href=\"https://ui.adsabs.harvard.edu/user/account/login\"><i>get "
         "your token</i></a>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>DOI</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Url for display\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"doiurl\" "
         "value=\""
      << prefs.preferences.mFields["doiurl"]
      << "\" /> <i>no leading http(s)</i>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "     Url for lookup\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"doilookupurl\" value=\""
      << prefs.preferences.mFields["doilookupurl"]
      << "\" /> <i>no leading http(s)</i>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      CrossRef score\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"doicrossrefscore\" value=\""
      << prefs.preferences.mFields["doicrossrefscore"] << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>SSL</h2>\n"
      << "    </td>\n"
      << "  <tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      PEM\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" name=\"pem\" "
         "value=\""
      << prefs.preferences.mFields["pem"] << "\" />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>Look</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Splash screen at startup\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input name=\"splash\" type=\"checkbox\" value=\"true\""
      << splash << " />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Theme\n"
      << "    </td>\n"
      << "    <td>\n"
      << HTML::Select(list_themes(prefs.preferences.mFields["base"] +
                                  prefs.preferences.mFields["plugins"]),
                      prefs.preferences.mFields["scheme"], "scheme", "")
      << "\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Number authors to Display\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"nauthors\" value=\""
      << prefs.preferences.mFields["nauthors"] << "\" /> <i>-1 is all</i>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      View as Table\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input name=\"table\" type=\"checkbox\" value=\"true\"" << table
      << " />\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>Export</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td width=\"20%\">\n"
      << "      Key code\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input type=\"radio\" name=\"key\" value=\"biblcode\" "
      << (prefs.preferences.mFields["key"] == "biblcode" ? "checked=\"checked\""
                                                         : "")
      << "\" /> Bibl.\n"
      << "      <input type=\"radio\" name=\"key\" value=\"ADScode\" "
      << (prefs.preferences.mFields["key"] == "ADScode" ? "checked=\"checked\""
                                                        : "")
      << "\" /> ADS\n"
      << "      <input type=\"radio\" name=\"key\" value=\"doi\" "
      << (prefs.preferences.mFields["key"] == "doi" ? "checked=\"checked\""
                                                    : "")
      << "\" /> DOI\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td width=\"20%\">\n"
      << "      BibTeX file \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"bibtex\" "
         "value=\""
      << prefs.preferences.mFields["bibtex"] << "\" /> "
      << HTML::Filesize(prefs.preferences.mFields["base"] +
                        prefs.preferences.mFields["bibtex"])
      << "\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td width=\"20%\">\n"
      << "      MSWord file \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"msword\" "
         "value=\""
      << prefs.preferences.mFields["msword"] << "\" /> "
      << HTML::Filesize(prefs.preferences.mFields["base"] +
                        prefs.preferences.mFields["msword"])
      << "\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td width=\"20%\">\n"
      << "      Text file \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" name=\"text\" "
         "value=\""
      << prefs.preferences.mFields["text"] << "\" /> "
      << HTML::Filesize(prefs.preferences.mFields["base"] +
                        prefs.preferences.mFields["text"])
      << "\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Translate abbreviations\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input name=\"translate\" type=\"checkbox\" value=\"true\""
      << translate << "\" /> BibTeX\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td colspan=\"2\">\n"
      << "      <h2>Security</h2>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Credential expiration period\n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"expire\" "
         "value=\""
      << prefs.preferences.mFields["expire"] << "\"/> seconds\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "   <tr>\n"
      << "     <td colspan=\"2\">\n"
      << "       &nbsp;\n"
      << "     </td>\n"
      << "   </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Username \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"text\" "
         "name=\"username\" value=\""
      << prefs.preferences.mFields["username"] << "\" /> \n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      New password \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"password\" "
         "name=\"password\" /> \n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td>\n"
      << "      Confirm password \n"
      << "    </td>\n"
      << "    <td>\n"
      << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
         "autocomplete=\"off\" class=\"config\" type=\"password\" "
         "name=\"confirmpassword\" /> \n"
      << "    </td>\n"
      << "  </tr>\n"
      << "  <tr>\n"
      << "    <td id=\"submenu\" colspan=\"2\">\n"
      << "      <button title=\"Save config\" type=\"button\" "
         "onclick=\"if(document.config.password.value==''||document.config."
         "password.value==document.config.confirmpassword.value)document."
         "config.submit();else{alert('Confirmation password does not match "
         "new "
         "password, please try again');}\">Save</button> <button "
         "title=\"Undo "
         "changes\" type=\"reset\" onmouseover=\"window.status='Undo "
         "changes';return true\">Undo</button>\n"
      << "    </td>\n"
      << "  </tr>\n"
      << "</table>\n"
      << "</form>\n";
}

void DisplayImportForm(HTTP &http, [[maybe_unused]] Preferences &prefs) {

  std::cout << "<form name=\"importBibTeX\" action=\"" << http.self
            << "?action=importBibTeX\" enctype=\"multipart/form-data\" "
               "method=\"post\">\n"
            << "<table class=\"layout import\">\n"
            << "<tr>\n"
            << "  <td colspan=\"2\">\n"
            << "    <h1>Import</h1>\n"
            << "    <h2>BibTeX</h2> \n"
            << "  </td>\n"
            << "</tr>\n"
            << "  <tr>\n"
            << "    <td>\n"
            << "      BibTeX File\n"
            << "    </td>\n"
            << "    <td>\n"
            << "      <input autocorrect=\"off\" autocapitalize=\"off\" "
               "autocomplete=\"off\" class=\"file\" type=\"file\" "
               "name=\"upload\" />\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "<tr>\n"
            << "  <td id=\"submenu\" colspan=\"2\">\n"
            << "    <button id=\"importBibTeX\" title=\"Import\" "
               "type=\"submit\">Import BibTeX</button> <button id=\"reset\" "
               "title=\"Reset\" type=\"reset\">Reset</button>\n"
            << "  </td>\n"
            << "</tr>\n"
            << "</table>\n"
            << "</form>\n";
}

typedef struct {
  std::ostream &ostr;
  Preferences &prefs;
} ReindexRecordContext;

bool reindex_record(Record &record, long id, ReindexRecordContext &ctx) {
  ctx.ostr << "  <tr>\n"
           << "    <td>\n"
           << record.mFields["id"] << "\n"
           << "    </td>\n"
           << "    <td>\n"
           << id << "\n"
           << "    </td>\n"
           << "    <td>\n";

  std::filesystem::path SourcePath(ctx.prefs.preferences.mFields["base"] +
                                   "archive/" + record.mFields["id"] + ".pdf");

  if (record.mFields["archive"] == "on" &&
      std::filesystem::exists(SourcePath)) {
    std::filesystem::path DestinationPath(
        ctx.prefs.preferences.mFields["base"] + "archive/" +
        std::to_string(id) + ".pdf");
    std::filesystem::rename(SourcePath, DestinationPath);
    ctx.ostr << "      yes\n";
  } else {
    ctx.ostr << "      no\n";
  }

  ctx.ostr << "    </td>\n"
           << "  </tr>\n";

  return true;
}

void DisplayReindexForm([[maybe_unused]] HTTP &http, Preferences &prefs) {

  std::cout << "<table class=\"layout reindex\">\n"
            << "  <tr>\n"
            << "    <td id=\"col1\" colspan=\"3\">\n"
            << "      <h1>Reindex</h1>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <th id=\"old\">\n"
            << "      old\n"
            << "    </th>\n"
            << "    <th id=\"new\">\n"
            << "      new\n"
            << "    </th>\n"
            << "    <th id=\"pdf\">\n"
            << "      PDF\n"
            << "    </th>\n"
            << "  </tr>\n";

  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);

  if (!d.good()) {
    return;
  }

  ReindexRecordContext ctx = {std::cout, prefs};
  if (!d.ReindexRecords(ctx, reindex_record)) {
    return;
  }

  d.Commit();

  std::cout << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <td>\n"
            << "      &nbsp;\n"
            << "    </td>\n"
            << "    <td>\n"
            << "      &nbsp;\n"
            << "    </td>\n"
            << "    <td>\n"
            << "      &nbsp;\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "</table>\n";
}

void DisplayKeywords(HTTP &http, Preferences &prefs) {

  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);

  if (!d.good()) {
    return;
  }

  auto uniq = d.UniqueValuesForKey("keywords", split_on_comma);

  size_t n_uniq = uniq.size();

  std::cout << "<table class=\"layout keywords\">\n"
            << "  <tr>\n"
            << "    <td id=\"col1\" colspan=\"3\">\n"
            << "      <h1>Keywords" << " (" << n_uniq << ")</h1>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <th id=\"col1\">\n"
            << "      Keyword\n"
            << "    </th>\n"
            << "    <th id=\"col2\">\n"
            << "      Keyword\n"
            << "    </th>\n"
            << "    <th id=\"col3\">\n"
            << "      Keyword\n"
            << "    </th>\n"
            << "  </tr>\n";

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
    std::cout << "  <tr>\n";
    for (unsigned j = 0; j < 3; j++) {
      Keyword = "&nbsp;";
      if (j < col_per_row) {
        size_t offset = (j * n_per_col) + i;
        if (offset < n_uniq) {
          Keyword = Coders::LaTeXDecode(uniq[offset]);
          Keyword = "<a href=\"" + http.self +
                    "?action=search&amp;match=" + Coders::URLEncode(Keyword) +
                    "&amp;scheme=keywords\">" + Keyword + "</a>";
        }
      }
      std::cout << "    <td>\n"
                << "      " << Keyword << "\n"
                << "    </td>\n";
    }
    std::cout << "  </tr>\n";
  }
  std::cout << "</table>\n";
}

void DisplayAuthors(HTTP &http, Preferences &prefs) {

  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);

  if (!d.good()) {
    return;
  }

  auto uniq = d.UniqueValuesForKey("author", split_on_and);

  size_t n_uniq = uniq.size();

  std::cout << "<table class=\"layout authors\">\n"
            << "  <tr>\n"
            << "    <td id=\"col1\" colspan=\"3\">\n"
            << "      <h1>Authors" << " (" << n_uniq << ")</h1>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <th id=\"col1\">\n"
            << "      Keyword\n"
            << "    </th>\n"
            << "    <th id=\"col2\">\n"
            << "      Keyword\n"
            << "    </th>\n"
            << "    <th id=\"col3\">\n"
            << "      Keyword\n"
            << "    </th>\n"
            << "  </tr>\n";

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
    std::cout << "  <tr>\n";
    for (unsigned j = 0; j < 3; j++) {
      Author = "&nbsp;";
      if (j < col_per_row) {
        size_t offset = (j * n_per_col) + i;
        if (offset < n_uniq) {
          Author = Coders::LaTeXDecode(uniq[offset]);
          Author = "<a href=\"" + http.self +
                   "?action=search&amp;match=" + Coders::URLEncode(Author) +
                   "&amp;scheme=author\">" + Author + "</a>";
        }
      }
      std::cout << "    <td>\n"
                << "      " << Author << "\n"
                << "    </td>\n";
    }
    std::cout << "  </tr>\n";
  }
  std::cout << "</table>\n";
}

void DisplayDuplicatesForm(HTTP &http, Preferences &prefs) {

  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);

  if (!d.good()) {
    return;
  }

  auto duplicates = d.DuplicateRecordsForKey("biblcode");

  std::cout << "<form name=\"entry\" action=\"" << http.self
            << "?action=delete&amp;id=-1\" method=\"post\">\n"
            << "<table class=\"layout duplicates\">\n"
            << "  <tr>\n"
            << "   <td colspan=\"10\">\n"
            << "     <h1>Duplicates</h1>" << "   </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <td id=\"submenu\" colspan=\"10\">\n"
            << "      <button id=\"delete\" title=\"Delete selected entries\" "
               "type=\"button\" onclick=\"if(confirm('Are you sure you want to "
               "delete the selected "
               "entries?'))document.entry.submit();\">Delete</button> <button "
               "id=\"reset\" title=\"Reset\" type=\"reset\">Reset</button>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <th>\n"
            << "      <span title=\"Toggle, hot-key Alt + T, Ctrl + T "
               "(Apple)\"><input type=\"checkbox\" "
               "onclick=\"javascript:Toggle();\" accesskey=\"t\" /></span> \n"
            << "    </th>\n"
            << "    <th>\n"
            << "      Title/Booktitle\n"
            << "    </th>\n"
            << "    <th>\n"
            << "      Authors\n"
            << "    </th>\n"
            << "    <th>\n"
            << "      Journal\n"
            << "    </th>\n"
            << "    <th>\n"
            << "      Year\n"
            << "    </th>\n"
            << "    <th>\n"
            << "      Keyword(s)\n"
            << "    </th>\n"
            << "    <th>\n"
            << "      Abstract\n"
            << "    </th>\n"
            << "    <th>\n"
            << "      PDF\n"
            << "    </th>\n"
            << "    <th>\n"
            << "      C\n"
            << "    </th>\n"
            << "    <th>\n"
            << "      &nbsp;\n"
            << "    </th>\n"
            << "  </tr>\n";

  Strings s(prefs.preferences.mFields["base"] +
            prefs.preferences.mFields["abbreviation"]);

  for (auto &g : duplicates) {
    for (auto &r : g) {
      DisplayData(http, prefs, r, s.strings);
    }
    std::cout << "  <tr>\n"
              << "    <td class=\"divider\" colspan=\"10\">&nbsp;</td>\n"
              << "  </tr>\n";
  }

  std::cout << "  <tr id=\"results\">\n"
            << "    <td id=\"col1\" colspan=\"10\">\n"
            << duplicates.size() << " duplicat"
            << (duplicates.size() == 1 ? "e" : "es") << " found\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr id=\"submenu\">\n"
            << "    <td colspan=\"10\">\n"
            << "      <button id=\"delete\" title=\"Delete selected entries\" "
               "type=\"button\" onclick=\"if(confirm('Are you sure you want to "
               "delete the selected "
               "entries?'))document.entry.submit();\">Delete</button> <button "
               "id=\"reset\" title=\"Reset\" type=\"reset\">Reset</button>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "</table>\n"
            << "</form>\n"
            << "<script type=\"text/javascript\">\n"
            << "  var CheckValue = false;\n"
            << "  var FormName   = 'entry';\n"
            << "  var FieldName  = 'select';\n"
            << "\n"
            << "  function Toggle(){ \n"
            << "    CheckValue = !CheckValue;\n"
            << "    if(!document.forms[FormName])\n"
            << "      return;\n"
            << "    var objCheckBoxes = "
               "document.forms[FormName].elements[FieldName];\n"
            << "    if(!objCheckBoxes)\n"
            << "      return;\n"
            << "    var countCheckBoxes = objCheckBoxes.length;\n"
            << "    if(!countCheckBoxes)\n"
            << "     objCheckBoxes.checked = CheckValue;\n"
            << "    else\n"
            << "      for(var i = 0; i < countCheckBoxes; i++)\n"
            << "        objCheckBoxes[i].checked = CheckValue;\n"
            << "   }\n"
            << "</script>\n";
}

void DisplayDOICrossrefForm(HTTP &http, Preferences &prefs) {

  std::cout << "<table class=\"layout dois\">\n"
            << "  <tr>\n"
            << "    <td id=\"col1\" colspan=\"2\">\n"
            << "      <h1>CrossRef DOIs</h1>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "  <tr>\n"
            << "    <th id=\"id\">\n"
            << "      id\n"
            << "    </th>\n"
            << "    <th id=\"doi\">\n"
            << "      doi\n"
            << "    </th>\n"
            << "  </tr>\n";
  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);

  if (!d.good()) {
    return;
  }

  float need_score =
      strtof(prefs.preferences.mFields["doicrossrefscore"].c_str(), nullptr);

  for (auto &record : d.vRecords) {

    std::string doi(record.mFields["doi"]);

    if (!doi.empty()) {
      continue;
    }

    std::string id_str(record.mFields.at("id")),
        crossref_str(record.mFields["doicrossrefstatus"]);

    if (!crossref_str.empty()) {
      std::cout << "  <tr>\n"
                << "    <td>\n"
                << "      <a href=\"" << http.self
                << "?action=open&amp;id=" << id_str << "\">" << id_str << "</a>"
                << "\n"
                << "    </td>\n"
                << "    <td>\n"
                << crossref_str << "\n"
                << "    </td>\n"
                << "  </tr>\n";
      continue;
    }

    std::string author(Coders::LaTeXDecode(record.mFields["author"]));

    std::string::size_type pos = author.find(", ");
    if (pos != std::string::npos) {
      author = author.substr(0, pos);
    }
    std::string url(
        prefs.preferences.mFields["doilookupurl"] +
        "/v1/works?rows=1&sort=score&order=desc" +
        "&query.bibliographic=" + Coders::URLEncode(record.mFields["title"]) +
        "&query.author=" + Coders::URLEncode(author) +
        "&filter=from-pub-date:" + record.mFields["year"] + ",until-pub-date:" +
        record.mFields["year"] + "&select=DOI,score" + "&mailto=" CONTACT);

    std::string crossref(
        http.SecureGet(url, {}, 443, prefs.preferences.mFields["pem"].c_str()));

    std::cout << "  <tr>\n"
              << "    <td>\n"
              << "      <a href=\"" << http.self
              << "?action=open&amp;id=" << id_str << "\">" << id_str << "</a>"
              << "\n"
              << "    </td>\n"
              << "    <td>\n";

    rapidjson::Document json;
    if (json.Parse(crossref.c_str()).HasParseError()) {
      std::string value("JSON parse error (");
      value.append(GetParseError_En(json.GetParseError()));
      value.append(")");

      record.mFields["doicrossrefstatus"] = value;
      std::cout << value << "\n"
                << "    </td>\n"
                << "  </tr>\n";
      continue;
    }

    if (!json.HasMember("message") || !json["message"].IsObject() ||
        !json["message"].HasMember("total-results") ||
        json["message"]["total-results"].GetInt() == 0) {
      record.mFields["doicrossrefstatus"] =
          "no matching crossref found (no results)";
      std::cout << record.mFields["doicrossrefstatus"] << "\n"
                << "    </td>\n"
                << "  </tr>\n";
      continue;
    }

    if (!json["message"].HasMember("items") ||
        !json["message"]["items"].IsArray() ||
        json["message"]["items"].Size() == 0 ||
        !json["message"]["items"][0].HasMember("DOI") ||
        json["message"]["items"][0]["DOI"].GetStringLength() == 0 ||
        !json["message"]["items"][0].HasMember("score")) {
      record.mFields["doicrossrefstatus"] =
          "no matching crossref found (malformed message)";
      std::cout << record.mFields["doicrossrefstatus"] << "\n"
                << "    </td>\n"
                << "  </tr>\n";
      continue;
    }

    doi = json["message"]["items"][0]["DOI"].GetString();

    float score = json["message"]["items"][0]["score"].GetFloat();
    if (score < need_score) {
      std::string value("score too low [");
      value = value.append(doi) + "] (" + std::to_string(score) + '<' +
              std::to_string(need_score) + ')';
      record.mFields["doicrossrefstatus"] = value;
      std::cout << record.mFields["doicrossrefstatus"] << '\n'
                << "    </td>\n"
                << "  </tr>\n";
      continue;
    }

    record.mFields["doi"] = std::string("{") + doi + "}";

    std::cout << doi << "\n"
              << "    </td>\n"
              << "  </tr>\n";
  }
  std::cout << "</table>\n";

  d.Commit();
}

void Delete(HTTP &http, Preferences &prefs) {

  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);

  if (!d.good()) {
    return;
  }

  auto helper = [&d, &prefs](std::string id_str) {
    if (!d.RemoveRecord(id_str)) {
      return false;
    }
    std::filesystem::path TargetPath(prefs.preferences.mFields["base"] +
                                     "archive/" + id_str + ".pdf");

    if (std::filesystem::exists(TargetPath)) {
      std::filesystem::remove(TargetPath);
    }
    return (true);
  };

  std::string id_str(http.get.mFields.at("id"));
  if (id_str != "-1") {
    if (!helper(id_str)) {
      return;
    }
  } else {
    std::vector<std::string> vIdentifiers =
        split_on_comma(http.post.mFields["select"]);

    for (const auto &id_str : vIdentifiers) {
      if (!helper(id_str)) {
        return;
      }
    }
  }

  d.Commit();

  DisplayDataList(http, prefs);
}

void Import(HTTP &http, Preferences &prefs) {

  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);

  if (!d.good()) {
    return;
  }

  size_t size = 0;

  auto record = BibTeX::Parse(http.file_data, size);
  while (record.mFields.size()) {
    d.SetRecord(record, "-1");
    record = BibTeX::Parse(http.file_data, size);
  }

  d.Commit();

  DisplayDataList(http, prefs);
}

void Export(HTTP &http, Preferences &prefs) {

  std::string format(http.get.mFields["action"]);
  format = format.substr(6);

  std::cout << "<table class=\"layout export\">\n"
            << "<tr>\n"
            << "  <td>\n"
            << "    <h1>Export</h1>\n"
            << "    <h2>" << format << "</h2> \n"
            << "  </td>\n"
            << "  <td id=\"submenu\">\n"
            << "    <button id=\"download\" title=\"Download File\" "
               "type=\"button\" onclick=\"gotoURL('";

  for (char &c : format) {
    c = tolower(c);
  }

  std::cout << prefs.preferences.mFields["baseurl"]
            << prefs.preferences.mFields[format]
            << "')\">Download File</button>\n"
            << "  </td>\n"
            << "</tr>\n";

  std::filesystem::path filename(prefs.preferences.mFields["base"] +
                                 prefs.preferences.mFields[format]);

  std::ofstream ofstr(filename);
  if (ofstr.fail()) {
    return;
  }

  Database d(prefs.preferences.mFields["base"] +
             prefs.preferences.mFields["data"]);

  if (!d.good()) {
    ofstr.close();
    return;
  }

  Strings s(prefs.preferences.mFields["base"] +
            prefs.preferences.mFields["abbreviation"]);

  int n_export = 0;

  std::string id_str(http.get.mFields.at("id"));
  if (id_str != "-1") {
    if (format == "bibtex") {
      BibTeX::Setup setup = {prefs.preferences, s.strings};
      d.ExportRecord(id_str, ofstr, setup, BibTeX::Export);
      ++n_export;
    } else if (format == "msword") {
      MSWord::Header(ofstr);
      MSWord::Setup setup = {prefs.preferences, s.strings};
      d.ExportRecord(id_str, ofstr, setup, MSWord::Export);
      MSWord::Footer(ofstr);
      ++n_export;
    } else if (format == "text") {
      Text::Setup setup = {prefs.preferences, s.strings};
      d.ExportRecord(id_str, ofstr, setup, Text::Export);
      ++n_export;
    } else {
      return;
    }
  } else {
    if (format == "msword") {
      MSWord::Header(ofstr);
    }
    std::vector<std::string> vIdentifiers =
        split_on_comma(http.post.mFields["select"]);
    for (const auto &id_str : vIdentifiers) {
      if (format == "bibtex") {
        BibTeX::Setup setup = {prefs.preferences, s.strings};
        d.ExportRecord(id_str, ofstr, setup, BibTeX::Export);
        ++n_export;
      } else if (format == "msword") {
        MSWord::Setup setup = {prefs.preferences, s.strings};
        d.ExportRecord(id_str, ofstr, setup, MSWord::Export);
        ++n_export;
      } else if (format == "text") {
        Text::Setup setup = {prefs.preferences, s.strings};
        d.ExportRecord(id_str, ofstr, setup, Text::Export);
        ++n_export;
      } else {
        return;
      }
    }
    if (format == "msword") {
      MSWord::Footer(ofstr);
    }
  }

  ofstr.close();

  std::cout << "  <tr>\n"
            << "    <td>\n"
            << "      Exported " << n_export << " record(s) to `"
            << prefs.preferences.mFields["base"]
            << prefs.preferences.mFields[format] << "'\n"
            << "    </td>\n"
            << "    <td id=\"submenu\">\n"
            << "      <button id=\"download\" title=\"Download File\" "
               "type=\"button\" onclick=\"gotoURL('"
            << prefs.preferences.mFields["baseurl"]
            << prefs.preferences.mFields[format]
            << "')\">Download File</button>\n"
            << "    </td>\n"
            << "  </tr>\n"
            << "</table>\n";
}
