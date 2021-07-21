/**
 *  @file   HTTP.cpp
 *  @brief  HTTP Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include <HTTP.h>

HTTP::HTTP() {

  if (getenv("SCRIPT_NAME")) {
    self = getenv("SCRIPT_NAME");
  }

  if (getenv("REQUEST_URI") && getenv("QUERY_STRING")) {
    uri = Coders::URLDecodeInplace(getenv("REQUEST_URI"));
    if (!uri.empty() && uri.find('?') == std::string::npos) {
      uri += "?";
      uri += getenv("QUERY_STRING");
    }
  }

  if (getenv("QUERY_STRING")) {
    get.Parse(getenv("QUERY_STRING"));
  }

  if (getenv("CONTENT_TYPE")) {
    std::string content_type(getenv("CONTENT_TYPE"));
    if (getenv("CONTENT_LENGTH")) {
      char *endptr;
      size_t content_length = strtol(getenv("CONTENT_LENGTH"), &endptr, 10);
      if ('\0' == *endptr) {
        if (content_length > 0) {
          data.assign(std::istreambuf_iterator<char>(std::cin),
                      std::istreambuf_iterator<char>());
          std::string d;
          std::string::size_type start = 0, end = std::string::npos;
          if ((end = content_type.find("multipart/form-data", start)) !=
              std::string::npos) {
            if ((start = content_type.find("; boundary=", end)) !=
                std::string::npos) {
              std::string boundary(content_type.substr(start + 11));
              while ((start = data.find(boundary, end)) != std::string::npos) {
                start += boundary.length();
                end = data.find(boundary, start);
                d = data.substr(start, end - start - 3);
                if (d == "--\r\n")
                  break;
                std::string::size_type s = d.find("Content-Disposition: "),
                                       e = d.find(";", s);
                if (s != std::string::npos && e != std::string::npos) {
                  std::string disposition(d.substr(s + 21, e - s - 21));
                  if (disposition == "form-data") {
                    if ((s = d.find("name=\"", e)) != std::string::npos &&
                        (e = d.find("\"", s + 6)) != std::string::npos) {
                      if (d.find("filename=\"", e) != std::string::npos) {
                        s = d.find("\r\n\r\n", e);
                        file_data = d.substr(s + 4, d.length() - s - 5);
                        continue;
                      }
                      std::string name(d.substr(s + 6, e - s - 6));
                      if ((s = d.find("\r\n\r\n", e)) != std::string::npos) {
                        post.mFields.emplace(
                            name, d.substr(s + 4, d.length() - s - 5));
                      }
                    } else { // name
                      state = false;
                      return;
                    }
                  } else { // form-data
                    state = false;
                    return;
                  }
                } else // disposition
                  state = false;
                start = end;
              }
            } else { // boundary
              state = false;
              return;
            }
          } else // multipart form-data
            post.Parse(data.data());
        } else { // read
          state = false;
          return;
        }
      } else { // strtol
        state = false;
        return;
      }
    } else { // content-length
      state = false;
      return;
    }
  }
  state = true;
}

bool HTTP::good() { return state; }

void HTTP::WriteHeader(bool nocache) {
  std::cout << "Content-type: text/html; charset=utf-8\n";
  if (nocache) {
    std::cout << "Cache-Control: no-cache, no-store, must-revalidate\n"
              << "Pragma: no-cache\n"
              << "Expires: 0\n"
              << "Connection: close\n";
  }
  std::cout << "\n";
}

void HTTP::WriteRedirect(const char *url) {
  std::cout << "Location: " << url << "\n"
            << "Connection: close\n\n";
}

std::string HTTP::Get(std::string_view url, std::vector<std::string> headers,
                      short port, size_t block_size) {

  if (url.empty()) {
    return (std::string());
  }

  struct protoent *protocol = getprotobyname("tcp");
  if (!protocol) {
    return (std::string());
  }
  int sockfd = socket(AF_INET, SOCK_STREAM, protocol->p_proto);
  if (sockfd == -1) {
    return (std::string());
  }

  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    return (std::string());
  }

  int opt;
  if ((opt = fcntl(sockfd, F_GETFL, NULL)) < 0)
    return (NULL);

  if (fcntl(sockfd, F_SETFL, opt | O_NONBLOCK) < 0)
    return (NULL);

  struct sockaddr_in local, remote;

  local.sin_family = AF_INET;
  local.sin_port = htons(0);
  local.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&local.sin_zero, 8);
  if (bind(sockfd, (struct sockaddr *)&local, sizeof(struct sockaddr)) == -1) {
    return (std::string());
  }

  remote.sin_family = AF_INET;
  remote.sin_port = htons(port);

  std::string_view path(url.begin() + url.find_first_of('/'),
                        url.size() - url.find_first_of('/'));
  std::string hostname(url.begin(), url.find_first_of('/'));

  struct hostent *remote_host = gethostbyname(hostname.data());
  if (!remote_host) {
    return (std::string());
  }

  bzero(&remote.sin_addr, sizeof(struct in_addr));
  memcpy(&remote.sin_addr, remote_host->h_addr, sizeof(struct in_addr));
  bzero(&remote.sin_zero, 8);

  if (connect(sockfd, (struct sockaddr *)&remote, sizeof(struct sockaddr)) ==
      -1) {
    if (errno == EINPROGRESS) {
      struct timeval timeout = {.tv_sec = 3,
                                .tv_usec = 0}; // TODO: Make user configurable
      fd_set fdset;
      FD_ZERO(&fdset);
      FD_SET(sockfd, &fdset);
      if (select(sockfd + 1, NULL, &fdset, NULL, &timeout) <= 0) {
        if (fcntl(sockfd, F_SETFL, opt) < 0)
          return (NULL);
      }
    }
  }

  if (fcntl(sockfd, F_SETFL, opt) < 0)
    return (NULL);

  std::stringstream ss;

  ss << "GET ";
  if (!path.empty()) {
    ss << path;
  } else {
    ss << '/';
  }
  ss << " HTTP/1.1\r\nHost: " << hostname << "\r\n";
  if (headers.size()) {
    for (const auto &h : headers) {
      ss << h << "\r\n";
    }
  }
  ss << "User-Agent: HTTP-Client/1.0\r\nConnection: close\r\n\r\n";

  std::string request(ss.str());

  const char *bytes = request.c_str();

  int bytes_to_send = request.size(), bytes_send, total_bytes_send = 0;
  while (total_bytes_send < bytes_to_send) {
    bytes_send = send(sockfd, bytes + total_bytes_send,
                      bytes_to_send - total_bytes_send, 0);
    if (bytes_send == -1) {
      return (std::string());
    }
    total_bytes_send += bytes_send;
  }

  std::string response;

  char buff[block_size];

  int bytes_recv;
  while ((bytes_recv = recv(sockfd, buff, block_size - 1, 0))) {
    if (bytes_recv == -1) {
      return (std::string());
    }
    buff[bytes_recv] = '\0';
    response.append(buff);
  }
  return (response.substr(response.find("\r\n\r\n") + 4));
}

std::string HTTP::SecureGet(std::string_view url,
                            std::vector<std::string> headers, short port,
                            const char *pem, size_t block_size) {

  if (url.empty()) {
    return (std::string());
  }

  std::string_view path(url.begin() + url.find_first_of('/'),
                        url.size() - url.find_first_of('/'));
  std::string hostname(url.begin(), url.find_first_of('/'));

  SSL_library_init();

  struct timeval timeout = {.tv_sec = 3,
                            .tv_usec = 0}; // TODO: Make user configurable

  std::stringstream ss;
  std::string request, response;

  char buff[block_size];

  if (!ctx) {
    ctx = SSL_CTX_new(SSLv23_method());
    if (!ctx) {
      return (std::string());
    }

    if (SSL_CTX_load_verify_locations(ctx, pem, nullptr) != 1) {
      goto fail;
    }

    bio = BIO_new_ssl_connect(ctx);
    if (!bio) {
      goto fail;
    }

    SSL *ssl = nullptr;
    BIO_get_ssl(bio, &ssl);
    if (!ssl) {
      goto fail;
    }
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    struct hostent *remote_host = gethostbyname(hostname.c_str());
    if (!remote_host) {
      goto fail;
    }

    BIO_ADDR *bio_addr = BIO_ADDR_new();
    if (!bio_addr) {
      BIO_ADDR_free(bio_addr);
      goto fail;
    }

    if (BIO_ADDR_rawmake(bio_addr, AF_INET, remote_host->h_addr,
                         remote_host->h_length, htons(port)) != 1) {
      BIO_ADDR_free(bio_addr);
      goto fail;
    }

    if (BIO_set_conn_address(bio, bio_addr) != 1) {
      BIO_ADDR_free(bio_addr);
      goto fail;
    }

    BIO_ADDR_free(bio_addr);

    if(BIO_do_connect_timeout(bio, &timeout) != 1) {
      goto fail;
    }

    if (SSL_get_verify_result(ssl) != X509_V_OK) {
      goto fail;
    }
  } else if (BIO_do_connect_timeout(bio, &timeout) != 1) {
    goto fail;
  }

  ss << "GET ";
  if (!path.empty()) {
    ss << path;
  } else {
    ss << '/';
  }
  ss << " HTTP/1.1\r\nHost: " << hostname << "\r\n";
  if (headers.size()) {
    for (const auto &h : headers) {
      ss << h << "\r\n";
    }
  }
  ss << "User-Agent: HTTP-Client/1.0\r\n"
     << "Connection: close\r\n\r\n";

  request = ss.str();

  BIO_puts(bio, request.c_str());

  int bytes_recv;
  while ((bytes_recv = BIO_read(bio, buff, block_size - 1))) {
    if (bytes_recv == -1) {
      return (std::string());
    }
    buff[bytes_recv] = '\0';
    response.append(buff);
  }

  if (BIO_reset(bio) != 0) {
    return (std::string());
  }

  return (response.substr(response.find("\r\n\r\n") + 4));

fail:
  BIO_free_all(bio);
  bio = nullptr;
  SSL_CTX_free(ctx);
  ctx = nullptr;
  return (std::string());
}

std::string HTTP::SecurePost(std::string_view url, std::string_view post,
                             std::vector<std::string> headers, short port,
                             const char *pem, size_t block_size) {

  if (url.empty()) {
    return (std::string());
  }

  std::string_view path(url.begin() + url.find_first_of('/'),
                        url.size() - url.find_first_of('/'));
  std::string hostname(url.begin(), url.find_first_of('/'));

  SSL_library_init();

  struct timeval timeout = {.tv_sec = 3,
                            .tv_usec = 0}; // TODO: Make user configurable

  std::stringstream ss;
  std::string request, response;

  char buff[block_size];

  if (!ctx) {
    ctx = SSL_CTX_new(SSLv23_method());
    if (!ctx) {
      return (std::string());
    }

    if (SSL_CTX_load_verify_locations(ctx, pem, nullptr) != 1) {
      goto fail;
    }

    bio = BIO_new_ssl_connect(ctx);
    if (!bio) {
      goto fail;
    }

    SSL *ssl = nullptr;
    BIO_get_ssl(bio, &ssl);
    if (!ssl) {
      goto fail;
    }
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    struct hostent *remote_host = gethostbyname(hostname.c_str());
    if (!remote_host) {
      goto fail;
    }

    BIO_ADDR *bio_addr = BIO_ADDR_new();
    if (!bio_addr) {
      BIO_ADDR_free(bio_addr);
      goto fail;
    }

    if (BIO_ADDR_rawmake(bio_addr, AF_INET, remote_host->h_addr,
                         remote_host->h_length, htons(port)) != 1) {
      BIO_ADDR_free(bio_addr);
      goto fail;
    }

    if (BIO_set_conn_address(bio, bio_addr) != 1) {
      BIO_ADDR_free(bio_addr);
      goto fail;
    }

    BIO_ADDR_free(bio_addr);

    if (BIO_do_connect_timeout(bio, &timeout) != 1) {
      goto fail;
    }

    if (SSL_get_verify_result(ssl) != X509_V_OK) {
      goto fail;
    }
  } else if (BIO_do_connect_timeout(bio, &timeout) != 1) {
    goto fail;
  }

  ss << "POST ";
  if (!path.empty()) {
    ss << path;
  } else {
    ss << '/';
  }
  ss << " HTTP/1.1\r\nHost: " << hostname << "\r\n";
  if (headers.size()) {
    for (const auto &h : headers) {
      ss << h << "\r\n";
    }
  }
  ss << "Content-Length: " << post.length()
     << "\r\nUser-Agent: HTTP-Client/1.0\r\n"
     << "Connection: close\r\n\r\n"
     << post;

  request = ss.str();

  BIO_puts(bio, request.c_str());

  int bytes_recv;

  while ((bytes_recv = BIO_read(bio, buff, block_size - 1))) {
    if (bytes_recv == -1) {
      return (std::string());
    }
    buff[bytes_recv] = '\0';
    response.append(buff);
  }

  if (BIO_reset(bio) != 0) {
    return (std::string());
  }

  return (response.substr(response.find("\r\n\r\n") + 4));

fail:
  BIO_free_all(bio);
  bio = nullptr;
  SSL_CTX_free(ctx);
  ctx = nullptr;
  return (std::string());
}

int BIO_do_connect_timeout(BIO *bio, struct timeval *timeout) {

  while (true) {
    if (BIO_do_connect(bio) == 1) {
      return (1);
    }

    if (!BIO_should_retry(bio)) {
      return (-1);
    }

    int fd = BIO_get_fd(bio, NULL);
    if (fd == -1) {
      return (-1);
    }

    fd_set rdfds, wrfds;
    FD_ZERO(&rdfds);
    FD_ZERO(&wrfds);
    if (BIO_should_read(bio)) {
      FD_SET(fd, &rdfds);
    } else if (BIO_should_write(bio)) {
      FD_SET(fd, &wrfds);
    } else {
      FD_SET(fd, &rdfds);
      FD_SET(fd, &wrfds);
    }

    if (select(fd + 1, &rdfds, &wrfds, NULL, timeout) <= 0) {
      return (-1);
    }
  }
  return (1);
}
