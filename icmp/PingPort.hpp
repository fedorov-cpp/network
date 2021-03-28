#ifndef NETWORK_PINGPORT_HPP
#define NETWORK_PINGPORT_HPP

#include <chrono>
#include <memory>
#include <netinet/ip_icmp.h>
#include <IPort.hpp>
#include <socket/Socket.hpp>

namespace network {

  // @warning: on Linux to allow root to use icmp sockets, run (by default no user have permission to ICMP):
  // sysctl -w net.ipv4.ping_group_range="0 0"
  // For non-root:
  // sudo sysctl -w net.ipv4.ping_group_range="0 $(id -u $(whoami))"

  class PingPort: public IPort {
    public:
      explicit PingPort(const std::string &ip,
                        size_t timeout_ms=1000UL);
      bool connected() override;
      IOResult send(const std::string &str) override;
      IOResult recv(std::string &str) override;

    private:
      bool connectedTo(const sockaddr_in &addr) override;
      IOResult sendto(const sockaddr_in &addr, const std::string &str) override;
      IOResult recvfrom(const sockaddr_in &addr, std::string &str) override;
      void restart();

      Socket m_socket;
      sockaddr_in m_addr;
      icmphdr m_hdr;
      u_int16_t m_sequence;
      size_t m_timeout_ms;
      size_t m_interval;
      bool m_connected;
      std::chrono::high_resolution_clock::time_point m_start_point;
      constexpr static const char * const PING_REQ_STR = "are you alive?";
      constexpr static const size_t PING_REQ_STR_SIZE = 15UL;
  };

} // end namespace network


#endif //NETWORK_PINGPORT_HPP
