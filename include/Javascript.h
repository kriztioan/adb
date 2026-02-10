/**
 *  @file   Javascript.h
 *  @brief  Javascript Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once
#include "Pool.h"

#include <string_view>
#include <vector>

namespace Javascript {

std::string_view Array(std::vector<std::string_view> &v, Pool &pool);
}; // namespace Javascript
