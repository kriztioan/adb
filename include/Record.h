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

using Field =
    std::unordered_map<std::string_view, std::string_view>::const_iterator;

class Record {

public:
  Record() = default;
  Record(char *data);

  void Parse(char *data);

  inline auto operator[](std::string_view key) const {
    return (mFields.find(key));
  }

  auto end() const { return (mFields.end()); }

  std::unordered_map<std::string_view, std::string_view> mFields;
};

#endif // end of RECORD_H
