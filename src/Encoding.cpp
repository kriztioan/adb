/**
 *  @file   Encoding.cpp
 *  @brief  Encoding Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Encoding.h"

char *Encoding::URLDecodeInplace(char *str) {

  size_t i;
  char *p = str, *q = p, r[3] = {0}, *s;
  while (*p) {
    if (*p == '+') {
      *q++ = ' ';
      ++p;
    } else if (*p == '%') {
      i = 0;
      s = r;
      while (++p && i++ < 2) {
        *s++ = *p;
      }
      std::from_chars(r, r + 2, i, 16);
      *q++ = static_cast<char>(i);
    } else {
      *q++ = *p++;
    }
  }
  *q = '\0';

  return str;
}

std::string Encoding::URLEncode(std::string_view sv) {

  static constexpr const char reserved[] = "$&+,/:;=?@{}";

  std::string_view::size_type len = sv.length();

  std::string str;

  str.reserve(len);

  char buff[4];

  size_t j;
  for (std::string_view::size_type i = 0; i < len; i++) {
    if (sv[i] == ' ') {
      str += '+';
    } else {
      for (j = 0; j < sizeof(reserved); j++) {
        if (sv[i] == reserved[j]) {
          snprintf(buff, 4, "%%%2X", (int)reserved[j]);
          str += buff;
          break;
        }
      }
      if (j == sizeof(reserved)) {
        str += sv[i];
      }
    }
  }
  return str;
}

std::string_view Encoding::LaTeXDecode(std::string_view sv, Pool &pool) {

  std::string_view::size_type len = sv.length();

  pool.begin();
  for (std::string_view::size_type i = 0; i < len; i++) {
    if (sv[i] == '_') {
      if (i + 1 == len) {
        pool << '_';
        break;
      }
      std::string_view::size_type j = i + 1;
      if (j + 1 == len) {
        pool << sv.substr(i);
        break;
      }
      if (sv[j] == '{') {
        ++j;
      }
      if (j == len) {
        pool << sv.substr(i);
        break;
      }
      while (sv[j] != ' ' && sv[j] != '}' && j < len) {
        auto sup_it = latex_subscripts.find(sv[j]);
        if (sup_it != latex_subscripts.end()) {
          pool << sup_it->second;
        } else {
          pool << sv[j];
        }
        ++j;
      }
      i = j;
      continue;
    } else if (sv[i] == '^') {
      if (i + 1 == len) {
        pool << '^';
        break;
      }
      std::string_view::size_type j = i + 1;
      if (j + 1 == len) {
        pool << sv.substr(i);
        break;
      }
      if (sv[j] == '{') {
        ++j;
      }
      if (j == len) {
        pool << sv.substr(i);
        break;
      }
      while (sv[j] != ' ' && sv[j] != '}' && j < len) {
        auto sup_it = latex_superscripts.find(sv[j]);
        if (sup_it != latex_superscripts.end()) {
          pool << sup_it->second;
        } else {
          pool << sv[j];
        }
        ++j;
      }
      i = j;
      continue;
    } else if (sv[i] == '\\') {
      if (i + 1 == len) {
        pool << '\\';
        break;
      }
      if (i + 2 == len) {
        pool << sv.substr(i, 2);
        break;
      }
      auto acc_it = latex_accents.find(sv.substr(i, 2));
      if (acc_it != latex_accents.end()) {
        auto trans_it = acc_it->second.find(sv[i + 2]);
        if (trans_it != acc_it->second.end()) {
          pool << trans_it->second;
          i += 2;
          continue;
        }
      }
      auto esc_it = latex_escaped.find(sv[i + 1]);
      if (esc_it != latex_escaped.end()) {
        pool << esc_it->second;
        i += 1;
        continue;
      }
      std::string_view::size_type j = i + 1;
      while (sv[j] != ' ' && sv[j] != '$' && sv[j] != '}' && sv[j] != '\\' &&
             j < len) {
        ++j;
      }
      if (j == len) {
        pool << sv.substr(i);
        break;
      }
      auto sym_it = latex_symbols.find(sv.substr(i, j - i));
      if (sym_it != latex_symbols.end()) {
        pool << sym_it->second;
        i = j;
        continue;
      }
    } else if (sv[i] == '{' || sv[i] == '}' || sv[i] == '\"' || sv[i] == '~' ||
               sv[i] == '$') {
      continue;
    }
    pool << sv[i];
  }

  return pool.sv();
}

std::string Encoding::HTMLEncode(std::string_view sv) {

  size_t len = sv.length();

  std::string str;

  for (size_t i = 0; i < len; i++) {
    if (sv[i] == '\r') {
      continue;
    } else if (sv[i] == '\n') {
      str += "<br />\n";
    } else if (sv[i] == '<') {
      str += "&lt;";
    } else if (sv[i] == '>') {
      str += "&gt;";
    } else if (sv[i] == '\"') {
      str += "&quot;";
    } else {
      str += sv[i];
    }
  }

  return str;
}

std::string Encoding::HTML2XML(std::string_view sv) {

  std::string html(sv);

  std::string::size_type idx = 0;
  while ((idx = html.find("&", idx)) != std::string::npos) {
    html.replace(idx, 1, "&amp;");
    ++idx;
  }

  static constexpr const char *escape = "\"'<>",
                              *replace[] = {"&quot;", "&apos;", "&lt;", "&gt;"};

  size_t nescape = strlen(escape);

  for (size_t i = 0; i < nescape; i++) {
    idx = 0;
    while ((idx = html.find(escape[i], idx)) != std::string::npos) {
      html.replace(idx, 1, replace[i]);
      ++idx;
    }
  }

  return html;
}
