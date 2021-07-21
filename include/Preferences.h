/**
 *  @file   Preferences.h
 *  @brief  Preferences Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef PREFERENCES_H
#define PREFERENCES_H
#include "Record.h"

#include <cstdio>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

class Preferences {
public:
  Preferences(const std::filesystem::path &f, char *defaults = nullptr);
  ~Preferences();

  bool Save();

  Record preferences;

private:
  std::filesystem::path filename;
  bool state;
  char *data = nullptr;
  off_t size;
};

#endif // end of PREFERENCES_H