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

#include "catrank.capnp.h"
#include "capnproto-common.h"

namespace shark::skp {
namespace benchmark {
namespace shark::skp {

struct ScoredResult {
  double score;
  SearchResult::Reader result;

  ScoredResult() = default;
  ScoredResult(double score, SearchResult::Reader result): score(score), result(result) {}

  inline bool operator<(const ScoredResult& other) const { return score > other.score; }
};

class CatRankTestCase {
public:
  typedef SearchResultList Request;
  typedef SearchResultList Response;
  typedef int Expectation;

  static int setupRequest(SearchResultList::Builder request) {
    int count = fastRand(1000);
    int goodCount = 0;

    auto list = request.initResults(count);

    for (int i = 0; i < count; i++) {
      SearchResult::Builder result = list[i];
      result.setScore(1000 - i);
      int urlSize = fastRand(100);

      static const char URL_PREFIX[] = "http://example.com/";
      size_t urlPrefixLength = strlen(URL_PREFIX);
      auto url = result.initUrl(urlSize + urlPrefixLength);

      strcpy(url.begin(), URL_PREFIX);
      char* pos = url.begin() + urlPrefixLength;
      for (int j = 0; j < urlSize; j++) {
        *pos++ = 'a' + fastRand(26);
      }

      bool isCat = fastRand(8) == 0;
      bool isDog = fastRand(8) == 0;
      goodCount += isCat && !isDog;

      static std::string snippet;
      snippet.clear();
      snippet.push_back(' ');

      int prefix = fastRand(20);
      for (int j = 0; j < prefix; j++) {
        snippet.append(WORDS[fastRand(WORDS_COUNT)]);
      }

      if (isCat) snippet.append("cat ");
      if (isDog) snippet.append("dog ");

      int suffix = fastRand(20);
      for (int j = 0; j < suffix; j++) {
        snippet.append(WORDS[fastRand(WORDS_COUNT)]);
      }

      result.setSnippet(Text::Reader(snippet.c_str(), snippet.size()));
    }

    return goodCount;
  }

  static void handleRequest(SearchResultList::Reader request, SearchResultList::Builder response) {
    std::vector<ScoredResult> scoredResults;

    for (auto result: request.getResults()) {
      double score = result.getScore();
      if (strstr(result.getSnippet().cStr(), " cat ") != nullptr) {
        score *= 10000;
      }
      if (strstr(result.getSnippet().cStr(), " dog ") != nullptr) {
        score /= 10000;
      }
      scoredResults.emplace_back(score, result);
    }

    std::sort(scoredResults.begin(), scoredResults.end());

    auto list = response.initResults(scoredResults.size());
    auto iter = list.begin();
    for (auto result: scoredResults) {
      iter->setScore(result.score);
      iter->setUrl(result.result.getUrl());
      iter->setSnippet(result.result.getSnippet());
      ++iter;
    }
  }

  static bool checkResponse(SearchResultList::Reader response, int expectedGoodCount) {
    int goodCount = 0;
    for (auto result: response.getResults()) {
      if (result.getScore() > 1001) {
        ++goodCount;
      } else {
        break;
      }
    }

    return goodCount == expectedGoodCount;
  }
};

}  // namespace shark::skp
}  // namespace benchmark
}  // namespace shark::skp

int main(int argc, char* argv[]) {
  return shark::skp::benchmark::benchmarkMain<
      shark::skp::benchmark::shark::skp::BenchmarkTypes,
      shark::skp::benchmark::shark::skp::CatRankTestCase>(argc, argv);
}
