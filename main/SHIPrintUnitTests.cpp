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

class StringPrinter : public SHI::Print {
 public:
  std::string result;
  size_t write(const uint8_t* buffer, size_t size) {
    auto temp = std::string(reinterpret_cast<const char*>(buffer), size);
    result += temp;
    return temp.size();
  }
  size_t write(uint8_t value) {
    result += static_cast<char>(value);
    return sizeof(char);
  }
  void testAndReset(const char* expected) {
    ASSERT_STREQ(expected, result.c_str());
    result = "";
  }

 private:
};

class PrinterTest : public ::testing::Test {
 public:
  StringPrinter printer;
};

TEST_F(PrinterTest, exceptionalCases) {
  const char* empty = nullptr;
  ASSERT_EQ(printer.print(empty), 0);
  printer.testAndReset("");
  ASSERT_EQ(printer.print(10, 0), 2) << printer.result;
  printer.testAndReset("10");
}

TEST_F(PrinterTest, simpleCasesIntegerWrite) {
  ASSERT_EQ(printer.write('A'), 1) << printer.result;
  printer.testAndReset("A");
  ASSERT_EQ(printer.write(65), 1) << printer.result;
  printer.testAndReset("A");
  ASSERT_EQ(printer.write(static_cast<uint8_t>(65)), 1) << printer.result;
  printer.testAndReset("A");
  ASSERT_EQ(printer.write(static_cast<int8_t>(65)), 1) << printer.result;
  printer.testAndReset("A");
  ASSERT_EQ(printer.write(static_cast<uint16_t>(65)), 1) << printer.result;
  printer.testAndReset("A");
  ASSERT_EQ(printer.write(static_cast<int16_t>(65)), 1) << printer.result;
  printer.testAndReset("A");
  ASSERT_EQ(printer.write(static_cast<uint32_t>(65)), 1) << printer.result;
  printer.testAndReset("A");
  ASSERT_EQ(printer.write(static_cast<int32_t>(65)), 1) << printer.result;
  printer.testAndReset("A");
  ASSERT_EQ(printer.write(static_cast<uint64_t>(65)), 1) << printer.result;
  printer.testAndReset("A");
  ASSERT_EQ(printer.write(static_cast<int64_t>(65)), 1) << printer.result;
  printer.testAndReset("A");
  ASSERT_EQ(printer.write(static_cast<int64_t>(0xAAAA41)), 1) << printer.result;
  printer.testAndReset("A");
}

TEST_F(PrinterTest, simpleCasesInteger) {
  ASSERT_EQ(printer.print(5), 1) << printer.result;
  printer.testAndReset("5");
  ASSERT_EQ(printer.print(static_cast<uint8_t>(5)), 1) << printer.result;
  printer.testAndReset("5");
  ASSERT_EQ(printer.print(static_cast<int8_t>(5)), 1) << printer.result;
  printer.testAndReset("5");
  ASSERT_EQ(printer.print(static_cast<uint16_t>(5)), 1) << printer.result;
  printer.testAndReset("5");
  ASSERT_EQ(printer.print(static_cast<int16_t>(5)), 1) << printer.result;
  printer.testAndReset("5");
  ASSERT_EQ(printer.print(static_cast<uint32_t>(5)), 1) << printer.result;
  printer.testAndReset("5");
  ASSERT_EQ(printer.print(static_cast<int32_t>(5)), 1) << printer.result;
  printer.testAndReset("5");
  ASSERT_EQ(printer.print(static_cast<uint64_t>(5)), 1) << printer.result;
  printer.testAndReset("5");
  ASSERT_EQ(printer.print(static_cast<int64_t>(5)), 1) << printer.result;
  printer.testAndReset("5");

  ASSERT_EQ(printer.print(-5), 2) << printer.result;
  printer.testAndReset("-5");
  ASSERT_EQ(printer.print(0xFFFFFFFFFF, 16), 10) << printer.result;
  printer.testAndReset("FFFFFFFFFF");
}

TEST_F(PrinterTest, simpleCasesFloat) {
  ASSERT_EQ(printer.print(3.14), 4) << printer.result;
  printer.testAndReset("3.14");
  ASSERT_EQ(printer.print(3.14, 5), 7) << printer.result;
  printer.testAndReset("3.14000");
  ASSERT_EQ(printer.print(3.14 / 0., 5), 3) << printer.result;
  printer.testAndReset("inf");
  ASSERT_EQ(printer.print(-3.14 / 0., 5), 4) << printer.result;
  printer.testAndReset("-inf");
  ASSERT_EQ(printer.print(0.0 / 0.0, 5), 3) << printer.result;
  printer.testAndReset("nan");
}

TEST_F(PrinterTest, simpleCasesStringChar) {
  ASSERT_EQ(printer.print('c'), 1) << printer.result;
  printer.testAndReset("c");
  ASSERT_EQ(printer.println('c'), 2) << printer.result;
  printer.testAndReset("c\n");
  ASSERT_EQ(printer.print("Hello"), 5) << printer.result;
  printer.testAndReset("Hello");
  ASSERT_EQ(printer.println("Hello"), 6) << printer.result;
  printer.testAndReset("Hello\n");
  ASSERT_EQ(printer.printf("Hello, this is longer than 32 characters. This "
                           "allows testing the dynamic allocation."),
            85)
      << printer.result;
  printer.testAndReset(
      "Hello, this is longer than 32 characters. This allows testing the "
      "dynamic allocation.");

  const time_t epoch = 1583835687;
  struct tm* now = gmtime(&epoch);  // NOLINT
  ASSERT_EQ(printer.print(now, NULL), 24) << printer.result;
  printer.testAndReset("Tue Mar 10 10:21:27 2020");
  ASSERT_EQ(printer.println(now, "%F %T"), 20) << printer.result;
  printer.testAndReset("2020-03-10 10:21:27\n");
  ASSERT_EQ(printer.print(now,
                          "Today is wonderful day on the %c and this is more "
                          "than 32 characters"),
            0)
      << "This is overruning the buffer and thus returns 0";
  printer.testAndReset("");
}
