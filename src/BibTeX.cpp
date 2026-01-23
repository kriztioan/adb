/**
 *  @file   BibTeX.cpp
 *  @brief  BibTeX Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "BibTeX.h"

std::string BibTeX::SplitAuthors(std::string_view authors, int max_authors,
                                 const std::string &self) {

  std::string html;
  if (authors.empty()) {
    return (html);
  }

  html.reserve(128);

  std::string::size_type beg = 0, end = authors.find(" and ");

  auto output_html = [&]() {
    std::string author;

    for (const auto &c : authors.substr(beg, end - beg)) {
      if (c != '{' && c != '}') {
        author += c;
      }
    }
    html += "<span title=\"Search for &apos;" + author + "&apos;\"><a href=\"" +
            self + "?action=search&amp;match=" + Coders::HTMLEncode(author) +
            "&amp;scheme=author\">" + author + "</a></span>";
  };

  int nauthors = 1;
  output_html();
  if (end != std::string::npos) {
    while (nauthors < max_authors || max_authors == -1) {
      beg = end + 5;
      end = authors.find(" and ", beg);
      html += ", ";
      output_html();
      ++nauthors;
      if (end == std::string::npos) {
        break;
      }
    }
    if (nauthors == max_authors && end != std::string::npos) {
      html += " <span class=\"etal\">et al.</span>";
    }
  }

  return (html);
}

std::string BibTeX::SplitKeywords(const std::string &keywords,
                                  const std::string &self) {

  std::string html;

  html.reserve(256);

  std::string::size_type begin = 0, end;

  while ((end = keywords.find(", ", begin)) != std::string::npos) {
    std::string keyword(keywords, begin, end - begin);
    html += " <a href=\"" + self + "?action=search&amp;match=" + keyword +
            "&amp;scheme=keywords\">" + keyword + "</a>";
    begin = end + 2;
  }

  if (!keywords.empty()) {
    std::string keyword(keywords, begin);
    html += " <a href=\"" + self + "?action=search&amp;match=" + keyword +
            "&amp;scheme=keywords\">" + keyword + "</a>";
  }

  return (html);
}

Record BibTeX::Parse(std::string_view bibtex, size_t &nbytes_parsed) {

  std::string data("id=-1&");

  char buff[4096];
  size_t buff_offset = 0;

  // part I
  while (nbytes_parsed < bibtex.length() && bibtex[nbytes_parsed] != '@') {
    if (bibtex[nbytes_parsed] == '%') {
      while (bibtex[nbytes_parsed] != '\n' && nbytes_parsed < bibtex.length())
        ++nbytes_parsed;
    }
    ++nbytes_parsed;
  }
  if (nbytes_parsed >= bibtex.length())
    return (Record{});
  else if (bibtex[nbytes_parsed] != '@')
    return (Record{});
  ++nbytes_parsed;

  // part II

  while (bibtex[nbytes_parsed] != '{' && nbytes_parsed < bibtex.length()) {
    if (bibtex[nbytes_parsed] == '%') {
      while (bibtex[nbytes_parsed] != '\n' && nbytes_parsed < bibtex.length())
        ++nbytes_parsed;
    } else if (bibtex[nbytes_parsed] == '@' || bibtex[nbytes_parsed] == '=' ||
               bibtex[nbytes_parsed] == '}' || bibtex[nbytes_parsed] == ',')
      break;
    else if (!isspace(bibtex[nbytes_parsed]))
      buff[buff_offset++] = bibtex[nbytes_parsed];
    ++nbytes_parsed;
  }
  if (nbytes_parsed >= bibtex.length() || bibtex[nbytes_parsed] != '{')
    return (Record{});
  buff[buff_offset] = '\0';
  data += "type=" + Coders::URLEncode(buff) + "&ADSabstract=on&ADSfullpaper=on";
  buff_offset = 0;
  ++nbytes_parsed;

  // part III

  while (bibtex[nbytes_parsed] != ',' && bibtex[nbytes_parsed] != '}' &&
         nbytes_parsed < bibtex.length()) {
    if (bibtex[nbytes_parsed] == '%') {
      while (bibtex[nbytes_parsed] != '\n' && nbytes_parsed < bibtex.length())
        ++nbytes_parsed;
    } else if (bibtex[nbytes_parsed] == '@' || bibtex[nbytes_parsed] == '=' ||
               bibtex[nbytes_parsed] == '{')
      break;
    else if (!isspace(bibtex[nbytes_parsed]))
      buff[buff_offset++] = bibtex[nbytes_parsed];
    ++nbytes_parsed;
  }
  if (nbytes_parsed >= bibtex.length() ||
      (bibtex[nbytes_parsed] != '}' && bibtex[nbytes_parsed] != ','))
    return (Record{});
  else if (bibtex[nbytes_parsed] == '}')
    return (Record{});
  buff[buff_offset] = '\0';
  data += "&biblcode=" + Coders::URLEncode(buff);
  buff_offset = 0;
  ++nbytes_parsed;

  //

  bool pairs = true;
  while (pairs) {

    // part IV

    while (bibtex[nbytes_parsed] != '=' && nbytes_parsed < bibtex.length()) {
      if (bibtex[nbytes_parsed] == '%') {
        while (bibtex[nbytes_parsed] != '\n' && nbytes_parsed < bibtex.length())
          ++nbytes_parsed;
      } else if (bibtex[nbytes_parsed] == '@' || bibtex[nbytes_parsed] == '{' ||
                 bibtex[nbytes_parsed] == '}' || bibtex[nbytes_parsed] == ',')
        break;
      else if (!isspace(bibtex[nbytes_parsed]))
        buff[buff_offset++] = bibtex[nbytes_parsed];
      ++nbytes_parsed;
    }
    if (nbytes_parsed >= bibtex.length() || bibtex[nbytes_parsed] != '=')
      return (Record{});
    buff[buff_offset] = '\0';
    data += '&' + Coders::URLEncode(buff);
    buff_offset = 0;
    ++nbytes_parsed;

    // part V

    int balance_brackets = 0;
    while (bibtex[nbytes_parsed] != ',' && bibtex[nbytes_parsed] != '}' &&
           nbytes_parsed < bibtex.length()) {
      if (bibtex[nbytes_parsed] == '%') {
        while (bibtex[nbytes_parsed] != '\n' && nbytes_parsed < bibtex.length())
          ++nbytes_parsed;
      } else if (bibtex[nbytes_parsed] == '@' || bibtex[nbytes_parsed] == '=')
        break;

      /*
        else if(bibtex[nbytes_parsed] == '\"'){
        while(bibtex[nbytes_parsed] != '\"' && nbytes_parsed < bibtex.length()){
        buff[buff_offset++] = bibtex[nbytes_parsed];
        ++nbytes_parsed;
        }
        }
      */

      else if (bibtex[nbytes_parsed] == '{') {
        ++balance_brackets;

        buff[buff_offset++] = bibtex[nbytes_parsed++];
        while (balance_brackets != 0 && nbytes_parsed < bibtex.length()) {

          if (bibtex[nbytes_parsed] == '{')
            ++balance_brackets;
          else if (bibtex[nbytes_parsed] == '}')
            --balance_brackets;

          if (bibtex[nbytes_parsed] == '\n' || bibtex[nbytes_parsed] == '\r') {
            while (isspace(bibtex[nbytes_parsed]) &&
                   nbytes_parsed < bibtex.length())
              ++nbytes_parsed;
            buff[buff_offset] = ' ';
          } else
            buff[buff_offset++] = bibtex[nbytes_parsed++];
        }
        nbytes_parsed--;
      } else if (!isspace(bibtex[nbytes_parsed]))
        buff[buff_offset++] = bibtex[nbytes_parsed];
      ++nbytes_parsed;
    }
    if (nbytes_parsed >= bibtex.length() ||
        (bibtex[nbytes_parsed] != '}' && bibtex[nbytes_parsed] != ','))
      return (Record{});
    else if (bibtex[nbytes_parsed] == '}')
      pairs = false;
    buff[buff_offset] = '\0';
    data += '=' + Coders::URLEncode(buff);
    buff_offset = 0;
    ++nbytes_parsed;
  }
  ++nbytes_parsed;

  return (Record(data.data()));
}

