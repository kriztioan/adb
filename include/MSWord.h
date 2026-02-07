/**
 *  @file   MSWord.h
 *  @brief  MSWord Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include "Record.h"
#include "Uuid.h"

#include <algorithm>
#include <iostream>
#include <string>

namespace MSWord {

struct ExportContext {
  Record &strings;
  std::string_view &key;
};

void Header(std::ostream &ostr);
bool Export(Record &record, std::ostream &ostr, ExportContext &ctx);
void Footer(std::ostream &ostr);
}; // namespace MSWord
