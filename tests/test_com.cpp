#include <gtest/gtest.h>
#include <com/ComPort.hpp>

/**
 * To successfully run this test please install tty0tty software:
 * https://github.com/freemed/tty0tty
 * install module tty0tty.ko:
 * sudo cp tty0tty.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
 * sudo depmod
 * sudo modprobe tty0tty
 * and give permissions to ports:
 * sudo chmod 666 /dev/tnt*
 */
TEST(Com, ComPort_loopback) {
  using network::ComPort;
  int remote = ::open("/dev/tnt1", O_RDWR);
  EXPECT_GE(remote, 0);
  ComPort port("/dev/tnt0", 500);
  const std::string msg = "Hello!";
  // send
  port.send(msg);
  char buf[32]{0};
  EXPECT_EQ(::read(remote, reinterpret_cast<void*>(buf), 32), msg.size());
  // recv
  EXPECT_EQ(::write(remote, reinterpret_cast<const void*>(msg.c_str()), msg.size()), msg.size());
  std::string answer;
  port.recv(answer);
  EXPECT_EQ(answer, msg);
  ::close(remote);
}