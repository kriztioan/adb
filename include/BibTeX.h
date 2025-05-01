/**
 *  @file   BibTeX.h
 *  @brief  BibTeX Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef BIBTEX_H
#define BIBTEX_H

#include "Coders.h"
#include "Record.h"
#include "Strings.h"
#include <string>

namespace BibTeX {

using Setup = struct _Setup {
  Record &prefs;
  Record &strings;
};

std::string SplitAuthors(std::string_view authors, int max_authors,
                         const std::string &self);
std::string SplitKeywords(const std::string &keywords, const std::string &self);

Record Parse(std::string_view bibtex, size_t &nbytes_parsed);
bool Export(Record &record, std::ostream &ostr, Setup &setup);
}; // namespace BibTeX
#endif // end of BIBTEX_H
