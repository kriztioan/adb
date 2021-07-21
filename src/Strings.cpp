/**
 *  @file   Strings.cpp
 *  @brief  Strings Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Strings.h"

Strings::Strings(const std::filesystem::path &f) : filename(f) {

  state = false;

  int fd = open(filename.c_str(), O_RDONLY | O_SHLOCK);
  if (fd == -1) {
    return;
  }

  struct stat f_stat;
  if (fstat(fd, &f_stat) == -1 || (f_stat.st_mode & S_IFDIR)) {
    close(fd);
    return;
  }

  size = f_stat.st_size;

  char *data = (char *)mmap(nullptr, f_stat.st_size, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE, fd, 0);

  if (!data) {
    return;
  }

  char *p = data, *q = p + f_stat.st_size, *c, *key;
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
        while (*p) {
          if (*p == '\\')
            p += 2;
          if (*p++ != '\"')
            continue;
          break;
        }
        *p = '\0';

        strings.mFields.emplace(key, Coders::LaTeXDecode(c));

        while (*p != '\n' && *p)
          ++p;
      } else {
        return;
      }
    }
    ++p;
  }

  close(fd);

  state = true;
}

Strings::~Strings() {

  if (data) {
    munmap(data, size);
  }
}

bool Strings::good() { return (state); }
