//
//  LoggingComms.hpp
//  SHITest
//
//  Created by Karsten Becker on 20.02.20.
//  Copyright © 2020 Karsten Becker. All rights reserved.
//

#pragma once

#include <stdio.h>

#include <string>

#include "SHICommunicator.h"
#include "SHIHardware.h"

class LoggingCommunicator : public SHI::Communicator {
 public:
  LoggingCommunicator() : Communicator("LoggingCommunicator") {}
  void setupCommunication() override { SHI::hw->logInfo(name, __func__, ""); }
  void loopCommunication() override { SHI::hw->logInfo(name, __func__, ""); }
  void newReading(const SHI::MeasurementBundle &reading) override {
    for (auto &&data : reading.data) {
      SHI::hw->logInfo(name, __func__,
                       std::string(reading.src->getName()) + "." +
                           data.getMetaData()->getName() + "=" +
                           data.toTransmitString() + data.getMetaData()->unit +
                           " " + dataType[(int)data.getMetaData()->type] + " " +
                           dataState[(int)data.getDataState()]);
    }
  }
  void newStatus(const SHI::Measurement &status, SHI::SHIObject *src) override {
    SHI::hw->logInfo(name, __func__, status.toTransmitString());
  }

  SHI::Configuration *getConfig() override { return nullptr; }

 protected:
  const std::string dataType[4]{"INT", "FLOAT", "STRING", "STATUS"};

  const std::string dataState[3]{"VALID", "NO_DATA", "ERROR"};
};
