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

#pragma once

#if defined(__GNUC__) && !defined(CAPNP_HEADER_WARNINGS)
#pragma GCC system_header
#endif

#include "common.h"
#include <shark/skp/serialize.h>
#include <shark/skp/serialize-packed.h>
#include <shark/tml/debug.h>
#if HAVE_SNAPPY
#include <shark/skp/serialize-snappy.h>
#endif  // HAVE_SNAPPY
#include <thread>

namespace shark::skp {
namespace benchmark {
namespace shark::skp {

class CountingOutputStream: public shark::rt::FdOutputStream {
public:
  CountingOutputStream(int fd): FdOutputStream(fd), throughput(0) {}

  uint64_t throughput;

  void write(const void* buffer, size_t size) override {
    FdOutputStream::write(buffer, size);
    throughput += size;
  }

  void write(shark::rt::ArrayPtr<const shark::rt::ArrayPtr<const byte>> pieces) override {
    FdOutputStream::write(pieces);
    for (auto& piece: pieces) {
      throughput += piece.size();
    }
  }
};

// =======================================================================================

struct Uncompressed {
  typedef shark::rt::FdInputStream& BufferedInput;
  typedef InputStreamMessageReader MessageReader;

  class ArrayMessageReader: public FlatArrayMessageReader {
  public:
    ArrayMessageReader(shark::rt::ArrayPtr<const byte> array,
                       ReaderOptions options = ReaderOptions(),
                       shark::rt::ArrayPtr<word> scratchSpace = nullptr)
      : FlatArrayMessageReader(shark::rt::arrayPtr(
          reinterpret_cast<const word*>(array.begin()),
          reinterpret_cast<const word*>(array.end())), options) {}
  };

  static inline void write(shark::rt::OutputStream& output, MessageBuilder& builder) {
    writeMessage(output, builder);
  }
};

struct Packed {
  typedef shark::rt::BufferedInputStreamWrapper BufferedInput;
  typedef PackedMessageReader MessageReader;

  class ArrayMessageReader: private shark::rt::ArrayInputStream, public PackedMessageReader {
  public:
    ArrayMessageReader(shark::rt::ArrayPtr<const byte> array,
                       ReaderOptions options = ReaderOptions(),
                       shark::rt::ArrayPtr<word> scratchSpace = nullptr)
      : ArrayInputStream(array),
        PackedMessageReader(*this, options, scratchSpace) {}
  };

  static inline void write(shark::rt::OutputStream& output, MessageBuilder& builder) {
    writePackedMessage(output, builder);
  }

  static inline void write(shark::rt::BufferedOutputStream& output, MessageBuilder& builder) {
    writePackedMessage(output, builder);
  }
};

#if HAVE_SNAPPY
static byte snappyReadBuffer[SNAPPY_BUFFER_SIZE];
static byte snappyWriteBuffer[SNAPPY_BUFFER_SIZE];
static byte snappyCompressedBuffer[SNAPPY_COMPRESSED_BUFFER_SIZE];

struct SnappyCompressed {
  typedef BufferedInputStreamWrapper BufferedInput;
  typedef SnappyPackedMessageReader MessageReader;

  class ArrayMessageReader: private ArrayInputStream, public SnappyPackedMessageReader {
  public:
    ArrayMessageReader(shark::rt::ArrayPtr<const byte> array,
                       ReaderOptions options = ReaderOptions(),
                       shark::rt::ArrayPtr<word> scratchSpace = nullptr)
      : ArrayInputStream(array),
        SnappyPackedMessageReader(static_cast<ArrayInputStream&>(*this), options, scratchSpace,
                                  shark::rt::arrayPtr(snappyReadBuffer, SNAPPY_BUFFER_SIZE)) {}
  };

  static inline void write(OutputStream& output, MessageBuilder& builder) {
    writeSnappyPackedMessage(output, builder,
        shark::rt::arrayPtr(snappyWriteBuffer, SNAPPY_BUFFER_SIZE),
        shark::rt::arrayPtr(snappyCompressedBuffer, SNAPPY_COMPRESSED_BUFFER_SIZE));
  }
};
#endif  // HAVE_SNAPPY

// =======================================================================================

struct NoScratch {
  struct ScratchSpace {};

  template <typename Compression>
  class MessageReader: public Compression::MessageReader {
  public:
    inline MessageReader(typename Compression::BufferedInput& input, ScratchSpace& scratch)
        : Compression::MessageReader(input) {}
  };

  template <typename Compression>
  class ArrayMessageReader: public Compression::ArrayMessageReader {
  public:
    inline ArrayMessageReader(shark::rt::ArrayPtr<const byte> input, ScratchSpace& scratch)
        : Compression::ArrayMessageReader(input) {}
  };

  class MessageBuilder: public MallocMessageBuilder {
  public:
    inline MessageBuilder(ScratchSpace& scratch): MallocMessageBuilder() {}
  };

