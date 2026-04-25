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

enum class Color: uint8_t {
  BLACK,
  WHITE,
  RED,
  GREEN,
  BLUE,
  CYAN,
  MAGENTA,
  YELLOW,
  SILVER
};
constexpr uint COLOR_RANGE = static_cast<uint>(Color::SILVER) + 1;

struct Wheel {
  float airPressure;
  uint16_t diameter;
  bool snowTires;
};

struct Engine {
  uint32_t cc;
  uint16_t horsepower;
  uint8_t cylinders;
  uint8_t bits;
  inline bool usesGas()      const { return bits & 1; }
  inline bool usesElectric() const { return bits & 2; }

  inline void setBits(bool usesGas, bool usesElectric) {
    bits = (uint8_t)usesGas | ((uint8_t)usesElectric << 1);
  }
};

struct Car {
  // SORT FIELDS BY SIZE since we need "theoretical best" memory usage
  Engine engine;
  List<Wheel> wheels;
  const char* make;
  const char* model;
  float fuelCapacity;
  float fuelLevel;
  uint32_t weight;
  uint16_t length;
  uint16_t width;
  uint16_t height;
  Color color;
  uint8_t seats;
  uint8_t doors;
  uint8_t cupHolders;

  uint8_t bits;

  inline bool hasPowerWindows()  const { return bits & 1; }
  inline bool hasPowerSteering() const { return bits & 2; }
  inline bool hasCruiseControl() const { return bits & 4; }
  inline bool hasNavSystem()     const { return bits & 8; }

  inline void setBits(bool hasPowerWindows, bool hasPowerSteering,
                      bool hasCruiseControl, bool hasNavSystem) {
    bits = (uint8_t)hasPowerWindows
         | ((uint8_t)hasPowerSteering << 1)
         | ((uint8_t)hasCruiseControl << 2)
         | ((uint8_t)hasNavSystem << 3);
  }
};


uint64_t carValue(const Car& car) {
  // Do not think too hard about realism.

  uint64_t result = 0;

  result += car.seats * 200;
  result += car.doors * 350;
  for (auto wheel: car.wheels) {
    result += wheel.diameter * wheel.diameter;
    result += wheel.snowTires ? 100 : 0;
  }

  result += car.length * car.width * car.height / 50;

  auto engine = car.engine;
  result += engine.horsepower * 40;
  if (engine.usesElectric()) {
    if (engine.usesGas()) {
      // hybrid
      result += 5000;
    } else {
      result += 3000;
    }
  }

  result += car.hasPowerWindows() ? 100 : 0;
  result += car.hasPowerSteering() ? 200 : 0;
  result += car.hasCruiseControl() ? 400 : 0;
  result += car.hasNavSystem() ? 2000 : 0;

  result += car.cupHolders * 25;

  return result;
}

void randomCar(Car* car) {
  // Do not think too hard about realism.

  static const char* const MAKES[] = { "Toyota", "GM", "Ford", "Honda", "Tesla" };
  static const char* const MODELS[] = { "Camry", "Prius", "Volt", "Accord", "Leaf", "Model S" };

  car->make = copyString(MAKES[fastRand(sizeof(MAKES) / sizeof(MAKES[0]))]);
  car->model = copyString(MODELS[fastRand(sizeof(MODELS) / sizeof(MODELS[0]))]);

  car->color = (Color)fastRand(COLOR_RANGE);
  car->seats = 2 + fastRand(6);
  car->doors = 2 + fastRand(3);

  for (auto& wheel: car->wheels.init(4)) {
    wheel.diameter = 25 + fastRand(15);
    wheel.airPressure = 30 + fastRandDouble(20);
    wheel.snowTires = fastRand(16) == 0;
  }

  car->length = 170 + fastRand(150);
  car->width = 48 + fastRand(36);
  car->height = 54 + fastRand(48);
  car->weight = car->length * car->width * car->height / 200;

  car->engine.horsepower = 100 * fastRand(400);
  car->engine.cylinders = 4 + 2 * fastRand(3);
  car->engine.cc = 800 + fastRand(10000);
  car->engine.setBits(true, fastRand(2));

  car->fuelCapacity = 10.0 + fastRandDouble(30.0);
  car->fuelLevel = fastRandDouble(car->fuelCapacity);
  bool hasPowerWindows = fastRand(2);
  bool hasPowerSteering = fastRand(2);
  bool hasCruiseControl = fastRand(2);
  car->cupHolders = fastRand(12);
  bool hasNavSystem = fastRand(2);
  car->setBits(hasPowerWindows, hasPowerSteering, hasCruiseControl, hasNavSystem);
}

class CarSalesTestCase {
public:
  typedef List<Car> Request;
  typedef uint64_t Response;
  typedef uint64_t Expectation;

  static uint64_t setupRequest(List<Car>* request) {
    uint64_t result = 0;
    for (auto& car: request->init(fastRand(200))) {
      randomCar(&car);
      result += carValue(car);
    }
    return result;
  }
  static void handleRequest(const List<Car>& request, uint64_t* response) {
    *response = 0;
    for (auto& car: request) {
      *response += carValue(car);
    }
  }
  static inline bool checkResponse(uint64_t response, uint64_t expected) {
    return response == expected;
  }
};

}  // namespace null
}  // namespace benchmark
}  // namespace shark::skp

int main(int argc, char* argv[]) {
  return shark::skp::benchmark::benchmarkMain<
      shark::skp::benchmark::null::BenchmarkTypes,
      shark::skp::benchmark::null::CarSalesTestCase>(argc, argv);
}
