//
//  DummySensor.cpp
//  SHITest
//
//  Created by Karsten Becker on 20.02.20.
//  Copyright © 2020 Karsten Becker. All rights reserved.
//

#include "SHICommunicator.h"
#include "SHIHardware.h"
#include "SHISensor.h"
#include "SHIVisitor.h"

class DummySensor : public SHI::Sensor {
 public:
  DummySensor() : Sensor("Dummy") {}

  std::vector<SHI::MeasurementBundle> readSensor() override {
    static int count = 0;
    SHI::hw->logInfo(name, __func__, "Loop Dummy Sensor");
    auto humMeasure = humidty->measuredFloat(humidtyValue);
    SHI::Measurement tempMeasure =
        count++ >= 10 ? temperature->measuredNoData()
                      : temperature->measuredFloat(temperatureValue);
    if (count > 11) count = 0;
    return {SHI::MeasurementBundle({humMeasure, tempMeasure}, this)};
  }
  bool setupSensor() override {
    SHI::hw->logInfo(name, __func__, "Setup Dummy Sensor");
    addMetaData(humidty);
    addMetaData(temperature);
    return true;
  }
  bool stopSensor() override {
    SHI::hw->logInfo(name, __func__, "Stop Dummy Sensor");
    return true;
  }
  std::shared_ptr<SHI::MeasurementMetaData> humidty =
      std::make_shared<SHI::MeasurementMetaData>("Humidity", "%",
                                                 SHI::SensorDataType::FLOAT);
  std::shared_ptr<SHI::MeasurementMetaData> temperature =
      std::make_shared<SHI::MeasurementMetaData>("Temperature", "°C",
                                                 SHI::SensorDataType::FLOAT);
  float humidtyValue = 0;
  float temperatureValue = 0;
  const SHI::Configuration *getConfig() const override { return nullptr; }
  bool reconfigure(SHI::Configuration *newConfig) override { return true; }

 private:
};
class PrintHierachyVisitor : public SHI::Visitor {
 public:
  void enterVisit(SHI::Sensor *sensor) override {
    result += std::string(indent, ' ') + "S:" + std::string(sensor->getName()) +
              " Status:" + sensor->getStatus().stringRepresentation + "\n";
    indent++;
  }
  void leaveVisit(SHI::Sensor *sensor) override { indent--; }
  void enterVisit(SHI::SensorGroup *channel) override {
    result += std::string(indent, ' ') +
              "CH:" + std::string(channel->getName()) +
              " Status:" + channel->getStatus().stringRepresentation + "\n";
    indent++;
  }
  void leaveVisit(SHI::SensorGroup *channel) override { indent--; }
  void enterVisit(SHI::Hardware *harwdware) override {
    result += std::string(indent, ' ') +
              "HW:" + std::string(harwdware->getName()) +
              " Status:" + harwdware->getStatus().stringRepresentation + "\n";
    indent++;
  }
  void leaveVisit(SHI::Hardware *harwdware) override { indent--; }
  void visit(SHI::Communicator *communicator) override {
    result +=
        std::string(indent, ' ') + "C:" + std::string(communicator->getName()) +
        " Status:" + communicator->getStatus().stringRepresentation + "\n";
  }
  void visit(SHI::MeasurementMetaData *data) override {
    result += std::string(indent, ' ') + "MD:" + data->getName() +
              " unit:" + data->unit +
              " type:" + std::to_string((int)data->type) +
              " Status:" + data->getStatus().stringRepresentation + "\n";
  }

  int indent = 0;
  std::string result = "";
};
