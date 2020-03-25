//
//  LoggingHW.hpp
//  SHITest
//
//  Created by Karsten Becker on 20.02.20.
//  Copyright © 2020 Karsten Becker. All rights reserved.
//

#pragma once
#include <sys/time.h>

#include <iostream>
#include <string>

#include "SHIHardware.h"

namespace SHI {

class LoggingHardware : public SHI::Hardware {
 public:
  LoggingHardware() : SHI::Hardware("LoggingHardware") {}
  void resetWithReason(const std::string &reason, bool restart) override {
    logInfo(name, __func__, reason);
  }
  void errLeds(void) override { logInfo(name, __func__, ""); }

  void setupWatchdog() override { logInfo(name, __func__, ""); }
  void feedWatchdog() override { logInfo(name, __func__, ""); }
  void disableWatchdog() override { logInfo(name, __func__, ""); }

  std::string getNodeName() override {
    logInfo(name, __func__, "");
    return "LoggingHW";
  }
  std::string getResetReason() override {
    logInfo(name, __func__, "");
    return resetReason;
  }
  void resetConfig() override { logInfo(name, __func__, ""); }
  void printConfig() override { logInfo(name, __func__, ""); }

  void setup(const std::string &defaultName) override {
    logInfo(name, __func__, defaultName);
    setupSensors();
    setupCommunicators();
  }
  void loop() override {
    logInfo(name, __func__, "");
    internalLoop();
  }

  void logInfo(const std::string &name, const char *func,
               std::string message) override {
    log((std::string("INFO: ") + name + "." + func + "() " + message).c_str());
  }
  void logWarn(const std::string &name, const char *func,
               std::string message) override {
    log((std::string("WARN: ") + name + "." + func + "() " + message).c_str());
  }
  void logError(const std::string &name, const char *func,
                std::string message) override {
    log((std::string("ERROR: ") + name + "." + func + "() " + message).c_str());
  }
  int64_t getEpochInMs() override {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
  }
  const SHI::Configuration *getConfig() const override { return nullptr; }
  bool reconfigure(Configuration *newConfig) override { return true; }
  const char *resetReason = "NONE";

 protected:
  void log(const std::string &message) override {
    std::cout << message << std::endl;
  };
};

}  // namespace SHI
