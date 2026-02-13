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

#include "Pool.h"

namespace HTML {

std::string_view Select(std::string_view options, std::string_view selected,
                        std::string_view name, std::string_view onchange,
                        Pool &pool);

std::string_view Filesize(std::filesystem::path &f, Pool &pool);

std::string_view SplitAuthors(std::string_view authors, std::string_view self,
                              Pool &pool, int max_authors = -1);
std::string_view SplitKeywords(std::string_view keywords, std::string_view self,
                               Pool &pool, std::string_view separator = " ");
}; // namespace HTML
