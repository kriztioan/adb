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

#define class bt_errclass_
#include <btparse.h>
#undef class

#include <array>
#include <string>
#include <string_view>

class BibTeXParser {
public:
  BibTeXParser(std::string_view input, Pool &pool);
  ~BibTeXParser();

  Record Parse();

  operator bool() const { return status && pos != std::string_view::npos; }

private:
  std::string_view sv;
  std::string_view::size_type pos;
  bt_name_format *fmt;
  Pool &pool;
  int status;
};

namespace BibTeX {
bool Export(Record &record, std::ostream &ostr, std::string_view &key);
}; // namespace BibTeX
