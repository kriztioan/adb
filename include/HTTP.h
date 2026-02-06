/**
 *  @file   HTTP.h
 *  @brief  HTTP Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef HTTP_H
#define HTTP_H

#include "Encoding.h"
#include "Record.h"

#include <charconv>
#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include <fcntl.h>
#include <sys/select.h>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/opensslconf.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

class HTTP {
public:
  HTTP();

  bool good();

  static void WriteHeader(bool nocache = true);
  static void WriteRedirect(std::string_view url);

  static std::string Get(std::string_view url,
                         std::vector<std::string_view> headers = {},
                         short port = 80, size_t block_size = 4096);
  std::string SecureGet(std::string_view url,
                        std::vector<std::string_view> headers = {},
                        short port = 443, std::string_view pem = {},
                        size_t block_size = 4096);
  std::string SecurePost(std::string_view url, std::string_view post,
                         std::vector<std::string_view> headers = {},
                         short port = 443, std::string_view pem = {},
                         size_t block_size = 4096);

  Record get;
  Record post;
  std::string self;
  std::string uri;
  std::string_view file_data;

private:
  bool state;
  std::string data;
  BIO *bio = nullptr;
  SSL_CTX *ctx = nullptr;
};

int BIO_do_connect_timeout(BIO *bio, struct timeval *timeout);

#endif // end HTTP_H
