/**
 *  @file   Preferences.h
 *  @brief  Preferences Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include "Record.h"

#include <cstdio>
#include <fcntl.h>
#include <filesystem>
#include <string_view>
#include <sys/mman.h>
#include <unistd.h>

class Preferences {
public:
  Preferences(const std::filesystem::path &f, std::string_view defaults = {});
  ~Preferences();

  bool Save();

  inline auto operator[](std::string_view key) const {
    return preferences[key];
  }

  auto end() const { return preferences.end(); }

  Record preferences;

private:
  std::filesystem::path filename;
  off_t size;
  bool state;
  char *data = nullptr;
};
