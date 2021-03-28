#include <cstring>
#include "Socket.hpp"

using network::Socket;
using network::SOCKET;
using network::TcpKeepAlive;

SOCKET Socket::get() {
  return m_socket;
}

void Socket::set(network::SOCKET socket) {
  m_socket = socket;
}

ssize_t Socket::send(const std::string &str, int flags) {
  return ::send(m_socket, reinterpret_cast<const void*>(str.c_str()), str.size(), flags);
}

ssize_t Socket::sendto(const sockaddr_in &addr, const std::string &str, int flags) {
  return ::sendto(m_socket, reinterpret_cast<const void*>(str.c_str()), str.size(), flags,
                  reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in));
}

ssize_t Socket::recv(std::string &str, int flags) {
  str.clear();
  char buf[RECV_BUF_SIZE]{0};
  auto result = ::recv(m_socket, reinterpret_cast<void*>(buf), RECV_BUF_SIZE, flags);
  if(result > 0) {
    str.append(buf, static_cast<size_t>(result));
  }
  return result;
}

ssize_t Socket::recvfrom(sockaddr_in &addr, std::string &str, int flags) {
  str.clear();
  char buf[RECV_BUF_SIZE]{0};
  ::memset(&addr, 0, sizeof(sockaddr_in));
  socklen_t addrLen = sizeof(sockaddr_in);
  auto result = ::recvfrom(m_socket, reinterpret_cast<void*>(buf), RECV_BUF_SIZE, flags,
                           reinterpret_cast<sockaddr*>(&addr), &addrLen);
  if(result > 0) {
    str.append(buf, static_cast<size_t>(result));
  }
  return result;
}

SOCKET Socket::create(int af, int type, int protocol) {
  m_socket = ::socket(af, type, protocol);
  return m_socket;
}

int Socket::shutdown(int flag) {
  return ::shutdown(m_socket, flag);
}

int Socket::close() {
  int ret = ::close(m_socket);
  m_socket = INVALID_SOCKET;
  return ret;
}

int Socket::bind(const sockaddr_in &addr) {
  return ::bind(m_socket, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in));
}

int Socket::connect(const sockaddr_in &addr) {
  return ::connect(m_socket, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in));
}

int Socket::listen(int maxClientsNum) {
  return ::listen(m_socket, maxClientsNum);
}

SOCKET Socket::accept() {
  sockaddr_in addr;
  ::memset(&addr, 0, sizeof(sockaddr_in));
  socklen_t addrLen = sizeof(addr);
  SOCKET ret = ::accept(m_socket, reinterpret_cast<sockaddr*>(&addr), &addrLen);
  return ret;
}

bool Socket::setNagleOpt(bool state) {
  auto value = static_cast<int>(state);
  return setOption(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const void*>(&value), sizeof(int));
}

bool Socket::setReuseAddrOpt(bool state) {
  auto value = static_cast<int>(state);
  return setOption(SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const void*>(&value), sizeof(int));
}

bool Socket::setKeepAliveOpt(const TcpKeepAlive &state) {
  bool ret = true;
  ret &= setOption(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const void*>(&state.m_isOn), sizeof(int));
  if(state.m_isOn) {
    ret &= setOption(SOL_TCP, TCP_KEEPIDLE, reinterpret_cast<const void*>(&state.m_idle), sizeof(int));
    ret &= setOption(SOL_TCP, TCP_KEEPINTVL, reinterpret_cast<const void*>(&state.m_intvl), sizeof(int));
    ret &= setOption(SOL_TCP, TCP_KEEPCNT, reinterpret_cast<const void*>(&state.m_cnt), sizeof(int));
  }
  return ret;
}

bool Socket::setNonBlocking(bool state) {
  bool ret = true;
  int fd = ::fcntl(m_socket, F_GETFL); // get file descriptor
  int flags = state ? (fd | O_NONBLOCK) : (fd & (~O_NONBLOCK));
  ret &= (SOCKET_ERROR != ::fcntl(m_socket, F_SETFL, flags));
  return ret;
}

bool Socket::setBroadcast(bool state) {
  auto value = static_cast<int>(state);
  return setOption(SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const void*>(&value), sizeof(int));
}

bool Socket::setRecvBufSize(int size) {
  return setOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const void*>(&size), sizeof(int));
}

bool Socket::setSendBufSize(int size) {
  return setOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const void*>(&size), sizeof(int));
}

sockaddr_in Socket::getPeerAddr() {
  sockaddr_in addr;
  ::memset(&addr, 0, sizeof(sockaddr_in));
  socklen_t addrLen = sizeof(sockaddr_in);
  if(SOCKET_ERROR == ::getpeername(m_socket, reinterpret_cast<sockaddr*>(&addr), &addrLen)) {
    ::memset(&addr, 0, sizeof(sockaddr_in));
  }
  return addr;
}

bool Socket::isNoError() {
  int error;
  socklen_t errorLen = sizeof(int);
  bool ret = (SOCKET_ERROR != ::getsockopt(m_socket, SOL_SOCKET, SO_ERROR,
                                           reinterpret_cast<void*>(&error),
                                           &errorLen));
  ret &= (0 == error);
  return ret;
}

sockaddr_in Socket::getAddr(const std::string &ip, uint16_t port) noexcept {
  sockaddr_in addr;
  ::memset(&addr, 0, sizeof(sockaddr_in));
  addr.sin_family = AF_INET;
  int result = ::inet_pton(AF_INET, ip.c_str(), &(addr.sin_addr));
  addr.sin_port = htons(port);
  if(result < 1) {
    ::memset(&addr, 0, sizeof(sockaddr_in));
  }
  return addr;
}

std::string Socket::getIp(const sockaddr_in &addr) {
  return std::string(::inet_ntoa(addr.sin_addr));
}

uint16_t Socket::getPort(const sockaddr_in &addr) {
  return ntohs(addr.sin_port);
}

bool Socket::setOption(int level, int opt, const void *val, socklen_t len) {
  return (0 == ::setsockopt(m_socket, level, opt, val, len));
}