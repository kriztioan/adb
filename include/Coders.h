/**
 *  @file   Coders.h
 *  @brief  Coders Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef CODERS_H
#define CODERS_H

#include <array>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <string_view>

#include "Pool.h"

namespace Coders {
char *URLDecodeInplace(char *str);

std::string URLEncode(std::string_view sv);
std::string LaTeXDecode(std::string_view str);
std::string_view LaTeXDecode(std::string_view sv, Pool &pool);
std::string HTMLEncode(std::string_view sv);
std::string HTML2XML(std::string html);

}; // namespace Coders

#endif // end of CODERS_H
