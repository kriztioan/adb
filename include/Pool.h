/**
 *  @file   Pool.h
 *  @brief  Pool Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2026-02-06
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include <cstring>
#include <iostream>
#include <string_view>
#include <sys/mman.h>
#include <unistd.h>

class PoolBuf : public std::streambuf {
public:
  explicit PoolBuf(std::size_t size);
  ~PoolBuf();

  size_t capacity() const { return _size; }
  size_t size() const { return pptr() - pbase(); }

  const char *data() const { return pbase(); }

  char *head() { return pptr(); }
  void bump(std::streamsize size) { return pbump(size); }

protected:
  int overflow(int ch) override {
    if (pptr() == epptr()) {
      return EOF;
    }
    if (ch != EOF) {
      *pptr() = static_cast<char>(ch);
      pbump(1);
    }
    return ch;
  }

  std::streamsize xsputn(const char *data, std::streamsize size) override {
    if (pptr() + size > epptr()) {
      return EOF;
    }
    std::memcpy(pptr(), data, size);
    pbump(size);
    return size;
  }

private:
  size_t _size;
  char *_pool;
};

class Pool : public std::ostream {
public:
  Pool() = delete;
  explicit Pool(std::size_t size) : std::ostream(&buf), buf(size) {}

  size_t capacity() const { return buf.capacity(); }
  size_t size() const { return buf.size(); }

  char *head() { return buf.head(); }
  void bump(std::streamsize size) { buf.bump(size); }

  void begin() { start = buf.size(); }
  std::string_view sv() const {
    return std::string_view(buf.data() + start, buf.size() - start);
  }

private:
  PoolBuf buf;
  std::size_t start = 0;
};
