#include <gtest/gtest.h>
#include <com/ComPort.hpp>
#include <icmp/PingPort.hpp>
#include <tcp/TcpClient.hpp>
#include <tcp/TcpServer.hpp>
#include <udp/UdpClient.hpp>
#include <udp/UdpServer.hpp>

TEST(Interface, IPort) {
  using network::IPort;

  network::ComPort com("/dev/tnt1");
  EXPECT_NE(dynamic_cast<IPort*>(&com), nullptr);
  network::PingPort ping("0.0.0.0");
  EXPECT_NE(dynamic_cast<IPort*>(&ping), nullptr);
  network::TcpClient tcpClient("0.0.0.0", 502);
  EXPECT_NE(dynamic_cast<IPort*>(&tcpClient), nullptr);
  network::TcpServer tcpServer("0.0.0.0", 502);
  EXPECT_NE(dynamic_cast<IPort*>(&tcpServer), nullptr);
  network::UdpClient udpClient("0.0.0.0", 502);
  EXPECT_NE(dynamic_cast<IPort*>(&udpClient), nullptr);
  network::UdpServer udpServer("0.0.0.0", 502);
  EXPECT_NE(dynamic_cast<IPort*>(&udpServer), nullptr);
}