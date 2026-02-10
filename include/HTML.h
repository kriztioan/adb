/**
 *  @file   HTML.h
 *  @brief  HTML Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string_view>

namespace HTML {

std::string Select(std::string_view options, std::string_view selected,
                   std::string_view name, std::string_view onchange);

std::string Filesize(std::filesystem::path &f);

}; // namespace HTML
