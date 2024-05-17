/**
 *  @file   Strings.h
 *  @brief  Strings Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef STRINGS_H
#define STRINGS_H

#include "Record.h"
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

class Strings {

public:
  Strings(const std::filesystem::path &f);
  ~Strings();

  bool good();

  Record strings;

private:
  std::filesystem::path filename;
  bool state;
  char *data = nullptr;
  off_t size;
};

#endif // end of STRINGS_H
