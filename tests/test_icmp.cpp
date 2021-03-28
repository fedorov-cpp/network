#include <gtest/gtest.h>
#include <icmp/PingPort.hpp>
#include <chrono>

// @warning: on Linux to allow root to use icmp sockets, run (by default no user have permission to ICMP):
// sysctl -w net.ipv4.ping_group_range="0 0"
// For non-root:
// sudo sysctl -w net.ipv4.ping_group_range="0 $(id -u $(whoami))"

TEST(Icmp, PingPort_hostname) {
  using network::PingPort;
  size_t timeout_ms = 100UL;
  PingPort port("www.google.com", timeout_ms);
  std::chrono::high_resolution_clock::time_point sp =
    std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::time_point temp =
    std::chrono::high_resolution_clock::now();
  std::chrono::milliseconds passed = std::chrono::duration_cast<std::chrono::seconds>(temp - sp);
  while(static_cast<size_t>(passed.count()) <= timeout_ms) {
    temp = std::chrono::high_resolution_clock::now();
    passed = std::chrono::duration_cast<std::chrono::seconds>(temp - sp);
  }
  EXPECT_EQ(true, port.connected());
}

TEST(Icmp, PingPort_ip) {
  using network::PingPort;
  size_t timeout_ms = 100UL;
  PingPort port("0.0.0.0", timeout_ms);
  std::chrono::high_resolution_clock::time_point sp =
    std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::time_point temp =
    std::chrono::high_resolution_clock::now();
  std::chrono::milliseconds passed = std::chrono::duration_cast<std::chrono::seconds>(temp - sp);
  while(static_cast<size_t>(passed.count()) <= timeout_ms) {
    temp = std::chrono::high_resolution_clock::now();
    passed = std::chrono::duration_cast<std::chrono::seconds>(temp - sp);
  }
  EXPECT_EQ(true, port.connected());
}