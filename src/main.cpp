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

static std::string_view defaults =
    "$abbreviation=\"data/AAS.abr\"\n"
    "$administrator=\"christiaanboersma@hotmail.com\"\n"
    "$adsurl=\"ui.adsabs.harvard.edu\"\n"
    "$adstoken=\"\"\n"
    "$base=\"./\"\n"
    "$baseurl=\"http://localhost/adb/\"\n"
    "$bibtex=\"data/bibliography.bib\"\n"
    "$data=\"data/database.dat\"\n"
    "$doicrossrefscore=\"19.0\"\n"
    "$doilookupurl=\"api.crossref.org\"\n"
    "$doiurl=\"doi.org\"\n"
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
    "$username=\"user\"\n";

int main() {
  std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();

  std::ios::sync_with_stdio(false);

  std::cin.tie(nullptr);

  HTTP http;

  std::string_view action;
  auto get_it = http.get["action"];
  if (get_it != http.get.end()) {
    action = get_it->second;
  }

  Preferences prefs(CONFIG, defaults);

  auto post_it = http.post["save"];
  if (action == "config" && post_it != http.post.end() &&
      post_it->second == "true") {
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
    std::find_if(std::begin(actions), std::end(actions), [](const auto &item) {
      return item.first == "splash";
    })->second(http, prefs);
  }
  DisplayFooter(t0);

  WriteHTMLFooter();

  WriteOutput();

  return EXIT_SUCCESS;
}
