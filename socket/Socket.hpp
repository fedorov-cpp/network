#ifndef NETWORK_SOCKET_HPP
#define NETWORK_SOCKET_HPP

#include <cstdint>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

namespace network {

  struct TcpKeepAlive {
    int m_isOn;
    int m_idle;
    int m_intvl;
    int m_cnt;
  };

  using SOCKET = int;
  constexpr int INVALID_SOCKET = INT32_MAX;
  constexpr int SOCKET_ERROR = -1;
  constexpr int RECV_BUF_SIZE = 65535;
  constexpr int SEND_BUF_SIZE = 65535;

  class Socket {
    public:
      SOCKET get();
      void   set(SOCKET socket);

      ssize_t send(const std::string &str, int flags);
      ssize_t sendto(const sockaddr_in &addr, const std::string &str, int flags);
      ssize_t recv(std::string &str, int flags);
      ssize_t recvfrom(sockaddr_in &addr, std::string &str, int flags);

      SOCKET create(int af, int type, int protocol);
      int    shutdown(int flag = SHUT_RDWR);
      int    close();
      int    bind(const sockaddr_in &addr);
      int    connect(const sockaddr_in &addr);
      int    listen(int maxClientsNum);
      SOCKET accept();

      bool setNagleOpt(bool state);
      bool setReuseAddrOpt(bool state);
      bool setKeepAliveOpt(const TcpKeepAlive &state);
      bool setNonBlocking(bool state);
      bool setBroadcast(bool state);
      bool setRecvBufSize(int size = RECV_BUF_SIZE);
      bool setSendBufSize(int size = SEND_BUF_SIZE);

      sockaddr_in getPeerAddr();
      bool        isNoError();

      static sockaddr_in getAddr(const std::string &ip, uint16_t port) noexcept;
      static std::string getIp(const sockaddr_in &addr);
      static uint16_t getPort(const sockaddr_in &addr);

    private:
      bool setOption(int level, int opt, const void *val, socklen_t len);

      SOCKET m_socket = SOCKET_ERROR;
  };

} // end namespace network

#endif //NETWORK_SOCKET_HPP
