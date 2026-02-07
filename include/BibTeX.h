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

std::string SplitAuthors(std::string_view authors, int max_authors,
                         std::string_view self);
std::string SplitKeywords(std::string_view keywords, std::string_view self);

Record Parse(std::string_view bibtex, size_t &nbytes_parsed, Pool &pool);
bool Export(Record &record, std::ostream &ostr, std::string_view &key);
}; // namespace BibTeX
