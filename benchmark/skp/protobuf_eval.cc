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

#include "eval.pb.h"
#include "protobuf-common.h"

namespace shark::skp {
namespace benchmark {
namespace protobuf {

int32_t makeExpression(Expression* exp, uint depth) {
  exp->set_op((Operation)(fastRand(Operation_MAX + 1)));

  int32_t left, right;

  if (fastRand(8) < depth) {
    left = fastRand(128) + 1;
    exp->set_left_value(left);
  } else {
    left = makeExpression(exp->mutable_left_expression(), depth + 1);
  }

  if (fastRand(8) < depth) {
    right = fastRand(128) + 1;
    exp->set_right_value(right);
  } else {
    right = makeExpression(exp->mutable_right_expression(), depth + 1);
  }

  switch (exp->op()) {
    case Operation::ADD:
      return left + right;
    case Operation::SUBTRACT:
      return left - right;
    case Operation::MULTIPLY:
      return left * right;
    case Operation::DIVIDE:
      return div(left, right);
    case Operation::MODULUS:
      return mod(left, right);
  }
  throw std::logic_error("Can't get here.");
}

int32_t evaluateExpression(const Expression& exp) {
  uint32_t left, right;

  if (exp.has_left_value()) {
    left = exp.left_value();
  } else {
    left = evaluateExpression(exp.left_expression());
  }

  if (exp.has_right_value()) {
    right = exp.right_value();
  } else {
    right = evaluateExpression(exp.right_expression());
  }

  switch (exp.op()) {
    case Operation::ADD:
      return left + right;
    case Operation::SUBTRACT:
      return left - right;
    case Operation::MULTIPLY:
      return left * right;
    case Operation::DIVIDE:
      return div(left, right);
    case Operation::MODULUS:
      return mod(left, right);
  }
  throw std::logic_error("Can't get here.");
}

class ExpressionTestCase {
public:
  typedef Expression Request;
  typedef EvaluationResult Response;
  typedef int32_t Expectation;

  static inline int32_t setupRequest(Expression* request) {
    return makeExpression(request, 0);
  }
  static inline void handleRequest(const Expression& request, EvaluationResult* response) {
    response->set_value(evaluateExpression(request));
  }
  static inline bool checkResponse(const EvaluationResult& response, int32_t expected) {
    return response.value() == expected;
  }
};

}  // namespace protobuf
}  // namespace benchmark
}  // namespace shark::skp

int main(int argc, char* argv[]) {
  return shark::skp::benchmark::benchmarkMain<
      shark::skp::benchmark::protobuf::BenchmarkTypes,
      shark::skp::benchmark::protobuf::ExpressionTestCase>(argc, argv);
}

#endif  // !CAPNP_NO_PROTOBUF_BENCHMARK
