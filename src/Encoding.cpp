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

  std::stringstream sstr;
  std::string buf;
  buf.reserve(sv.length());
  sstr.str(buf);

  std::string_view::size_type j;
  for (const auto c : sv) {
    if (c == ' ') {
      sstr << '+';
    } else if ((j = url_reserved.find(c)) != std::string_view::npos) {
      sstr << '%' << std::hex << std::setw(2) << std::setfill('0')
           << (int)url_reserved[j];
    } else {
      sstr << c;
    }
  }

  return sstr.str();
}

std::string_view Encoding::LaTeXDecode(std::string_view sv, Pool &pool) {

  std::string_view utf8_nfc;

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
      auto cmb_it = latex_accents.find(sv[i + 1]);
      if (cmb_it != latex_accents.end()) {
        utf8_nfc = cmb_it->second;
        i += 1;
        continue;
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
        if (!utf8_nfc.empty()) {
          pool << utf8_nfc;
          utf8_nfc = std::string_view();
        }
        i = j;
        continue;
      }
    } else if (sv[i] == '{' || sv[i] == '}' || sv[i] == '\"' || sv[i] == '~' ||
               sv[i] == '$') {
      continue;
    }
    pool << sv[i];
    if (!utf8_nfc.empty()) {
      pool << utf8_nfc;
      utf8_nfc = std::string_view();
    }
  }

  return pool.sv();
}

std::string Encoding::HTMLEncode(std::string_view sv) {

  std::stringstream sstr;
  std::string buf;
  buf.reserve(sv.length());

  for (const auto c : sv) {
    auto esc_it = xml_escape.find(c);
    if (esc_it != xml_escape.end()) {
      sstr << esc_it->second;
    } else if (c == '\n') {
      sstr << "<br />";
    } else if (c != '\r') {
      sstr << c;
    }
  }

  return sstr.str();
}

std::string Encoding::XMLEncode(std::string_view sv) {

  std::stringstream sstr;
  std::string buf;
  buf.reserve(sv.length());

  for (const auto c : sv) {
    auto esc_it = xml_escape.find(c);
    if (esc_it != xml_escape.end()) {
      sstr << esc_it->second;
    } else if (c == '&') {
      sstr << "&amp;";
    } else if (c != '\r') {
      sstr << c;
    }
  }

  return sstr.str();
}
