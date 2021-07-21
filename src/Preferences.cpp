/**
 *  @file   Preferences.cpp
 *  @brief  Preferences Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Preferences.h"

Preferences::Preferences(const std::filesystem::path &f, char *defaults)
    : filename(f) {

  state = false;

  char *p;

  struct stat f_stat;

  int fd = open(filename.c_str(), O_RDONLY | O_SHLOCK);

  if (fd == -1) {
    if (!defaults) {
      return;
    }
    p = defaults;
  } else {
    if (fstat(fd, &f_stat) == -1 || (f_stat.st_mode & S_IFDIR)) {
      close(fd);
      return;
    }

    size = f_stat.st_size;

    data = p = (char *)mmap(nullptr, size, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE, fd, 0);
  }

  char *key, *value;
  while (*p) {
    if (*p == '#')
      while (*(++p) != '\n' && *p)
        ;
    else if (*p == '$') {
      key = ++p;
      while (*p != ' ' && *p != '\t' && *p != '=' && *p)
        ++p;
      if (!*p)
        break;
      *p = '\0';
      while ((*(++p) == ' ' || *p == '\t' || *p == '=') && *p)
        ;
      if (!*p)
        break;
      value = p;
      if (*p == '\"') {
        value = ++p;
        while (*p && *p != '\"')
          ++p;
        if (!*p)
          break;
        *(p++) = '\0';
      } else {
        while (*p != ' ' && *p == '\t' && *p == '\n' && *p)
          ++p;
        if (!*p)
          break;
        *p = '\0';
      }

      preferences.mFields.emplace(key, value);
    }
    ++p;
  }

  close(fd);

  state = true;
}

Preferences::~Preferences() {

  if (data) {
    munmap(data, size);
  }
}

bool Preferences::Save() {

  int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_EXLOCK | O_TRUNC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  if (fd == -1) {
    return (false);
  }

  dprintf(fd, "#\n"
              "# Automatic generated configuration file for AdB\n"
              "#\n"
              "# valid variables are $save, $base $data, $bibtex $msword "
              "$abbreviaton $plugins $baseurl, $administrator, $splash, "
              "$scheme, $username, $password and $expire\n"
              "#\n\n");

  for (auto &f : preferences.mFields) {
    dprintf(fd, "$%s = \"%s\"\n", f.first.c_str(), f.second.c_str());
  }

  close(fd);

  return (true);
}