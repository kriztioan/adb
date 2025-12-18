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

#include <algorithm>
#include <iostream>
#include <string>

#include <openssl/evp.h>

namespace MSWord {

using Setup = struct _Setup {
  Record &prefs;
  Record &strings;
};

#define GUID_MS_LENGTH 37
using GUID = struct _GUID {
  uint8_t uuid[16];
  char ms[GUID_MS_LENGTH];
};

void Header(std::ostream &ostr);
bool Export(Record &record, std::ostream &ostr, Setup &setup);
void Footer(std::ostream &ostr);

GUID *GUIDCreate(GUID *guid, const std::string &value);

}; // namespace MSWord

#endif // end of MSWORD_H
