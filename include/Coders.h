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
#include <string>

namespace Coders {
char *URLDecodeInplace(char *str);

std::string URLEncode(std::string str);
std::string LaTeXDecode(std::string str);
std::string HTMLEncode(std::string str);
std::string HTML2XML(std::string html);

}; // namespace Coders

#endif // end of CODERS_H
