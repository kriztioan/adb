/**
 *  @file   Strings.cpp
 *  @brief  Strings Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Strings.h"

BibTeX::Strings::Strings(const std::filesystem::path &f, Pool &pool)
    : filename(f), pool(pool) {
  Parse(f);
}

BibTeX::Strings::Strings(Pool &pool) : pool(pool) {}

int BibTeX::Strings::Parse(const std::filesystem::path &f) {

  filename = f;

  state = false;

  if (!std::filesystem::is_regular_file(f)) {
    return state;
  }

  size_t page_size = getpagesize();

  size = (std::filesystem::file_size(f) + page_size - 1) & ~(page_size - 1);

  int fd = open(filename.c_str(), O_RDONLY | O_SHLOCK);
  if (fd == -1) {
    return state;
  }

  char *data =
      (char *)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (data == MAP_FAILED) {
    close(fd);
    return state;
  }

  char *p = data, *q = p + size, *c, *key;
  while (p < q) {
    while (isspace(*p))
      ++p;
    if (*p == '%')
      while (*p != '\n')
        ++p;
    else if (*p == '@') {
      c = ++p;
      while (*c != '{')
        ++c;
      *c = '\0';
      if (strcmp(p, "STRING") == 0) {
        p = ++c;
        key = p;
        while (*p != ' ' && *p != '=')
          ++p;
        *p = '\0';
        while (*(++p) != '\"')
          ;
        *p = '\0';
        c = ++p;
        while (*p && *p != '\"') {
          if (*p == '\\')
            p += 2;
          else
            ++p;
        }
        *p = '\0';

        strings.mFields.emplace(key, Encoding::LaTeXDecode(c, pool));

        while (*p != '\n' && *p)
          ++p;
      } else {
        return state;
      }
    }
    ++p;
  }
  close(fd);

  state = true;

  return state;
}

BibTeX::Strings::~Strings() {

  if (data) {
    munmap(data, size);
  }
}

bool BibTeX::Strings::good() { return state; }
