#ifndef NETWORK_UDPSERVER_HPP
#define NETWORK_UDPSERVER_HPP

#include <set>
#include <list>
#include <memory>
#include <socket/Socket.hpp>
#include <IPort.hpp>

namespace network {

  class UdpServer: public ISocketPort {
    public:
      UdpServer(const std::string &serverIp,
                uint16_t port,
                size_t maxClients = UINT64_MAX,
                const std::set<std::string> &a_clientIp = {},
                bool sendingToNewAddr = false,
                bool receivingFromNewAddr = false);
      IOResult send(const std::string &str) override;
      IOResult recv(std::string &str) override;
      IOResult sendto(const sockaddr_in &addr, const std::string &str) override;
      IOResult recvfrom(const sockaddr_in &addr, std::string &str) override;
      sockaddr_in *getAddr() override;

    private:
      bool connected() override;
      bool connectedTo(const sockaddr_in &addr) override;

      void restart();
      void create();
      bool isKnownAddr(sockaddr_in &addr);

      sockaddr_in m_addr;
      size_t m_maxClients;
      std::list<sockaddr_in> ma_clientAddr;
      bool m_sendingToNewAddr;
      bool m_receivingFromNewAddr;
      bool m_broadcasted;
      std::unique_ptr<Socket> m_socketPtr;
  };

} // end namespace network

#endif //NETWORK_UDPSERVER_HPP
