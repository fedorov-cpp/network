#ifndef NETWORK_TCPCLIENT_HPP
#define NETWORK_TCPCLIENT_HPP

#include <memory>
#include <socket/Socket.hpp>
#include <IPort.hpp>

namespace network {

  /**
   * 1. Client would be restarted and reconnected if was not ready
   *    to write or write error occurred or peer shutdown
   * 2. Client would NOT be restarted and reconnected automatically if was not
   *    ready to read, but would be restarted on receive error or peer shutdown
   * 3. Client keeps connection automatically
   */
  class TcpClient: public ISocketPort {
    public:
      static constexpr int TCP_KEEPALIVE_CNT = 1;
      static constexpr int MIN_TIMEOUT = 10;

      TcpClient(const std::string &serverIp, uint16_t port);
      explicit TcpClient(std::unique_ptr<Socket> &&rhs);
      bool connected() override;
      IOResult send(const std::string &str) override;
      IOResult recv(std::string &str) override;
      sockaddr_in *getAddr() override;

    private:
      bool connectedTo(const sockaddr_in &addr) override;
      IOResult sendto(const sockaddr_in &addr, const std::string &str) override;
      IOResult recvfrom(const sockaddr_in &addr, std::string &str) override;

      void restart();
      void setOptions();
      void connect();

      sockaddr_in m_addr;
      bool m_connected;
      std::unique_ptr<Socket> m_socketPtr;

  };

} // end namespace network

#endif //NETWORK_TCPCLIENT_HPP
