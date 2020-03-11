//
//  LoggingHW.cpp
//  SHITest
//
//  Created by Karsten Becker on 20.02.20.
//  Copyright © 2020 Karsten Becker. All rights reserved.
//

#include <iostream>

#include "SHICommunicator.h"
#include "SHIHardware.h"
#include "SHISensor.h"
#include "SHIVisitor.h"

const uint8_t MAJOR_VERSION = 0;
const uint8_t MINOR_VERSION = 0;
const uint8_t PATCH_VERSION = 0;
const std::string VERSION = "0.1.0";

SHI::Hardware *SHI::hw = nullptr;
