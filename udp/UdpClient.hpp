#ifndef NETWORK_UDPCLIENT_HPP
#define NETWORK_UDPCLIENT_HPP

#include <memory>
#include <socket/Socket.hpp>
#include <IPort.hpp>

namespace network {

  class UdpClient: public ISocketPort {
    public:
      UdpClient(const std::string &serverIp,
                uint16_t port);
      IOResult send(const std::string &str) override;
      IOResult recv(std::string &str) override;
      sockaddr_in *getAddr() override;

    private:
      bool connected() override;
      bool connectedTo(const sockaddr_in &addr) override;
      IOResult sendto(const sockaddr_in &addr, const std::string &str) override;
      IOResult recvfrom(const sockaddr_in &addr, std::string &str) override;
      void restart();
      void create();

      sockaddr_in m_addr;
      bool m_broadcasted;
      std::unique_ptr<Socket> m_socketPtr;
  };

} // end namespace network

#endif //NETWORK_UDPCLIENT_HPP
