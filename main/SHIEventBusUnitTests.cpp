/**
 * Copyright (c) 2020 Karsten Becker All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */
#include <time.h>

#include <string>

#include "SHIEventBus.h"
#include "SHIFactory.h"
#include "gtest/gtest.h"

using SHI::EventBus::Bus;

using SHI::EventBus::Event;
using SHI::EventBus::EventBuilder;

using SHI::EventBus::Subscriber;
using SHI::EventBus::SubscriberBuilder;

using SHI::EventBus::DataType;
using SHI::EventBus::EventType;
using SHI::EventBus::SourceType;

class EventBusTest : public ::testing::Test {
 public:
  EventBusTest() {}
  void SetUp() override { bus = Bus::get(); }
  void TearDown() override { bus->reset(); }
  Bus *bus;
};

TEST_F(EventBusTest, eventBuilder) {
  auto builder = EventBuilder::source(SourceType::ACTUATOR);
  auto data = std::make_shared<std::string>("Data");
  ASSERT_EQ(builder.build(data).get(), nullptr);
  builder = builder.data(DataType::FLOAT);
  ASSERT_EQ(builder.build(data).get(), nullptr);
  builder = builder.event(EventType::REQUEST);
  ASSERT_EQ(builder.build(data).get(), nullptr);
  builder = builder.hash("Hello");
  ASSERT_NE(builder.build(data).get(), nullptr);
  builder = EventBuilder::source(SourceType::UNDEFINED);
  ASSERT_EQ(builder.build(data).get(), nullptr);
  builder = builder.source(SourceType::COMMUNICATOR)
                .event(EventType::DATA)
                .data(DataType::UNDEFINED);
  ASSERT_EQ(builder.build(data).get(), nullptr);
}

TEST_F(EventBusTest, subscriberBuilder) {
  auto subscriber = SubscriberBuilder::everything().build();
  ASSERT_NE(subscriber.get(), nullptr);
  subscriber = SubscriberBuilder::empty().build();
  ASSERT_EQ(subscriber.get(), nullptr);
  auto builder = SubscriberBuilder::empty();
  ASSERT_EQ(builder.build().get(), nullptr);
  builder = builder.addSource(SourceType::SENSOR);
  ASSERT_EQ(builder.build().get(), nullptr);
  builder = builder.allEvents();
  ASSERT_NE(builder.build().get(), nullptr);
}

TEST_F(EventBusTest, testSubscriberMasks) {
  auto data = std::make_shared<std::string>("Hello World!");
  auto event = EventBuilder::source(SourceType::SENSOR)
                   .event(EventType::DATA)
                   .data(DataType::STRING)
                   .customField(0xF0)
                   .hash("BME680")
                   .build(data);
  SubscriberBuilder everythingBuilder = SubscriberBuilder::everything();
  SubscriberBuilder nothing = SubscriberBuilder::empty();
  auto everything = everythingBuilder.build();
  auto withoutSensor =
      everythingBuilder.excludeSource(SourceType::SENSOR).build();
  auto withoutData = everythingBuilder.excludeEvent(EventType::DATA).build();
  auto withoutFieldMask =
      everythingBuilder.excludeCustomFieldMask(0xF0).build();
  auto withSensorAndCommunicator = nothing.addSource(SourceType::ACTUATOR)
                                       .addSource(SourceType::COMMUNICATOR)
                                       .allEvents()
                                       .allDataTypes()
                                       .allHashedNames()
                                       .allCustomFields()
                                       .build();
  auto withLifeCycleAndLogging = nothing.allSources()
                                     .addEvent(EventType::LIFECYCLE)
                                     .addEvent(EventType::LOGGING)
                                     .allDataTypes()
                                     .allHashedNames()
                                     .allCustomFields()
                                     .build();
  auto with3fieldMask = nothing.allSources()
                            .allEvents()
                            .allDataTypes()
                            .allHashedNames()
                            .addCustomFieldMask(0x1)
                            .addCustomFieldMask(0x2)
                            .build();
  auto with3fieldMaskSet = nothing.allSources()
                               .allEvents()
                               .allDataTypes()
                               .allHashedNames()
                               .setCustomFieldMask(0x3)
                               .build();
  ASSERT_TRUE(everything->matches(*event));
  ASSERT_FALSE(withoutSensor->matches(*event));
  ASSERT_FALSE(withoutData->matches(*event));
  ASSERT_FALSE(withoutFieldMask->matches(*event));
  ASSERT_FALSE(withSensorAndCommunicator->matches(*event));
  ASSERT_FALSE(withLifeCycleAndLogging->matches(*event));
  ASSERT_FALSE(with3fieldMask->matches(*event));
  ASSERT_FALSE(with3fieldMaskSet->matches(*event));
  event = EventBuilder::source(SourceType::ACTUATOR)
              .event(EventType::LOGGING)
              .data(DataType::STRING)
              .customField(0x01)
              .hash("BME680")
              .build(data);
  ASSERT_TRUE(everything->matches(*event));
  ASSERT_TRUE(withoutSensor->matches(*event));
  ASSERT_TRUE(withoutData->matches(*event));
  ASSERT_TRUE(withoutFieldMask->matches(*event));
  ASSERT_TRUE(withSensorAndCommunicator->matches(*event));
  ASSERT_TRUE(withLifeCycleAndLogging->matches(*event));
  ASSERT_TRUE(with3fieldMask->matches(*event));
  ASSERT_TRUE(with3fieldMaskSet->matches(*event));
  event = EventBuilder::source(SourceType::COMMUNICATOR)
              .event(EventType::LIFECYCLE)
              .data(DataType::STRING)
              .customField(0x03)
              .hash("BME680")
              .build(data);
  ASSERT_TRUE(everything->matches(*event));
  ASSERT_TRUE(withoutSensor->matches(*event));
  ASSERT_TRUE(withoutData->matches(*event));
  ASSERT_TRUE(withoutFieldMask->matches(*event));
  ASSERT_TRUE(withSensorAndCommunicator->matches(*event));
  ASSERT_TRUE(withLifeCycleAndLogging->matches(*event));
  ASSERT_TRUE(with3fieldMask->matches(*event));
  ASSERT_TRUE(with3fieldMaskSet->matches(*event));
}

