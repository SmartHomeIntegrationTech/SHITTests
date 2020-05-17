/*
 * Copyright (c) 2020 Karsten Becker All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

// WARNING, this is an automatically generated file!
// Don't change anything in here.
// Last update 2020-05-01

#include <iostream>
#include <string>

#include "LoggingHW.h"
// Configuration implementation for class SHI::LoggingHardwareConfig

namespace {}  // namespace

SHI::LoggingHardwareConfig::LoggingHardwareConfig(const JsonObject &obj)
    : loggingLevel(obj["loggingLevel"] | 0) {}

void SHI::LoggingHardwareConfig::fillData(JsonObject &doc) const {
  doc["loggingLevel"] = loggingLevel;
}

int SHI::LoggingHardwareConfig::getExpectedCapacity() const {
  return JSON_OBJECT_SIZE(1);
}
