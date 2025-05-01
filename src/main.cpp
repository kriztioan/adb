/**
 *  @file   main.cpp
 *  @brief  Article Database (AdB)
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "HTTP.h"
#include "Preferences.h"

#include "Actions.h"
#include "HTML.h"

#include <chrono>

#include "config.h"

static std::string defaults("$abbreviation=\"data/AAS.abr\"\n"
                            "$administrator=\"christiaanboersma@hotmail.com\"\n"
                            "$adsurl=\"ui.adsabs.harvard.edu\"\n"
                            "$adstoken=\"\"\n"
                            "$base=\"./\"\n"
                            "$baseurl=\"http://localhost/adb/\"\n"
                            "$bibtex=\"data/bibliography.bib\"\n"
                            "$data=\"data/database.dat\"\n"
                            "$doicrossrefscore=\"19.0\"\n"
                            "$doilookupurl=\"api.crossref.org\"\n"
                            "$doiurl=\"dx.doi.org\"\n"
                            "$expire=\"31536000\"\n"
                            "$key=\"biblcode\"\n"
                            "$msword=\"Sources.xml\"\n"
                            "$nauthors=\"3\"\n"
                            "$pem=\"/private/etc/ssl/cert.pem\"\n"
                            "$plugins=\"plugins/\"\n"
                            "$scheme=\"Default\"\n"
                            "$splash=\"true\"\n"
                            "$table=\"true\"\n"
                            "$text=\"data/references.txt\"\n"
                            "$updates=\"false\"\n"
                            "$username=\"user\"\n");

int main() {

  std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();

  HTTP http;

  std::string action;
  if (http.get.mFields.contains("action")) {
    action = http.get.mFields.at("action");
  }

  Preferences prefs(CONFIG, defaults.data());

  if (action == "config" && http.post.mFields.contains("save") &&
      http.post.mFields.at("save") == "true") {
    prefs.preferences = http.post;
    prefs.Save();
  }

  HTTP::WriteHeader();

  WriteHTMLHeader(prefs);

  DisplayMenu(http, prefs);
  auto a = std::find_if(
      std::begin(actions), std::end(actions),
      [&action](const auto &item) { return item.first == action; });
  if (a != std::end(actions)) {
    a->second(http, prefs);
  } else {
    http.uri += "?action=list";
    std::find_if(std::begin(actions), std::end(actions), [](const auto &item) {
      return item.first == "splash";
    })->second(http, prefs);
  }
  DisplayFooter(t0);

  WriteHTMLFooter();

  return (0);
}
