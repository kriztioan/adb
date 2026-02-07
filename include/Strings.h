/**
 *  @file   Strings.h
 *  @brief  Strings Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include "Pool.h"
#include "Record.h"

#include <fcntl.h>
#include <filesystem>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

namespace BibTeX {

class Strings {

public:
  Strings() = delete;
  Strings(Pool &pool);
  Strings(const std::filesystem::path &f, Pool &pool);
  ~Strings();

  int Parse(const std::filesystem::path &f);

  bool good();

  Record strings;

private:
  std::filesystem::path filename;
  Pool &pool;
  off_t size;
  bool state;
  char *data = nullptr;
};

} // namespace BibTeX
