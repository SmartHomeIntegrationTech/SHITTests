/**
 * Copyright (c) 2020 Karsten Becker All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <time.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "DummySensor.h"
#include "LoggingComms.h"
#include "LoggingHW.h"
#include "SHIBus.h"
#include "SHIFactory.h"
#include "gtest/gtest.h"

#define BASE_PATH "/Users/karstenbecker/PlatformIO/Projects/SHITTests/json/"

class FactoryTest : public ::testing::Test {
 public:
  void registerDefaultFactories() {
    auto factory = SHI::Factory::get();
    ASSERT_NE(factory, nullptr);
    ASSERT_TRUE(factory->registerFactory("hw", [=](JsonObject obj) {
      auto resObj = new SHI::LoggingHardware();
      return factory->defaultHardwareFactory(resObj, obj);
    }));
    ASSERT_TRUE(
        factory->registerFactory("LoggingCommunicator", [=](JsonObject obj) {
          auto resObj = new LoggingCommunicator();
          return factory->objToResult(resObj);
        }));
    ASSERT_TRUE(factory->registerFactory("sensorGroup", [=](JsonObject obj) {
      return factory->defaultSensorGroupFactory(obj);
    }));
    ASSERT_TRUE(factory->registerFactory("Dummy", [=](JsonObject obj) {
      return factory->objToResult(new DummySensor());
    }));
  }
  StaticJsonDocument<50000> doc;
  JsonObject getJson(std::string json) {
    doc.clear();
    deserializeJson(doc, json);
    return doc.as<JsonObject>();
  }

  std::string loadFile(const char* fileName) {
    std::ifstream inFile;
    inFile.open(fileName);  // open the input file

    std::stringstream strStream;
    strStream << inFile.rdbuf();  // read the file
    return strStream.str();       // str holds the content of the file
  }

  void writeFile(const char* fileName, std::string value) {
    std::ofstream outFile;
    outFile.open(fileName);
    outFile << value;
    outFile.close();
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
  ASSERT_EQ(factory->getError(factory->construct("404 Website not found")),
            SHI::FactoryErrors::FailureToParseJson);
  ASSERT_EQ(factory->getError(factory->construct("{}")),
            SHI::FactoryErrors::NoHWKeyFound);
  ASSERT_EQ(factory->getError(factory->construct("{\"hw\":{}}")),
            SHI::FactoryErrors::MissingRegistryForHW);
  ASSERT_EQ(factory->getError(factory->construct("{\"hw\":[]}")),
            SHI::FactoryErrors::InvalidHWKeyFound);
  registerDefaultFactories();
  ASSERT_EQ(SHI::hw, nullptr);
  ASSERT_EQ(factory->getError(factory->construct("{\"hw\":{}}")),
            SHI::FactoryErrors::None);
  ASSERT_NE(SHI::hw, nullptr);
}

TEST_F(FactoryTest, loadConfig) {
  std::string json = loadFile(BASE_PATH "in/construct.json");
  registerDefaultFactories();
  auto factory = SHI::Factory::get();
  auto constructionResult = factory->construct(json);
  ASSERT_EQ(factory->getError(constructionResult), SHI::FactoryErrors::None);
  PrintHierachyVisitor printer;
  SHI::hw->accept(printer);
  std::string printResult = loadFile(BASE_PATH "out/printVisitor.txt");
  ASSERT_STREQ(printer.result.c_str(), printResult.c_str());
  SHI::ConfigurationVisitor visitor;
  SHI::hw->accept(visitor);
  std::string visitorResult = visitor.toJson();
  writeFile(BASE_PATH "out/visitorResult.json", visitorResult);
  std::string jsonExpected = loadFile(BASE_PATH "out/construct.json");
  ASSERT_STREQ(visitorResult.c_str(), jsonExpected.c_str());
}

// TEST_F(FactoryTest, generateJSONConfig) { registerDefaultFactories(); }
