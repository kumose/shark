// Copyright (c) 2013-2016 Sandstorm Development Group, Inc. and contributors
// SPDX-License-Identifier: MIT
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "common.h"

namespace shark::skp {
namespace benchmark {
namespace null {

uint64_t arena[1024*1024];
uint64_t* arenaPos = arena;

template <typename T>
T* allocate(int count = 1) {
  T* result = reinterpret_cast<T*>(arenaPos);
  arenaPos += (sizeof(T) * count + 7) / 8;
  if (arenaPos > arena + sizeof(arena) / sizeof(arena[0])) {
    throw std::bad_alloc();
  }
  return result;
}

char* copyString(const char* str) {
  size_t len = strlen(str);
  char* result = allocate<char>(len);
  memcpy(result, str, len + 1);
  return result;
}

template <typename T>
struct List {
  size_t size;
  T* items;

  inline T* begin() const { return items; }
  inline T* end() const { return items + size; }

  inline List<T>& init(size_t size) {
    this->size = size;
    items = allocate<T>(size);
    return *this;
  }
};

// =======================================================================================

struct SingleUseObjects {
  class ObjectSizeCounter {
  public:
    ObjectSizeCounter(uint64_t iters): counter(0) {}

    void add(uint64_t wordCount) {
      counter += wordCount;
    }

    uint64_t get() { return counter; }

  private:
    uint64_t counter;
  };
};

struct ReusableObjects {
  class ObjectSizeCounter {
  public:
    ObjectSizeCounter(uint64_t iters): iters(iters), maxSize(0) {}

    void add(size_t wordCount) {
      maxSize = std::max(wordCount, maxSize);
    }

    uint64_t get() { return iters * maxSize; }

  private:
    uint64_t iters;
    size_t maxSize;
  };
};

// =======================================================================================

template <typename TestCase, typename ReuseStrategy, typename Compression>
struct BenchmarkMethods {
  static uint64_t syncClient(int inputFd, int outputFd, uint64_t iters) {
    fprintf(stderr, "Null benchmark doesn't do I/O.\n");
    exit(1);
  }

  static uint64_t asyncClientSender(
      int outputFd, ProducerConsumerQueue<typename TestCase::Expectation>* expectations,
      uint64_t iters) {
    fprintf(stderr, "Null benchmark doesn't do I/O.\n");
    exit(1);
  }

  static void asyncClientReceiver(
      int inputFd, ProducerConsumerQueue<typename TestCase::Expectation>* expectations,
      uint64_t iters) {
    fprintf(stderr, "Null benchmark doesn't do I/O.\n");
    exit(1);
  }

  static uint64_t asyncClient(int inputFd, int outputFd, uint64_t iters) {
    fprintf(stderr, "Null benchmark doesn't do I/O.\n");
    exit(1);
  }

  static uint64_t server(int inputFd, int outputFd, uint64_t iters) {
    fprintf(stderr, "Null benchmark doesn't do I/O.\n");
    exit(1);
  }

  static uint64_t passByObject(uint64_t iters, bool countObjectSize) {
    typename ReuseStrategy::ObjectSizeCounter sizeCounter(iters);

    for (; iters > 0; --iters) {
      arenaPos = arena;

      typename TestCase::Request request;
      typename TestCase::Expectation expected = TestCase::setupRequest(&request);

      typename TestCase::Response response;
      TestCase::handleRequest(request, &response);
      if (!TestCase::checkResponse(response, expected)) {
        throw std::logic_error("Incorrect response.");
      }

      sizeCounter.add((arenaPos - arena) * sizeof(arena[0]));
    }

    return sizeCounter.get();
  }

  static uint64_t passByBytes(uint64_t iters) {
    fprintf(stderr, "Null benchmark doesn't do I/O.\n");
    exit(1);
  }
};

struct BenchmarkTypes {
  typedef void Uncompressed;
  typedef void Packed;
#if HAVE_SNAPPY
  typedef void SnappyCompressed;
#endif  // HAVE_SNAPPY

  typedef ReusableObjects ReusableResources;
  typedef SingleUseObjects SingleUseResources;

  template <typename TestCase, typename ReuseStrategy, typename Compression>
  struct BenchmarkMethods: public null::BenchmarkMethods<TestCase, ReuseStrategy, Compression> {};
};

}  // namespace null
}  // namespace benchmark
}  // namespace shark::skp
