/**
 *  @file   HTML.h
 *  @brief  HTML Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef HTML_H
#define HTML_H

#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

namespace HTML {

std::string Select(std::string_view options, const std::string &selected,
                   const std::string &name, const std::string &onchange);

std::string Highlight(std::string_view body, std::string_view match);

std::string Filesize(std::string_view filename);

}; // namespace HTML

#endif // end of HTML_H
