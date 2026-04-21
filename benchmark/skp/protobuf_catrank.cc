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

#if !CAPNP_NO_PROTOBUF_BENCHMARK

#include "catrank.pb.h"
#include "protobuf-common.h"

namespace shark::skp {
namespace benchmark {
namespace protobuf {

struct ScoredResult {
  double score;
  const SearchResult* result;

  ScoredResult() = default;
  ScoredResult(double score, const SearchResult* result): score(score), result(result) {}

  inline bool operator<(const ScoredResult& other) const { return score > other.score; }
};

class CatRankTestCase {
public:
  typedef SearchResultList Request;
  typedef SearchResultList Response;
  typedef int Expectation;

  static int setupRequest(SearchResultList* request) {
    int count = fastRand(1000);
    int goodCount = 0;

    for (int i = 0; i < count; i++) {
      SearchResult* result = request->add_result();
      result->set_score(1000 - i);
      result->set_url("http://example.com/");
      std::string* url = result->mutable_url();
      int urlSize = fastRand(100);
      for (int j = 0; j < urlSize; j++) {
        url->push_back('a' + fastRand(26));
      }

      bool isCat = fastRand(8) == 0;
      bool isDog = fastRand(8) == 0;
      goodCount += isCat && !isDog;

      std::string* snippet = result->mutable_snippet();
      snippet->reserve(7 * 22);
      snippet->push_back(' ');

      int prefix = fastRand(20);
      for (int j = 0; j < prefix; j++) {
        snippet->append(WORDS[fastRand(WORDS_COUNT)]);
      }

      if (isCat) snippet->append("cat ");
      if (isDog) snippet->append("dog ");

      int suffix = fastRand(20);
      for (int j = 0; j < suffix; j++) {
        snippet->append(WORDS[fastRand(WORDS_COUNT)]);
      }
    }

    return goodCount;
  }

  static void handleRequest(const SearchResultList& request, SearchResultList* response) {
    std::vector<ScoredResult> scoredResults;

    for (auto& result: request.result()) {
      double score = result.score();
      if (result.snippet().find(" cat ") != std::string::npos) {
        score *= 10000;
      }
      if (result.snippet().find(" dog ") != std::string::npos) {
        score /= 10000;
      }
      scoredResults.emplace_back(score, &result);
    }

    std::sort(scoredResults.begin(), scoredResults.end());

    for (auto& result: scoredResults) {
      SearchResult* out = response->add_result();
      out->set_score(result.score);
      out->set_url(result.result->url());
      out->set_snippet(result.result->snippet());
    }
  }

  static bool checkResponse(const SearchResultList& response, int expectedGoodCount) {
    int goodCount = 0;
    for (auto& result: response.result()) {
      if (result.score() > 1001) {
        ++goodCount;
      } else {
        break;
      }
    }

    return goodCount == expectedGoodCount;
  }
};

}  // namespace protobuf
}  // namespace benchmark
}  // namespace shark::skp

int main(int argc, char* argv[]) {
  return shark::skp::benchmark::benchmarkMain<
      shark::skp::benchmark::protobuf::BenchmarkTypes,
      shark::skp::benchmark::protobuf::CatRankTestCase>(argc, argv);
}

#endif  // !CAPNP_NO_PROTOBUF_BENCHMARK
