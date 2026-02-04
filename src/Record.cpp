/**
 *  @file   Record.cpp
 *  @brief  Record Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Record.h"

Record::Record(char *data) { Parse(data); };

void Record::Parse(char *data) {

  if (!data || !*data) {
    return;
  }

  char *p = data, *q, *k, *v, *kk;
  while (*p) {
    while (*p && *p == '+') {
      ++p;
    }
    if (!*p) {
      return;
    }
    k = p;
    while (*p && *p != '=') {
      ++p;
    }
    if (!*p) {
      return;
    }
    *p = '\0';
    q = p++;
    while (*--q == '+') {
      *q = '\0';
    }
    kk = q + 1;

    while (*p && *p == '+') {
      ++p;
    }
    v = p;
    while (*p && *p != '&') {
      ++p;
    }
    if (*p) {
      *p = '\0';
      q = p++;
    } else {
      q = p;
    }
    while (*--q == '+') {
      *q = '\0';
    }
    if (*v) {
      v = Coders::URLDecodeInplace(v);
      auto field_it = mFields.find(k);
      if (field_it == mFields.end()) {
        mFields.emplace_hint(field_it, std::piecewise_construct,
                             std::forward_as_tuple(k, kk - k),
                             std::forward_as_tuple(v));
      } else {
        size_t len_q = field_it->second.size();
        q = const_cast<char *>(field_it->second.data()) + len_q;
        *q++ = ',';
        size_t len_v = strlen(v) + 1;
        std::memmove(q, v, len_v);
        field_it->second =
            std::string_view(field_it->second.data(), len_q + len_v);
      }
    }
  }
}
