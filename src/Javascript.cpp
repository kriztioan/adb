/**
 *  @file   Javascript.cpp
 *  @brief  Javascript Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Javascript.h"

std::string Javascript::Array(std::vector<std::string> &v) {

  std::string str("Array(");
  if (v.size()) {
    auto it = v.begin();
    str += "'" + *it++ + "'";
    for (; it != v.end(); it++) {
      str += ",'" + *it + "'";
    }
  }
  str += ')';

  return (str);
}
