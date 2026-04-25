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

#include "null-common.h"

namespace shark::skp {
namespace benchmark {
namespace null {

enum class Operation {
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  MODULUS
};
uint OPERATION_RANGE = static_cast<uint>(Operation::MODULUS) + 1;

struct Expression {
  Operation op;

  bool leftIsValue;
  bool rightIsValue;

  union {
    int32_t leftValue;
    Expression* leftExpression;
  };

  union {
    int32_t rightValue;
    Expression* rightExpression;
  };
};

int32_t makeExpression(Expression* exp, uint depth) {
  exp->op = (Operation)(fastRand(OPERATION_RANGE));

  int32_t left, right;

  if (fastRand(8) < depth) {
    exp->leftIsValue = true;
    left = fastRand(128) + 1;
    exp->leftValue = left;
  } else {
    exp->leftIsValue = false;
    exp->leftExpression = allocate<Expression>();
    left = makeExpression(exp->leftExpression, depth + 1);
  }

  if (fastRand(8) < depth) {
    exp->rightIsValue = true;
    right = fastRand(128) + 1;
    exp->rightValue = right;
  } else {
    exp->rightIsValue = false;
    exp->rightExpression = allocate<Expression>();
    right = makeExpression(exp->rightExpression, depth + 1);
  }

  switch (exp->op) {
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

  if (exp.leftIsValue) {
    left = exp.leftValue;
  } else {
    left = evaluateExpression(*exp.leftExpression);
  }

  if (exp.rightIsValue) {
    right = exp.rightValue;
  } else {
    right = evaluateExpression(*exp.rightExpression);
  }

  switch (exp.op) {
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
  typedef int32_t Response;
  typedef int32_t Expectation;

  static inline int32_t setupRequest(Expression* request) {
    return makeExpression(request, 0);
  }
  static inline void handleRequest(const Expression& request, int32_t* response) {
    *response = evaluateExpression(request);
  }
  static inline bool checkResponse(int32_t response, int32_t expected) {
    return response == expected;
  }

  static size_t spaceUsed(const Expression& expression) {
    return sizeof(Expression) +
        (expression.leftExpression == nullptr ? 0 : spaceUsed(*expression.leftExpression)) +
        (expression.rightExpression == nullptr ? 0 : spaceUsed(*expression.rightExpression));
  }
};

}  // namespace null
}  // namespace benchmark
}  // namespace shark::skp

int main(int argc, char* argv[]) {
  return shark::skp::benchmark::benchmarkMain<
      shark::skp::benchmark::null::BenchmarkTypes,
      shark::skp::benchmark::null::ExpressionTestCase>(argc, argv);
}
