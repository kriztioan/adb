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
#include "Record.h"

#include <iostream>

namespace Text {

bool Export(Record &record, std::ostream &ostr, Record &strings);
}; // namespace Text
