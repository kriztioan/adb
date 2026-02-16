/**
 *  @file   HTML.cpp
 *  @brief  HTML Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "HTML.h"

std::string_view HTML::Select(std::string_view options,
                              std::string_view selected, std::string_view name,
                              std::string_view onchange, Pool &pool) {

  pool.begin();
  pool << "      <select class=\"select\" name=\"" << name << "\" onchange=\""
       << onchange << ";\">";

  std::string_view option;
  std::string_view::size_type size, start = 0, end;

  if ((size = options.length()) > 0) {
    while (start < size) {
      end = options.find_first_of("&", start);
      if (end == std::string::npos) {
        end = options.length();
      }
      option = options.substr(start, end - start);
      pool << "\n        <option value=\"" << option << '\"';
      if (selected == option) {
        pool << " selected=\"selected\"";
      }
      pool << '>' << option << "</option>";
      start = end + 1;
    }
  } else {
    pool << "\n        <option value=\"" << selected << "\">" << selected
         << "</option>";
  }
  pool << "\n      </select>";

  return pool.sv();
}

std::string_view HTML::SplitAuthors(std::string_view authors,
                                    std::string_view self, Pool &pool,
                                    int max_authors) {

  if (authors.empty()) {
    return std::string_view();
  }

  pool.begin();

  std::string_view::size_type beg = 0, end = authors.find(" and ");

  auto output_author = [&]() {
    std::string author;

    for (const auto &c : authors.substr(beg, end - beg)) {
      if (c != '{' && c != '}') {
        author += c;
      }
    }
    pool << "<span title=\"Search for author &apos;" << author
         << "&apos;\"><a href=\"" << self << "?action=search&match=" << author
         << "&scheme=author\">" << author << "</a></span>";
  };

  int nauthors = 1;
  output_author();
  if (end != std::string_view::npos) {
    while (nauthors < max_authors || max_authors == -1) {
      beg = end + 5;
      end = authors.find(" and ", beg);
      pool << ", ";
      output_author();
      ++nauthors;
      if (end == std::string_view::npos) {
        break;
      }
    }
    if (nauthors == max_authors && end != std::string_view::npos) {
      pool << " <span class=\"etal\">et al.</span>";
    }
  }

  return pool.sv();
}

std::string_view HTML::SplitKeywords(std::string_view keywords,
                                     std::string_view self, Pool &pool,
                                     int max_keywords,
                                     std::string_view separator) {

  if (keywords.empty()) {
    return std::string_view();
  }

  pool.begin();

  std::string::size_type beg = 0, end = keywords.find(", ");

  auto output_keyword = [&]() {
    std::string_view keyword(keywords.substr(beg, end - beg));
    pool << "<span title=\"Search for keyword &apos;" << keyword
         << "&apos;\"><a href=\"" << self << "?action=search&match=" << keyword
         << "&scheme=keywords\">" << keyword << "</a></span>";
  };

  int nkeywords = 1;
  output_keyword();
  if (end != std::string_view::npos) {
    while (nkeywords < max_keywords || max_keywords == -1) {
      beg = end + 2;
      end = keywords.find(", ", beg);
      pool << separator;
      output_keyword();
      ++nkeywords;
      if (end == std::string_view::npos) {
        break;
      }
    }
    if (nkeywords == max_keywords && end != std::string_view::npos) {
      pool << " <span class=\"ellipsis\">&hellip;</span>";
    }
  }

  return pool.sv();
}

std::string_view HTML::Filesize(std::filesystem::path &f, Pool &pool) {
  if (!std::filesystem::exists(f)) {
    return " <font color=\"#ff0000\">File not found!</font>";
  } else {
    pool.begin();
    std::uintmax_t size = std::filesystem::file_size(f);
    if (size < 1e3) {
      pool << size << " bytes";
      return pool.sv();
    } else {
      pool.setf(std::ios::fixed);
      pool << std::setprecision(1) << (size / 1e3) << " kB";
      return pool.sv();
    }
  }
  return "";
}
