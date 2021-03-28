#ifndef NETWORK_TCPSERVER_HPP
#define NETWORK_TCPSERVER_HPP

#include <set>
#include <memory>
#include <unordered_map>
#include <tcp/TcpClient.hpp>
#include <IPort.hpp>

namespace std {
  template<>
  struct hash<sockaddr_in> {
    size_t operator()(const sockaddr_in & addr) const {
      std::string str;
      str += network::Socket::getIp(addr);
      str += ":";
      str += std::to_string(network::Socket::getPort(addr));
      return std::hash<std::string>()(str);
    }
  };
}

namespace network {

  class TcpServer: public ISocketPort {
      using TcpClientPtr = std::unique_ptr<TcpClient>;
      using TcpClientMap = std::unordered_map<sockaddr_in, TcpClientPtr>;

    public:
      TcpServer(const std::string &serverIp,
                uint16_t port,
                const std::set<std::string> &a_clientIp = {},
                bool acceptsNewAddr = false,
                int maxClients = 10);
      void accept();
      IOResult send(const std::string& str) override;
      IOResult recv(std::string &str) override;
      bool connected() override;
      bool connectedTo(const sockaddr_in &addr) override;
      IOResult sendto(const sockaddr_in &addr, const std::string &str) override;
      IOResult recvfrom(const sockaddr_in &addr, std::string &str) override;
      sockaddr_in *getAddr() override;

    private:
      void create();
      void restart();
      bool isKnownAddr(const sockaddr_in &addr);
      void updateClients(TcpClientMap &a_client);

      sockaddr_in m_addr;
      bool m_acceptsNewAddr;
      int m_maxClients;
      TcpClientMap ma_clientPtr;
      std::unique_ptr<Socket> m_serverSocketPtr;
  };

} // end namespace network

#endif //NETWORK_TCPSERVER_HPP
