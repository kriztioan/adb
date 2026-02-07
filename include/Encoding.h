/**
 *  @file   Encoding.h
 *  @brief  Encoding Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include <array>
#include <charconv>
#include <cstring>
#include <sstream>
#include <string>
#include <string_view>

#include "Pool.h"

namespace Encoding {
char *URLDecodeInplace(char *str);

std::string URLEncode(std::string_view sv);
std::string LaTeXDecode(std::string_view str);
std::string_view LaTeXDecode(std::string_view sv, Pool &pool);
std::string HTMLEncode(std::string_view sv);
std::string HTML2XML(std::string html);

}; // namespace Encoding
