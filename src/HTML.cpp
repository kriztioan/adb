/**
 *  @file   HTML.cpp
 *  @brief  HTML Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "HTML.h"

std::string HTML::Select(std::string_view options, const std::string &selected,
                   const std::string &name, const std::string &onchange) {

  std::string select("      <select class=\"select\" name=\"" + name +
                     "\" onchange=\"" + onchange + ";\">"),
      option;

  std::string::size_type size, start = 0, end;

  if ((size = options.length()) > 0) {
    while (start < size) {
      end = options.find_first_of("&", start);
      if (end == std::string::npos) {
        end = options.length();
      }
      option = options.substr(start, end - start);
      select += "\n        <option value=\"" + option + "\"";
      if (selected == option) {
        select += " selected=\"selected\"";
      }
      select += ">" + option + "</option>";
      start = end + 1;
    }
  } else {
    select += "\n        <option value=\"" + selected + "\">" + selected +
              "</option>";
  }
  select += "\n      </select>";

  return (select);
}

std::string HTML::Highlight(std::string_view body, std::string_view match) {

  std::string str;
  int body_len = body.length(), match_len = match.length();
  for (int pos = 0; pos < body_len; pos++) {
    if (body[pos] == '<') {
      while (body[pos] != '>') {
        str += body[pos];
        if (++pos > body_len) {
          return (str);
        }
      }
      str += body[pos];
    } else {
      int seek = 0, postmp = pos;
      while (tolower(body[postmp]) == tolower(match[seek])) {
        if ((++postmp > body_len - 1) || (++seek > match_len - 1)) {
          break;
        }
      }
      if (seek == match_len) {
        str += "<span class=\"highlight\" title=\"highlighted\">";
        for (; pos < postmp; pos++) {
          str += body[pos];
        }
        str += "</span>";
      }
      str += body[pos];
    }
  }

  return (str);
}

std::string HTML::Filesize(std::string_view filename) {
  if (!filename.empty()) {
    if (!std::filesystem::exists(filename)) {
      return (" <font color=\"#ff0000\">File not found!</font>");
    } else {
      std::uintmax_t size = std::filesystem::file_size(filename);
      std::stringstream ss;
      if (size < 1e3) {
        ss << size;
        return (ss.str() + " bytes");
      } else {
        ss.setf(std::ios::fixed);
        ss << std::setprecision(1) << (size / 1e3);
        return (ss.str() + " kB");
      }
    }
  }
  return (std::string());
}