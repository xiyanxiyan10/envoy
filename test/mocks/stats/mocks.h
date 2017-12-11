#pragma once

#include <chrono>
#include <cstdint>
#include <list>
#include <string>

#include "envoy/stats/stats.h"
#include "envoy/stats/timespan.h"
#include "envoy/thread_local/thread_local.h"
#include "envoy/upstream/cluster_manager.h"

#include "common/stats/stats_impl.h"

#include "gmock/gmock.h"

namespace Envoy {
namespace Stats {

class MockCounter : public Counter {
public:
  MockCounter();
  ~MockCounter();

  MOCK_METHOD1(add, void(uint64_t amount));
  MOCK_METHOD0(inc, void());
  MOCK_METHOD0(latch, uint64_t());
  MOCK_CONST_METHOD0(name, const std::string&());
  MOCK_CONST_METHOD0(tagExtractedName, const std::string&());
  MOCK_CONST_METHOD0(tags, const std::vector<Tag>&());
  MOCK_METHOD0(reset, void());
  MOCK_CONST_METHOD0(used, bool());
  MOCK_CONST_METHOD0(value, uint64_t());

  std::string name_;
  std::vector<Tag> tags_;
};

class MockGauge : public Gauge {
public:
  MockGauge();
  ~MockGauge();

  MOCK_METHOD1(add, void(uint64_t amount));
  MOCK_METHOD0(dec, void());
  MOCK_METHOD0(inc, void());
  MOCK_CONST_METHOD0(name, const std::string&());
  MOCK_CONST_METHOD0(tagExtractedName, const std::string&());
  MOCK_CONST_METHOD0(tags, const std::vector<Tag>&());
  MOCK_METHOD1(set, void(uint64_t value));
  MOCK_METHOD1(sub, void(uint64_t amount));
  MOCK_CONST_METHOD0(used, bool());
  MOCK_CONST_METHOD0(value, uint64_t());

  std::string name_;
  std::vector<Tag> tags_;
};

class MockHistogram : public Histogram {
public:
  MockHistogram();
  ~MockHistogram();

  // Note: cannot be mocked because it is accessed as a Property in a gmock EXPECT_CALL. This
  // creates a deadlock in gmock and is an unintended use of mock functions.
  const std::string& name() const override { return name_; };

  MOCK_CONST_METHOD0(tagExtractedName, const std::string&());
  MOCK_CONST_METHOD0(tags, const std::vector<Tag>&());
  MOCK_METHOD1(recordValue, void(uint64_t value));

  std::string name_;
  std::vector<Tag> tags_;
  Store* store_;
};

class MockSink : public Sink {
public:
  MockSink();
  ~MockSink();

  MOCK_METHOD0(beginFlush, void());
  MOCK_METHOD2(flushCounter, void(const Counter& counter, uint64_t delta));
  MOCK_METHOD2(flushGauge, void(const Gauge& gauge, uint64_t value));
  MOCK_METHOD0(endFlush, void());
  MOCK_METHOD2(onHistogramComplete, void(const Histogram& histogram, uint64_t value));
};

class MockStore : public Store {
public:
  MockStore();
  ~MockStore();

  ScopePtr createScope(const std::string& name) override { return ScopePtr{createScope_(name)}; }

  MOCK_METHOD2(deliverHistogramToSinks, void(const Histogram& histogram, uint64_t value));
  MOCK_METHOD1(counter, Counter&(const std::string&));
  MOCK_CONST_METHOD0(counters, std::list<CounterSharedPtr>());
  MOCK_METHOD1(createScope_, Scope*(const std::string& name));
  MOCK_METHOD1(gauge, Gauge&(const std::string&));
  MOCK_CONST_METHOD0(gauges, std::list<GaugeSharedPtr>());
  MOCK_METHOD1(histogram, Histogram&(const std::string& name));

  testing::NiceMock<MockCounter> counter_;
  std::vector<std::unique_ptr<MockHistogram>> histograms_;
};

/**
 * With IsolatedStoreImpl it's hard to test timing stats.
 * MockIsolatedStatsStore mocks only deliverHistogramToSinks for better testing.
 */
class MockIsolatedStatsStore : public IsolatedStoreImpl {
public:
  MockIsolatedStatsStore();
  ~MockIsolatedStatsStore();

  MOCK_METHOD2(deliverHistogramToSinks, void(const Histogram& histogram, uint64_t value));
};

} // namespace Stats
} // namespace Envoy
