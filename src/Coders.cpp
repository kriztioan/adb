/**
 *  @file   Coders.cpp
 *  @brief  Coders Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Coders.h"

char *Coders::URLDecodeInplace(char *str) {

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
      *q++ = static_cast<char>(std::strtol(r, nullptr, 16));
    } else {
      *q++ = *p++;
    }
  }
  *q = '\0';

  return str;
}

std::string Coders::URLEncode(std::string_view sv) {

  static constexpr const char reserved[] = "$&+,/:;=?@{}";

  size_t len = sv.length();

  std::string str;

  str.reserve(len);

  char buff[4];

  size_t j;
  for (size_t i = 0; i < len; i++) {
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
  return (str);
}

std::string Coders::LaTeXDecode(std::string_view sv) {

  static constexpr const char *translate = "&$%\\", *combine = "\"\'`^~",
                              *vowels = "aeiounAEIOUN",
                              *combined[] = {"uml", "acute", "grave", "circ",
                                             "tilde"},
                              *replace[] = {
                                  "c",   "&ccedil;", "C",      "&Cedil;",
                                  "o",   "&oslash;", "O",      "&Oslash;",
                                  "ae",  "&aelig;",  "AA",     "&#8491;",
                                  "deg", "&deg;",    "dagger", "&dagger;",
                                  "gt",  "&gt;",     "lt",     "&lt;"};

  char translated[9], *offset;

  size_t len = sv.length();

  std::ostringstream ostr;

  for (std::string::size_type i = 0; i < len; i++) {
    if (sv[i] == '\\') {
      if (strchr(translate, sv[i + 1])) {
        snprintf(translated, sizeof(translated), "&#%d;", sv[++i]);
        ostr << translated;
        continue;
      } else if ((offset = const_cast<char *>(strchr(combine, sv[i + 1])))) {
        if (strchr(vowels, sv[i + 2])) {
          snprintf(translated, sizeof(translated), "&%c%s;", sv[i + 2],
                   combined[(offset - combine)]);
          ostr << translated;
          i += 2;
          continue;
        }
      } else {
        for (unsigned j = 0; j < sizeof(replace) / sizeof(char *); j += 2) {
          size_t len1 = strlen(replace[j]);
          if (sv.substr(i + 1, len1) == replace[j]) {
            ostr << replace[j + 1];
            i += len1;
            break;
          }
        }
        continue;
      }
    } else if (sv[i] == '$') {
      if (sv[i + 1] == '\\') {
        ostr << "&";
        ++i;
        while (sv[++i] != '$' && i < len) {
          ostr << sv[i];
        }
        ostr << ";";
        continue;
      }
    } else if (sv[i] == '{' || sv[i] == '}' || sv[i] == '\"' || sv[i] == '~') {
      continue;
    }
    ostr << sv[i];
  }
  return (ostr.str());
}

std::string_view Coders::LaTeXDecode(std::string_view sv, Pool &pool) {

  static constexpr const char
      *translate = "&$%\\",
      *combine = "\"\'`^~", *vowels = "aeiounAEIOUN",
      *combined[] = {"uml", "acute", "grave", "circ", "tilde"},
      *replace[] = {"c",   "&ccedil;", "C",      "&Cedil;",  "o",  "&oslash;",
                    "O",   "&Oslash;", "ae",     "&aelig;",  "AA", "&#8491;",
                    "deg", "&deg;",    "dagger", "&dagger;", "gt", "&gt;",
                    "lt",  "&lt;"}; //"i", "&imath;"};

  char *offset;

  size_t len = sv.length(), replace_len;

  pool.begin();
  for (std::string::size_type i = 0; i < len; i++) {
    if (sv[i] == '\\') {
      if (strchr(translate, sv[i + 1])) {
        pool << "&#" << static_cast<int>(sv[++i]) << ";";
        continue;
      } else if ((offset = const_cast<char *>(strchr(combine, sv[i + 1])))) {
        if (strchr(vowels, sv[i + 2])) {
          pool << '&' << sv[i + 2] << combined[(offset - combine)] << ';';
          i += 2;
          continue;
        }
      } else {
        for (unsigned j = 0; j < sizeof(replace) / sizeof(char *); j += 2) {
          replace_len = strlen(replace[j]);
          if (sv.substr(i + 1, replace_len) == replace[j]) {
            pool << replace[j + 1];
            i += replace_len;
            break;
          }
        }
        continue;
      }
    } else if (sv[i] == '$') {
      if (sv[i + 1] == '\\') {
        pool << "&";
        ++i;
        while (sv[++i] != '$' && i < len) {
          pool << sv[i];
        }
        pool << ";";
        continue;
      }
    } else if (sv[i] == '{' || sv[i] == '}' || sv[i] == '\"' || sv[i] == '~') {
      continue;
    }
    pool << sv[i];
  }

  return (pool.sv());
}

std::string Coders::HTMLEncode(std::string_view sv) {

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

  return (str);
}

std::string Coders::HTML2XML(std::string html) {

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

  static constexpr const char *vowels = "aeiounAEIOUN",
                              *accents[] = {"uml", "acute", "grave", "circ",
                                            "tilde"};

  size_t nvowels = strlen(vowels),
         naccents = sizeof(accents) / sizeof(const char *);

  //  "    '    `    o    ~
  static constexpr int codes[12][5] = {{226, 225, 224, 229, 227},  // a
                                       {234, 233, 232, 234, 160},  // e
                                       {239, 237, 236, 238, 160},  // i
                                       {246, 243, 242, 244, 245},  // o
                                       {252, 250, 249, 251, 160},  // u
                                       {160, 160, 160, 160, 241},  // n
                                       {194, 193, 192, 143, 197},  // A
                                       {203, 201, 200, 202, 160},  // E
                                       {207, 205, 204, 206, 160},  // I
                                       {214, 211, 210, 212, 213},  // O
                                       {220, 218, 217, 219, 160},  // U
                                       {160, 160, 160, 160, 209}}; // N

  for (size_t i = 0; i < nvowels; i++) {
    for (size_t j = 0; j < naccents; j++) {
      idx = 0;
      while (
          (idx = html.find(std::string("&amp;") + vowels[i] + accents[j] + ";",
                           idx)) != std::string::npos) {
        html.replace(idx + 1, strlen(accents[j]) + 5,
                     '#' + std::to_string(codes[i][j]));
        ++idx;
      }
    }
  }

  std::array<std::string, 24> greek = {
      "Alpha", "Beta",    "Gamma",    "Delta", "Epsilon", "Zeta",
      "Eta",   "Theta",   "Iota",     "Kappa", "Lambda",  "Mu",
      "Nu",    "Xi",      "Omnicron", "Pi",    "Rho",     "Sigma",
      "Tau",   "Upsilon", "Phi",      "Chi",   "Psi",     "Omega"};

  size_t ngreek = greek.size();
  for (size_t i = 0; i < ngreek; i++) {
    while ((idx = html.find(std::string("&amp;") + greek[i] + ';', idx)) !=
           std::string::npos) {
      html.replace(idx + 1, greek[i].length() + 4,
                   "#" + std::to_string(913 + i));
    }
    greek[i][0] = (char)tolower(greek[i][0]);
    idx = 0;
    while ((idx = html.find(std::string("&amp;") + greek[i] + ';', idx)) !=
           std::string::npos) {
      html.replace(idx + 1, greek[i].length() + 4,
                   "#" + std::to_string(913 + i + 32));
    }
  }

  return (html);
}
