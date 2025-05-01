/**
 *  @file   Text.h
 *  @brief  Text Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef TEXT_H
#define TEXT_H

#include "Coders.h"
#include "Record.h"

#include <iostream>

namespace Text {

using Setup = struct _Setup {
  Record &prefs;
  Record &strings;
};

bool Export(Record &record, std::ostream &ostr, Setup &setup);
}; // namespace Text

#endif // end of TEXT_H
