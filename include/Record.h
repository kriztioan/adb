/**
 *  @file   Record.h
 *  @brief  Record Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef RECORD_H
#define RECORD_H

#include "Coders.h"
#include <string>
#include <string_view>
#include <unordered_map>

class Record {

public:
  Record() = default;
  Record(char *d);

  void Parse(char *data);
  std::unordered_map<std::string, std::string> mFields;

private:
};

#endif // end of RECORD_H
