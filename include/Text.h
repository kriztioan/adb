/**
 *  @file   Text.h
 *  @brief  Text Class Definition
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

#include <iostream>

namespace Text {

struct ExportContext {
  Record &strings;
  Pool &pool;
};

bool Export(Record &record, std::ostream &ostr, ExportContext &ctx);
}; // namespace Text
