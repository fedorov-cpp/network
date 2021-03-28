#include <gtest/gtest.h>
#include <udp/UdpClient.hpp>
#include <udp/UdpServer.hpp>
#include <chrono>

TEST(Udp, UdpTest) {
  using network::IOResult;

  size_t timeout_ms = 100UL;
  network::UdpServer server("0.0.0.0", 3333, UINT64_MAX, {}, true, true);
  network::UdpClient client("127.0.0.1", 3333);
  std::string test_string = "udp test string";
  EXPECT_EQ(IOResult::SUCCEED, client.send(test_string));
  std::chrono::high_resolution_clock::time_point sp =
    std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::time_point temp =
    std::chrono::high_resolution_clock::now();
  std::chrono::milliseconds passed = std::chrono::duration_cast<std::chrono::seconds>(temp - sp);
  std::string msg;
  while(static_cast<size_t>(passed.count()) <= timeout_ms) {
    temp = std::chrono::high_resolution_clock::now();
    passed = std::chrono::duration_cast<std::chrono::seconds>(temp - sp);
  }
  EXPECT_EQ(IOResult::SUCCEED, server.recv(msg));
  EXPECT_EQ(msg, test_string);
}