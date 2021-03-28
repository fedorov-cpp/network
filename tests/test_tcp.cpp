#include <gtest/gtest.h>
#include <tcp/TcpClient.hpp>
#include <tcp/TcpServer.hpp>
#include <chrono>

namespace {
  void delay(size_t timeout_ms) {
    std::chrono::high_resolution_clock::time_point sp =
      std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point temp =
      std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds passed = std::chrono::duration_cast<std::chrono::seconds>(temp - sp);
    while(static_cast<size_t>(passed.count()) <= timeout_ms) {
      temp = std::chrono::high_resolution_clock::now();
      passed = std::chrono::duration_cast<std::chrono::seconds>(temp - sp);
    }
  }
}

TEST(Tcp, TcpTest) {
  using network::IOResult;

  network::TcpServer server("0.0.0.0", 3333, {}, true);
  network::TcpClient client("127.0.0.1", 3333);
  server.accept();
  std::string test_string = "tcp test string";
  EXPECT_EQ(IOResult::SUCCEED, client.send(test_string));
  std::string msg;
  delay(100UL);
  EXPECT_EQ(IOResult::SUCCEED, server.recv(msg));
  EXPECT_EQ(msg, test_string);
}

TEST(Tcp, Server) {
  using network::IOResult;

  network::TcpServer server("0.0.0.0", 3333, {}, true);
  server.accept();
  delay(100UL);
  std::string msg;
  delay(100UL);
  while(server.recv(msg) == IOResult::SUCCEED) {
    std::cout << msg << std::endl;
  }
}