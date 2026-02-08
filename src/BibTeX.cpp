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
                                 std::string_view self) {

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
    html += "<span title=\"Search for &apos;" + author + "&apos;\"><a href=\"";
    html = html.append(self) + "?action=search&match=" + author +
           "&scheme=author\">" + author + "</a></span>";
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

std::string BibTeX::SplitKeywords(std::string_view keywords,
                                  std::string_view self) {

  std::string html;

  html.reserve(256);

  std::string::size_type begin = 0, end;

  while ((end = keywords.find(", ", begin)) != std::string::npos) {
    std::string keyword(keywords, begin, end - begin);
    html += " <a href=\"";
    html = html.append(self) + "?action=search&match=" + keyword +
           "&scheme=keywords\">" + keyword + "</a>";
    begin = end + 2;
  }

  if (!keywords.empty()) {
    std::string keyword(keywords.substr(begin));
    html += " <a href=\"";
    html = html.append(self) + "?action=search&match=" + keyword +
           "&scheme=keywords\">" + keyword + "</a>";
  }

  return (html);
}

Record BibTeX::Parse(std::string_view bibtex, size_t &nbytes_parsed,
                     Pool &pool) {

  pool.begin();
  pool << "id=-1&";

  std::string buff;
  buff.reserve(4096);

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
      buff += bibtex[nbytes_parsed];
    ++nbytes_parsed;
  }
  if (nbytes_parsed >= bibtex.length() || bibtex[nbytes_parsed] != '{') {
    return (Record{});
  }
  pool << "type=" << Encoding::URLEncode(buff.c_str())
       << "&ADSabstract=on&ADSfullpaper=on";
  buff.clear();
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
      buff += bibtex[nbytes_parsed];
    ++nbytes_parsed;
  }
  if (nbytes_parsed >= bibtex.length() ||
      (bibtex[nbytes_parsed] != '}' && bibtex[nbytes_parsed] != ','))
    return (Record{});
  else if (bibtex[nbytes_parsed] == '}') {
    return (Record{});
  }
  pool << "&biblcode=" << Encoding::URLEncode(buff.c_str());

  buff.clear();
  ++nbytes_parsed;

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
        buff += bibtex[nbytes_parsed];
      ++nbytes_parsed;
    }
    if (nbytes_parsed >= bibtex.length() || bibtex[nbytes_parsed] != '=') {
      return (Record{});
    }
    pool << '&' << Encoding::URLEncode(buff.c_str());
    buff.clear();
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
      else if (bibtex[nbytes_parsed] == '{') {
        ++balance_brackets;

        buff += bibtex[nbytes_parsed++];
        while (balance_brackets != 0 && nbytes_parsed < bibtex.length()) {

          if (bibtex[nbytes_parsed] == '{')
            ++balance_brackets;
          else if (bibtex[nbytes_parsed] == '}')
            --balance_brackets;

          if (bibtex[nbytes_parsed] == '\n' || bibtex[nbytes_parsed] == '\r') {
            while (isspace(bibtex[nbytes_parsed]) &&
                   nbytes_parsed < bibtex.length())
              ++nbytes_parsed;
            buff += ' ';
          } else {
            buff += bibtex[nbytes_parsed++];
          }
        }
        nbytes_parsed--;
      } else if (!isspace(bibtex[nbytes_parsed]))
        buff += bibtex[nbytes_parsed];
      ++nbytes_parsed;
    }
    if (nbytes_parsed >= bibtex.length() ||
        (bibtex[nbytes_parsed] != '}' && bibtex[nbytes_parsed] != ',')) {
      return (Record{});
    } else if (bibtex[nbytes_parsed] == '}') {
      pairs = false;
    }

    pool << '=' << Encoding::URLEncode(buff.c_str());
    buff.clear();
    ++nbytes_parsed;
  }
  ++nbytes_parsed;

  pool << '\0';
  return (Record(const_cast<char *>(pool.sv().data())));
}

bool BibTeX::Export(Record &record, std::ostream &ostr, std::string_view &key) {

  auto field_end = record.end();

  std::string_view type;
  auto field_it = record["type"];
  if (field_it != field_end) {
    type = field_it->second;
  }

  ostr << "@" << type << "{";
  field_it = record[key];
  if (field_it != field_end) {
    ostr << field_it->second;
  }

  static constexpr std::string_view keywords[] = {
      "ADScode", "ADSabstract", "ADSfullpaper", "id",      "keywords",
      "URL",     "type",        "biblcode",     "archive", "doicrossrefstatus"};

  auto mFields = record.mFields;
  for (const auto keyword : keywords) {
    field_it = mFields.find(keyword);
    if (field_it != mFields.end()) {
      mFields.erase(field_it);
    }
  }

  for (const auto &field : mFields) {
    ostr << ",\n"
         << std::setw(10) << std::setiosflags(std::ios::right) << field.first
         << " = " << field.second;
  }

  ostr << "\n}\n\n";

  return (true);
}
