#include "Uuid.h"

bool Uuid::UUIDFromURL(UUID *uuid, std::string_view url) {

  EVP_MD_CTX *ctx = EVP_MD_CTX_new();

  if (ctx == NULL) {

    return false;
  }

  const EVP_MD *md = EVP_md5();

  EVP_DigestInit(ctx, md);

  uint8_t ns[] = {0x6b, 0xa7, 0xb8, 0x11, 0x9d, 0xad, 0x11, 0xd1,
                  0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8};

  EVP_DigestUpdate(ctx, ns, sizeof(ns));

  EVP_DigestUpdate(ctx, url.data(), url.length());

  uint32_t md_len;

  EVP_DigestFinal(ctx, uuid->uuid, &md_len);

  EVP_MD_CTX_free(ctx);

  // version 3
  uuid->uuid[6] = (uuid->uuid[6] & 0x0F) | 0x30;

  // variant URL
  uuid->uuid[8] = (uuid->uuid[8] & 0x3F) | 0x80;

  snprintf(uuid->ms, UUID_MS_LENGTH,
           "%02X%02X%02X%02X-"
           "%02X%02X-"
           "%02X%02X-"
           "%02X%02X-"
           "%02X%02X%02X%02X%02X%02X",
           uuid->uuid[0], uuid->uuid[1], uuid->uuid[2], uuid->uuid[3],
           uuid->uuid[4], uuid->uuid[5], uuid->uuid[6], uuid->uuid[7],
           uuid->uuid[8], uuid->uuid[9], uuid->uuid[10], uuid->uuid[11],
           uuid->uuid[12], uuid->uuid[13], uuid->uuid[14], uuid->uuid[15]);

  return true;
}
