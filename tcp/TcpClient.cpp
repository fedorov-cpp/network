#include <stdexcept>

#include "TcpClient.hpp"

using network::TcpClient;
using network::IOResult;

TcpClient::TcpClient(const std::string &serverIp, uint16_t port):
  m_addr(Socket::getAddr(serverIp, port)),
  m_connected(false),
  m_socketPtr(std::make_unique<Socket>())
{
  restart();
}

TcpClient::TcpClient(std::unique_ptr<Socket> &&rhs):
  m_connected(false)
{
  m_socketPtr.swap(rhs);
  m_addr = m_socketPtr->getPeerAddr();
}

IOResult TcpClient::send(const std::string &str) {
  auto rc = IOResult::FAILED;
  auto bytes = m_socketPtr->send(str, MSG_NOSIGNAL);
  if(bytes == static_cast<ssize_t>(str.size())) {
    rc = IOResult::SUCCEED;
  } else if(0 == bytes) {
    // peer shutdown
    restart();
  } else if((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
    // error
    restart();
  } else {
    // non blocking socket not ready
    rc = IOResult::INPROGRESS;
  }
  return rc;
}

IOResult TcpClient::recv(std::string &str) {
  auto rc = IOResult::FAILED;
  auto bytes = m_socketPtr->recv(str, MSG_NOSIGNAL);
  if(bytes > 0) {
    rc = IOResult::SUCCEED;
  } else if(0 == bytes) {
    // peer shutdown
    restart();
  } else if((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
    // error
    restart();
  } else {
    // non blocking socket not ready
    rc = IOResult::INPROGRESS;
  }
  return rc;
}

void TcpClient::restart() {
  m_socketPtr->shutdown();
  m_socketPtr->close();
  m_socketPtr->create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  setOptions();
  connect();
}

void TcpClient::setOptions() {
  m_socketPtr->setNagleOpt(false);
  m_socketPtr->setReuseAddrOpt(true);
  m_socketPtr->setNonBlocking(true);
  network::TcpKeepAlive val;
  val.m_isOn = 1;
  val.m_cnt = TCP_KEEPALIVE_CNT;
  val.m_idle = MIN_TIMEOUT;
  val.m_intvl = MIN_TIMEOUT;
  m_socketPtr->setKeepAliveOpt(val);
}

void TcpClient::connect() {
  auto rc = m_socketPtr->connect(m_addr);
  m_connected = false;
  if((0 != rc) && (errno != EINPROGRESS)) {
    m_socketPtr->shutdown();
    m_socketPtr->close();
  } else {
    m_connected = true;
  }
}


bool TcpClient::connected() {
  return m_connected;
}


bool TcpClient::connectedTo([[maybe_unused]] const sockaddr_in &addr) {
  throw std::logic_error("TcpClient doesn't support multiple addresses.");
}


IOResult TcpClient::sendto([[maybe_unused]] const sockaddr_in &addr, [[maybe_unused]] const std::string &str) {
  throw std::logic_error("TcpClient doesn't support multiple addresses.");
}


IOResult TcpClient::recvfrom([[maybe_unused]] const sockaddr_in &addr, [[maybe_unused]] std::string &str) {
  throw std::logic_error("TcpClient doesn't support multiple addresses.");
}


sockaddr_in *network::TcpClient::getAddr() {
  return &m_addr;
}
