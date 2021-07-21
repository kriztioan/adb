/**
 *  @file   MSWord.h
 *  @brief  MSWord Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef MSWORD_H
#define MSWORD_H

#include "Record.h"

#include <iostream>

#include <uuid/uuid.h>

namespace MSWord {

using Setup = struct _Setup {
  Record &prefs;
  Record &strings;
};

using GUID = struct _GUID {
  union {
    struct {
      uint32_t data1;
      uint16_t data2;
      uint16_t data3;
      uint8_t data4[8];
    };
    uuid_t uuid;
  };
  char ms[37];
};

void Header(std::ostream &ostr);
bool Export(Record &record, std::ostream &ostr, Setup &setup);
void Footer(std::ostream &ostr);

GUID *GUIDCreate(GUID *guid);

}; // namespace MSWord

#endif // end of MSWORD_H