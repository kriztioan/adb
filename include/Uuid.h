/**
 *  @file   Uuid.h
 *  @brief  Uuid Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2026-01-24
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include <string_view>

#include <openssl/evp.h>

#define UUID_MS_LENGTH 37
using UUID = struct _UUID {
  uint8_t uuid[16];
  char ms[UUID_MS_LENGTH];
};

namespace Uuid {

bool UUIDFromURL(UUID *uuid, std::string_view url);

}; // namespace Uuid