  class ObjectSizeCounter {
  public:
    ObjectSizeCounter(uint64_t iters): counter(0) {}

    template <typename RequestBuilder, typename ResponseBuilder>
    void add(RequestBuilder& request, ResponseBuilder& response) {
      for (auto segment: request.getSegmentsForOutput()) {
        counter += segment.size() * sizeof(word);
      }
      for (auto segment: response.getSegmentsForOutput()) {
        counter += segment.size() * sizeof(word);
      }
    }

    uint64_t get() { return counter; }

  private:
    uint64_t counter;
  };
};

constexpr size_t SCRATCH_SIZE = 128 * 1024;
word scratchSpace[6 * SCRATCH_SIZE];
int scratchCounter = 0;

struct UseScratch {
  struct ScratchSpace {
    word* words;

    ScratchSpace() {
      KJ_REQUIRE(scratchCounter < 6, "Too many scratch spaces needed at once.");
      words = scratchSpace + scratchCounter++ * SCRATCH_SIZE;
    }
    ~ScratchSpace() noexcept {
      --scratchCounter;
    }
  };

  template <typename Compression>
  class MessageReader: public Compression::MessageReader {
  public:
    inline MessageReader(typename Compression::BufferedInput& input, ScratchSpace& scratch)
        : Compression::MessageReader(
            input, ReaderOptions(), shark::rt::arrayPtr(scratch.words, SCRATCH_SIZE)) {}
  };

  template <typename Compression>
  class ArrayMessageReader: public Compression::ArrayMessageReader {
  public:
    inline ArrayMessageReader(shark::rt::ArrayPtr<const byte> input, ScratchSpace& scratch)
        : Compression::ArrayMessageReader(
            input, ReaderOptions(), shark::rt::arrayPtr(scratch.words, SCRATCH_SIZE)) {}
  };

  class MessageBuilder: public MallocMessageBuilder {
  public:
    inline MessageBuilder(ScratchSpace& scratch)
        : MallocMessageBuilder(shark::rt::arrayPtr(scratch.words, SCRATCH_SIZE)) {}
  };

  class ObjectSizeCounter {
  public:
    ObjectSizeCounter(uint64_t iters): iters(iters), maxSize(0) {}

    template <typename RequestBuilder, typename ResponseBuilder>
    void add(RequestBuilder& request, ResponseBuilder& response) {
      size_t counter = 0;
      for (auto segment: request.getSegmentsForOutput()) {
        counter += segment.size() * sizeof(word);
      }
      for (auto segment: response.getSegmentsForOutput()) {
        counter += segment.size() * sizeof(word);
      }
      maxSize = std::max(counter, maxSize);
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
    shark::rt::FdInputStream inputStream(inputFd);
    typename Compression::BufferedInput bufferedInput(inputStream);

    CountingOutputStream output(outputFd);
    typename ReuseStrategy::ScratchSpace builderScratch;
    typename ReuseStrategy::ScratchSpace readerScratch;

    for (; iters > 0; --iters) {
      typename TestCase::Expectation expected;
      {
        typename ReuseStrategy::MessageBuilder builder(builderScratch);
        expected = TestCase::setupRequest(
            builder.template initRoot<typename TestCase::Request>());
        Compression::write(output, builder);
      }

      {
        typename ReuseStrategy::template MessageReader<Compression> reader(
            bufferedInput, readerScratch);
        if (!TestCase::checkResponse(
            reader.template getRoot<typename TestCase::Response>(), expected)) {
          throw std::logic_error("Incorrect response.");
        }
      }
    }

    return output.throughput;
  }

  static uint64_t asyncClientSender(
      int outputFd, ProducerConsumerQueue<typename TestCase::Expectation>* expectations,
      uint64_t iters) {
    CountingOutputStream output(outputFd);
    typename ReuseStrategy::ScratchSpace scratch;

    for (; iters > 0; --iters) {
      typename ReuseStrategy::MessageBuilder builder(scratch);
      expectations->post(TestCase::setupRequest(
          builder.template initRoot<typename TestCase::Request>()));
      Compression::write(output, builder);
    }

    return output.throughput;
  }

  static void asyncClientReceiver(
      int inputFd, ProducerConsumerQueue<typename TestCase::Expectation>* expectations,
      uint64_t iters) {
    shark::rt::FdInputStream inputStream(inputFd);
    typename Compression::BufferedInput bufferedInput(inputStream);

    typename ReuseStrategy::ScratchSpace scratch;

    for (; iters > 0; --iters) {
      typename TestCase::Expectation expected = expectations->next();
      typename ReuseStrategy::template MessageReader<Compression> reader(bufferedInput, scratch);
      if (!TestCase::checkResponse(
          reader.template getRoot<typename TestCase::Response>(), expected)) {
        throw std::logic_error("Incorrect response.");
      }
    }
  }

