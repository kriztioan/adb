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
  char *p = str, *q = p, r[3], *s;
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
      *s = '\0';
      *q++ = static_cast<char>(std::strtol(r, nullptr, 16));
    } else {
      *q++ = *p++;
    }
  }
  *q = '\0';

  return str;
}

std::string Coders::URLEncode(std::string str) {

  size_t len = str.length();
  static constexpr const char reserved[] = "$&+,/:;=?@{}";
  char buff[4];
  for (size_t i = 0; i < len; i++) {
    if (str[i] == ' ') {
      str[i] = '+';
    } else {
      for (size_t j = 0; j < sizeof(reserved); j++) {
        if (str[i] == reserved[j]) {
          snprintf(buff, 4, "%%%2X", (int)reserved[j]);
          str.replace(i, 1, buff);
          i += 2;
          len += 2;
        }
      }
    }
  }
  return (str);
}

std::string Coders::LaTeXDecode(std::string str) {

  static constexpr const char *translate = "&$%\\", *combine = "\"\'`^~",
                              *vowels = "aeiounAEIOUN",
                              *combined[] = {"uml", "acute", "grave", "circ",
                                             "tilde"},
                              *replace[] = {
                                "c", "&ccedil;", "C", "&Cedil;", "o",
                                "&oslash;", "O", "&Oslash;", "ae", "&aelig;",
                                "AA", "&#8491;", "deg", "&deg;",
                                "dagger", "&dagger;"};

      char translated[9],
      *offset;

  int n_char;
  for (std::string::size_type idx = 0; idx < str.length(); idx++) {
    if (str[idx] == '\\') {
      if (strchr(translate, str[idx + 1])) {
        n_char = sprintf(translated, "&#%d;", str[idx + 1]);
        str.replace(idx, 2, translated, n_char);
        idx += (n_char - 2);
      } else if ((offset = const_cast<char *>(strchr(combine, str[idx + 1])))) {
        if (strchr(vowels, str[idx + 2])) {
          n_char = sprintf(translated, "&%c%s;", str[idx + 2],
                           combined[(offset - combine)]);
          str.replace(idx, 3, translated, n_char);
          idx += (n_char - 1);
        }
      } else {
        for (unsigned i = 0; i < sizeof(replace) / sizeof(char *); i += 2) {
          size_t len1 = strlen(replace[i]);
          if (str.substr(idx + 1, len1) == replace[i]) {
            size_t len2 = strlen(replace[i + 1]);
            str.replace(idx, len1 + 1, replace[i + 1], len2);
            idx += (len2 - len1);
            break;
          }
        }
      }
    } else if (str[idx] == '$') {
      if (str[idx + 1] == '\\') {
        str[idx] = '&';
        str.erase(idx + 1, 1);
        while (str[idx++] != '$' && idx < str.length())
          ;
        if (idx < str.length()) {
          str[--idx] = ';';
        }
      }
    } else if (str[idx] == '{' || str[idx] == '}' || str[idx] == '\"' ||
               str[idx] == '~') {
      str.erase(idx--, 1);
    }
  }
  return (str);
}

std::string Coders::HTMLEncode(std::string str) {

  for (std::string::size_type idx = 0; idx < str.length(); idx++) {
    if (str[idx] == '\n') {
      if (str[idx - 1] == '\r') {
        str.erase(--idx, 1);
      }
      str.replace(idx, 1, "<br />\n");
      idx = idx + 6;
    } else if (str[idx] == ' ') {
      if (str[idx + 1] == ' ') {
        str.replace(idx, 1, "&nbsp;");
        idx = idx + 5;
      }
    } else if (str[idx] == '<') {
      str.replace(idx, 1, "&lt;");
      idx = idx + 3;
    } else if (str[idx] == '>') {
      str.replace(idx, 1, "&gt;");
      idx = idx + 3;
    } else if (str[idx] == '\"')
      str.replace(idx, 1, "&quot;");
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
