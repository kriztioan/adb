/**
 *  @file   Javascript.cpp
 *  @brief  Javascript Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Javascript.h"

std::string_view Javascript::Array(std::vector<std::string_view> &v,
                                   Pool &pool) {
  pool.begin();
  pool << "Array(";
  if (v.size()) {
    auto it = v.begin();
    pool << "'" << *it++ << "'";
    for (; it != v.end(); it++) {
      pool << ",'" << *it << "'";
    }
  }
  pool << ')';

  return pool.sv();
}
