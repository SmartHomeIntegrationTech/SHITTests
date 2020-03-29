/**
 * Copyright (c) 2020 Karsten Becker All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <time.h>

#include <string>

#include "DummySensor.h"
#include "LoggingComms.h"
#include "LoggingHW.h"
#include "SHIBus.h"
#include "SHIFactory.h"
#include "gtest/gtest.h"

class FactoryTest : public ::testing::Test {
 public:
  void registerDefaultFactories() {
    auto factory = SHI::Factory::get();
    ASSERT_NE(factory, nullptr);
    ASSERT_TRUE(factory->registerFactory("hw", [](JsonObject obj) {
      return SHI::Factory::defaultHardwareFactory(new SHI::LoggingHardware(),
                                                  obj);
    }));
    ASSERT_TRUE(factory->registerFactory(
        "logging", [](JsonObject obj) { return new LoggingCommunicator(); }));
    ASSERT_TRUE(factory->registerFactory("sensorGroup", [](JsonObject obj) {
      return SHI::Factory::defaultSensorGroupFactory(obj);
    }));
    ASSERT_TRUE(factory->registerFactory(
        "dummy", [](JsonObject obj) { return new DummySensor(); }));
  }
  StaticJsonDocument<50000> doc;
  JsonObject getJson(std::string json) {
    doc.clear();
    deserializeJson(doc, json);
    return doc.as<JsonObject>();
  }

  void TearDown() override {
    SHI::hw = nullptr;
    SHI::Factory::reset();
    doc.clear();
  }
};

TEST_F(FactoryTest, testRegistry) {
  auto factory = SHI::Factory::get();
  ASSERT_EQ(SHI::hw, nullptr);
  ASSERT_EQ(factory->construct("404 Website not found"),
            SHI::FactoryErrors::FailureToParseJson);
  ASSERT_EQ(factory->construct("{}"), SHI::FactoryErrors::NoHWKeyFound);
  ASSERT_EQ(factory->construct("{\"hw\":{}}"),
            SHI::FactoryErrors::MissingRegistryForHW);
  ASSERT_EQ(factory->construct("{\"hw\":[]}"),
            SHI::FactoryErrors::InvalidHWKeyFound);
  registerDefaultFactories();
  ASSERT_EQ(SHI::hw, nullptr);
  ASSERT_EQ(factory->construct("{\"hw\":{}}"), SHI::FactoryErrors::None);
  ASSERT_NE(SHI::hw, nullptr);
}

// TEST_F(FactoryTest, generateJSONConfig) { registerDefaultFactories(); }
