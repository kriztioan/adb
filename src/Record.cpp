/**
 *  @file   Record.cpp
 *  @brief  Record Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Record.h"

Record::Record(char *d) { Parse(d); };

void Record::Parse(char *data) {

  if (!data || !*data) {
    return;
  }

  char *p = data, *q, *key, *value;
  while (*p) {
    key = p;
    while (*p && *p != '=') {
      ++p;
    }
    if (!*p) {
      break;
    }
    *p = '\0';
    value = ++p;
    while (*p && *p != '&') {
      ++p;
    }
    q = p;
    while (*--q == '+') {
      *q = '\0';
    }

    if (!*p) {
      Coders::URLDecodeInplace(value);
      auto it = mFields.find(key);
      if (it == mFields.end()) {
        mFields.emplace_hint(it, key, value);
      } else {
        it->second += ',';
        it->second += value;
      }
      break;
    }
    *p = '\0';
    Coders::URLDecodeInplace(value);
    auto it = mFields.find(key);
    if (it == mFields.end()) {
      mFields.emplace_hint(it, key, value);
    } else {
      it->second += ',';
      it->second += value;
    }
    ++p;
  }
}