TEST_F(EventBusTest, testSubscriberMatching) {
  EventBuilder builder = EventBuilder::source(SourceType::SENSOR);
  auto eventBuilder = builder.event(EventType::DATA)
                          .data(DataType::STRING)
                          .customField(1)
                          .hash("BME680");
  auto event =
      eventBuilder.build(std::make_shared<std::string>("Hello World!"));
  auto everythinSub = SubscriberBuilder::everything().build();
  bus->subscribe(everythinSub);
  auto eventSubscriber = SubscriberBuilder::forEvent(*event.get()).build();
  bus->subscribe(eventSubscriber);
  auto unmatchedSource = SubscriberBuilder::forEvent(*event.get())
                             .setSource(SourceType::ACTUATOR)
                             .build();
  bus->subscribe(unmatchedSource);
  auto unmatchedData = SubscriberBuilder::forEvent(*event.get())
                           .setDataType(DataType::CONFIGURATION)
                           .build();
  bus->subscribe(unmatchedData);
  auto unmatchedEvent = SubscriberBuilder::forEvent(*event.get())
                            .setEvent(EventType::LIFECYCLE)
                            .build();
  bus->subscribe(unmatchedEvent);
  auto unmatchedCustomFieldExact =
      SubscriberBuilder::forEvent(*event.get()).setExactCustomField(7).build();
  bus->subscribe(unmatchedCustomFieldExact);
  auto unmatchedCustomFieldMask =
      SubscriberBuilder::forEvent(*event.get()).addCustomFieldMask(2).build();
  bus->subscribe(unmatchedCustomFieldMask);
  auto unmatchedHash =
      SubscriberBuilder::forEvent(*event.get()).setHashedName(1234).build();
  bus->subscribe(unmatchedHash);
  bus->publish(event);
  ASSERT_EQ(everythinSub->inbox.size(), 1);
  ASSERT_EQ(eventSubscriber->inbox.size(), 1);
  ASSERT_EQ(unmatchedSource->inbox.size(), 0);
  ASSERT_EQ(unmatchedEvent->inbox.size(), 0);
  ASSERT_EQ(unmatchedCustomFieldExact->inbox.size(), 0);
  ASSERT_EQ(unmatchedCustomFieldMask->inbox.size(), 0);
  ASSERT_EQ(unmatchedHash->inbox.size(), 0);
  // The subscribers are organized by event, so match will not be called by
  // publish for the event mismatch
  ASSERT_FALSE(unmatchedEvent->matches(*event));

  eventBuilder = eventBuilder.customField(3);
  event = eventBuilder.build(std::make_shared<std::string>("Hello World!"));
  bus->publish(event);
  ASSERT_EQ(everythinSub->inbox.size(), 2);
  ASSERT_EQ(eventSubscriber->inbox.size(), 1);
  ASSERT_EQ(unmatchedSource->inbox.size(), 0);
  ASSERT_EQ(unmatchedEvent->inbox.size(), 0);
  ASSERT_EQ(unmatchedCustomFieldExact->inbox.size(), 0);
  // This should now match as the field has now bit 1 &2 set
  ASSERT_EQ(unmatchedCustomFieldMask->inbox.size(), 1);
  ASSERT_EQ(unmatchedHash->inbox.size(), 0);
  eventBuilder = eventBuilder.hash("BME280");
  event = eventBuilder.build(std::make_shared<std::string>("Hello World!"));
  bus->publish(event);
  ASSERT_EQ(everythinSub->inbox.size(), 3);

  // Test if the subscriber is removed when it is disposed
  eventSubscriber.reset();
  bus->publish(event);
  ASSERT_EQ(everythinSub->inbox.size(), 4);
}
