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

#include "eval.capnp.h"
#include "capnproto-common.h"

namespace shark::skp {
namespace benchmark {
namespace shark::skp {

int32_t makeExpression(Expression::Builder exp, uint depth) {
  exp.setOp((Operation)(fastRand((int)Operation::MODULUS + 1)));

  uint32_t left, right;

  if (fastRand(8) < depth) {
    left = fastRand(128) + 1;
    exp.getLeft().setValue(left);
  } else {
    left = makeExpression(exp.getLeft().initExpression(), depth + 1);
  }

  if (fastRand(8) < depth) {
    right = fastRand(128) + 1;
    exp.getRight().setValue(right);
  } else {
    right = makeExpression(exp.getRight().initExpression(), depth + 1);
  }

  switch (exp.getOp()) {
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

int32_t evaluateExpression(Expression::Reader exp) {
  int32_t left = 0, right = 0;

  switch (exp.getLeft().which()) {
    case Expression::Left::VALUE:
      left = exp.getLeft().getValue();
      break;
    case Expression::Left::EXPRESSION:
      left = evaluateExpression(exp.getLeft().getExpression());
      break;
  }

  switch (exp.getRight().which()) {
    case Expression::Right::VALUE:
      right = exp.getRight().getValue();
      break;
    case Expression::Right::EXPRESSION:
      right = evaluateExpression(exp.getRight().getExpression());
      break;
  }

  switch (exp.getOp()) {
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

  static inline int32_t setupRequest(Expression::Builder request) {
    return makeExpression(request, 0);
  }
  static inline void handleRequest(Expression::Reader request, EvaluationResult::Builder response) {
    response.setValue(evaluateExpression(request));
  }
  static inline bool checkResponse(EvaluationResult::Reader response, int32_t expected) {
    return response.getValue() == expected;
  }
};

}  // namespace shark::skp
}  // namespace benchmark
}  // namespace shark::skp

int main(int argc, char* argv[]) {
  return shark::skp::benchmark::benchmarkMain<
      shark::skp::benchmark::shark::skp::BenchmarkTypes,
      shark::skp::benchmark::shark::skp::ExpressionTestCase>(argc, argv);
}