bool BibTeX::Export(Record &record, std::ostream &ostr, BibTeX::Setup &setup) {

  // get type
  std::string type(record.mFields["type"]);

  // get biblcode
  std::string biblcode(
      Coders::LaTeXDecode(record.mFields.at(setup.prefs.mFields.at("key"))));

  // get journal
  std::string journal(record.mFields["journal"]);

  if (type == "ARTICLE" && setup.prefs.mFields["translate"] == "true") {

    if (setup.strings.mFields.size()) {
      auto it = setup.strings.mFields.find(journal);
      if (it != setup.strings.mFields.end()) {
        journal = it->second;
      }
    }
  }

  // get rid of the AdB's own keywords, type, biblcode and journal
  std::unordered_map<std::string, std::string> mFields = record.mFields;

  static constexpr const char *AdBKeywords[] = {
      "ADScode", "ADSabstract",      "ADSfullpaper",
      "id",      "keywords",         "URL",
      "type",    "biblcode",         "archive",
      "journal", "doicrossrefstatus"};

  for (const auto *s : AdBKeywords) {
    auto it = mFields.find(s);
    if (it != mFields.end()) {
      mFields.erase(it);
    }
  }

  ostr << "@" << type << "{" << biblcode;

  if (!journal.empty())
    ostr << ",\n"
         << std::setw(10) << std::setiosflags(std::ios::right) << "journal"
         << " = " << journal;

  for (const auto &field : mFields) {
    if (!field.second.empty()) {
      ostr << ",\n"
           << std::setw(10) << std::setiosflags(std::ios::right) << field.first
           << " = " << field.second;
    }
  }
  ostr << "\n}\n\n";

  return (true);
}