  static uint64_t asyncClient(int inputFd, int outputFd, uint64_t iters) {
    ProducerConsumerQueue<typename TestCase::Expectation> expectations;
    std::thread receiverThread(asyncClientReceiver, inputFd, &expectations, iters);
    uint64_t throughput = asyncClientSender(outputFd, &expectations, iters);
    receiverThread.join();
    return throughput;
  }

  static uint64_t server(int inputFd, int outputFd, uint64_t iters) {
    shark::rt::FdInputStream inputStream(inputFd);
    typename Compression::BufferedInput bufferedInput(inputStream);

    CountingOutputStream output(outputFd);
    typename ReuseStrategy::ScratchSpace builderScratch;
    typename ReuseStrategy::ScratchSpace readerScratch;

    for (; iters > 0; --iters) {
      typename ReuseStrategy::MessageBuilder builder(builderScratch);
      typename ReuseStrategy::template MessageReader<Compression> reader(
          bufferedInput, readerScratch);
      TestCase::handleRequest(reader.template getRoot<typename TestCase::Request>(),
                              builder.template initRoot<typename TestCase::Response>());
      Compression::write(output, builder);
    }

    return output.throughput;
  }

  static uint64_t passByObject(uint64_t iters, bool countObjectSize) {
    typename ReuseStrategy::ScratchSpace requestScratch;
    typename ReuseStrategy::ScratchSpace responseScratch;

    typename ReuseStrategy::ObjectSizeCounter counter(iters);

    for (; iters > 0; --iters) {
      typename ReuseStrategy::MessageBuilder requestMessage(requestScratch);
      auto request = requestMessage.template initRoot<typename TestCase::Request>();
      typename TestCase::Expectation expected = TestCase::setupRequest(request);

      typename ReuseStrategy::MessageBuilder responseMessage(responseScratch);
      auto response = responseMessage.template initRoot<typename TestCase::Response>();
      TestCase::handleRequest(request.asReader(), response);

      if (!TestCase::checkResponse(response.asReader(), expected)) {
        throw std::logic_error("Incorrect response.");
      }

      if (countObjectSize) {
        counter.add(requestMessage, responseMessage);
      }
    }

    return counter.get();
  }

  static uint64_t passByBytes(uint64_t iters) {
    uint64_t throughput = 0;
    typename ReuseStrategy::ScratchSpace clientRequestScratch;
    UseScratch::ScratchSpace requestBytesScratch;
    typename ReuseStrategy::ScratchSpace serverRequestScratch;
    typename ReuseStrategy::ScratchSpace serverResponseScratch;
    UseScratch::ScratchSpace responseBytesScratch;
    typename ReuseStrategy::ScratchSpace clientResponseScratch;

    for (; iters > 0; --iters) {
      typename ReuseStrategy::MessageBuilder requestBuilder(clientRequestScratch);
      typename TestCase::Expectation expected = TestCase::setupRequest(
          requestBuilder.template initRoot<typename TestCase::Request>());

      shark::rt::ArrayOutputStream requestOutput(shark::rt::arrayPtr(
          reinterpret_cast<byte*>(requestBytesScratch.words), SCRATCH_SIZE * sizeof(word)));
      Compression::write(requestOutput, requestBuilder);
      throughput += requestOutput.getArray().size();
      typename ReuseStrategy::template ArrayMessageReader<Compression> requestReader(
          requestOutput.getArray(), serverRequestScratch);

      typename ReuseStrategy::MessageBuilder responseBuilder(serverResponseScratch);
      TestCase::handleRequest(requestReader.template getRoot<typename TestCase::Request>(),
                              responseBuilder.template initRoot<typename TestCase::Response>());

      shark::rt::ArrayOutputStream responseOutput(
          shark::rt::arrayPtr(reinterpret_cast<byte*>(responseBytesScratch.words),
                       SCRATCH_SIZE * sizeof(word)));
      Compression::write(responseOutput, responseBuilder);
      throughput += responseOutput.getArray().size();
      typename ReuseStrategy::template ArrayMessageReader<Compression> responseReader(
          responseOutput.getArray(), clientResponseScratch);

      if (!TestCase::checkResponse(
          responseReader.template getRoot<typename TestCase::Response>(), expected)) {
        throw std::logic_error("Incorrect response.");
      }
     }

    return throughput;
  }
};

struct BenchmarkTypes {
  typedef shark::skp::Uncompressed Uncompressed;
  typedef shark::skp::Packed Packed;
#if HAVE_SNAPPY
  typedef shark::skp::SnappyCompressed SnappyCompressed;
#endif  // HAVE_SNAPPY

  typedef shark::skp::UseScratch ReusableResources;
  typedef shark::skp::NoScratch SingleUseResources;

  template <typename TestCase, typename ReuseStrategy, typename Compression>
  struct BenchmarkMethods: public shark::skp::BenchmarkMethods<TestCase, ReuseStrategy, Compression> {};
};

}  // namespace shark::skp
}  // namespace benchmark
}  // namespace shark::skp
