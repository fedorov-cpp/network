#ifndef NETWORK_COMPORT_HPP
#define NETWORK_COMPORT_HPP

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string>
#include <IPort.hpp>

namespace network {

  class ComPort: public IPort {
    public:
      static constexpr int MAX_EVENTS = 10;

      enum class Mode {
          RS232,
          RS422,
          RS485_2W,
          RS485_4W
      };
      enum class BaudRate {
          BR4800 = B4800,
          BR9600 = B9600,
          BR19200 = B19200,
          BR38400 = B38400,
          BR57600 = B57600,
          BR115200 = B115200
      };
      enum class DataBits {
          DB5 = CS5,
          DB6 = CS6,
          DB7 = CS7,
          DB8 = CS8
      };
      enum class Parity {
          NONE = 0,
          EVEN = PARENB,
          ODD = (PARENB|PARODD)
      };
      enum class StopBits {
          SB1 = 0,
          SB2 = CSTOPB
      };

      explicit ComPort(const std::string& port,
                       int rcv_timeout_ms = 500,
                       BaudRate rate = BaudRate::BR9600,
                       DataBits db = DataBits::DB8,
                       Parity parity = Parity::NONE,
                       StopBits sb = StopBits::SB1,
                       size_t rcvBufSize = 512UL,
                       Mode mode = Mode::RS422);
      IOResult send(const std::string &str) override;
      IOResult recv(std::string& str) override;
      std::string getPort();

    private:
      bool connected() override;
      bool connectedTo(const sockaddr_in &addr) override;
      IOResult sendto(const sockaddr_in &addr, const std::string &str) override;
      IOResult recvfrom(const sockaddr_in &addr, std::string &str) override;
      void restart();
      void openPort();
      void closePort();
      void setup();

      std::string m_port;
      size_t m_rcvBufSize;
      int m_rcv_timeout_ms;
      Mode m_mode;
      BaudRate m_rate;
      DataBits m_db;
      Parity m_parity;
      StopBits m_sb;
      int m_handle;
  };

} // end namespace network

#endif //NETWORK_COMPORT_HPP
