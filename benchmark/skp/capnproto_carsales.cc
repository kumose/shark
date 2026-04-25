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

#include "carsales.capnp.h"
#include "capnproto-common.h"

namespace shark::skp {
namespace benchmark {
namespace shark::skp {

template <typename ReaderOrBuilder>
uint64_t carValue(ReaderOrBuilder car) {
  // Do not think too hard about realism.

  uint64_t result = 0;

  result += car.getSeats() * 200;
  result += car.getDoors() * 350;
  for (auto wheel: car.getWheels()) {
    result += wheel.getDiameter() * wheel.getDiameter();
    result += wheel.getSnowTires() ? 100 : 0;
  }

  result += car.getLength() * car.getWidth() * car.getHeight() / 50;

  auto engine = car.getEngine();
  result += engine.getHorsepower() * 40;
  if (engine.getUsesElectric()) {
    if (engine.getUsesGas()) {
      // hybrid
      result += 5000;
    } else {
      result += 3000;
    }
  }

  result += car.getHasPowerWindows() ? 100 : 0;
  result += car.getHasPowerSteering() ? 200 : 0;
  result += car.getHasCruiseControl() ? 400 : 0;
  result += car.getHasNavSystem() ? 2000 : 0;

  result += car.getCupHolders() * 25;

  return result;
}

void randomCar(Car::Builder car) {
  // Do not think too hard about realism.

  static const char* const MAKES[] = { "Toyota", "GM", "Ford", "Honda", "Tesla" };
  static const char* const MODELS[] = { "Camry", "Prius", "Volt", "Accord", "Leaf", "Model S" };

  car.setMake(MAKES[fastRand(sizeof(MAKES) / sizeof(MAKES[0]))]);
  car.setModel(MODELS[fastRand(sizeof(MODELS) / sizeof(MODELS[0]))]);

  car.setColor((Color)fastRand((uint)Color::SILVER + 1));
  car.setSeats(2 + fastRand(6));
  car.setDoors(2 + fastRand(3));

  for (auto wheel: car.initWheels(4)) {
    wheel.setDiameter(25 + fastRand(15));
    wheel.setAirPressure(30 + fastRandDouble(20));
    wheel.setSnowTires(fastRand(16) == 0);
  }

  car.setLength(170 + fastRand(150));
  car.setWidth(48 + fastRand(36));
  car.setHeight(54 + fastRand(48));
  car.setWeight(car.getLength() * car.getWidth() * car.getHeight() / 200);

  auto engine = car.initEngine();
  engine.setHorsepower(100 * fastRand(400));
  engine.setCylinders(4 + 2 * fastRand(3));
  engine.setCc(800 + fastRand(10000));
  engine.setUsesGas(true);
  engine.setUsesElectric(fastRand(2));

  car.setFuelCapacity(10.0 + fastRandDouble(30.0));
  car.setFuelLevel(fastRandDouble(car.getFuelCapacity()));
  car.setHasPowerWindows(fastRand(2));
  car.setHasPowerSteering(fastRand(2));
  car.setHasCruiseControl(fastRand(2));
  car.setCupHolders(fastRand(12));
  car.setHasNavSystem(fastRand(2));
}

class CarSalesTestCase {
public:
  typedef ParkingLot Request;
  typedef TotalValue Response;
  typedef uint64_t Expectation;

  static uint64_t setupRequest(ParkingLot::Builder request) {
    uint64_t result = 0;
    for (auto car: request.initCars(fastRand(200))) {
      randomCar(car);
      result += carValue(car);
    }
    return result;
  }
  static void handleRequest(ParkingLot::Reader request, TotalValue::Builder response) {
    uint64_t result = 0;
    for (auto car: request.getCars()) {
      result += carValue(car);
    }
    response.setAmount(result);
  }
  static inline bool checkResponse(TotalValue::Reader response, uint64_t expected) {
    return response.getAmount() == expected;
  }
};

}  // namespace shark::skp
}  // namespace benchmark
}  // namespace shark::skp

int main(int argc, char* argv[]) {
  return shark::skp::benchmark::benchmarkMain<
      shark::skp::benchmark::shark::skp::BenchmarkTypes,
      shark::skp::benchmark::shark::skp::CarSalesTestCase>(argc, argv);
}
