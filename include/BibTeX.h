/**
 *  @file   BibTeX.h
 *  @brief  BibTeX Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include "Encoding.h"
#include "Pool.h"
#include "Record.h"
#include "Strings.h"

#include <string>
#include <string_view>

namespace BibTeX {

std::string_view SplitAuthors(std::string_view authors, std::string_view self,
                              Pool &pool, int max_authors = -1);
std::string_view SplitKeywords(std::string_view keywords, std::string_view self,
                               Pool &pool, std::string_view separator = " ");

Record Parse(std::string_view bibtex, size_t &nbytes_parsed, Pool &pool);
bool Export(Record &record, std::ostream &ostr, std::string_view &key);
}; // namespace BibTeX
