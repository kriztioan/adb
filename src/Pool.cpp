/**
 *  @file   Pool.cpp
 *  @brief  Pool Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2026-02-06
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Pool.h"

PoolBuf::PoolBuf(size_t size) : _size(size) {

  size_t page_size = getpagesize();

  _size = (size + page_size - 1) & ~(page_size - 1);

  _pool = (char *)mmap(nullptr, _size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (_pool == MAP_FAILED) {
    return;
  }

  setp(_pool, _pool + _size);
}

PoolBuf::~PoolBuf() {
  if (_pool) {
    munmap(_pool, _size);
  }
}
