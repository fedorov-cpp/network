#include <cstring>
#include <stdexcept>

#include "UdpClient.hpp"

using network::UdpClient;
using network::IOResult;

UdpClient::UdpClient(const std::string &serverIp, uint16_t port):
  m_addr(Socket::getAddr(serverIp, port)),
  m_socketPtr(std::make_unique<Socket>())
{
  m_broadcasted = (".255" == serverIp.substr(serverIp.size()-4));
  restart();
}


IOResult UdpClient::send(const std::string &str) {
  auto rc = IOResult::SUCCEED;
  auto bytes = m_socketPtr->sendto(m_addr, str, MSG_NOSIGNAL);
  if(bytes < 0) {
    rc = IOResult::FAILED;
    if((errno != EINPROGRESS) && (errno != EWOULDBLOCK)) {
      restart();
    }
  }
  return rc;
}


IOResult UdpClient::recv(std::string &str) {
  auto rc = IOResult::FAILED;
  sockaddr_in addr;
  ::memset(&addr, 0, sizeof(sockaddr_in));
  auto bytes = m_socketPtr->recvfrom(addr, str, MSG_NOSIGNAL);
  if(bytes >= 0) {
    rc = IOResult::SUCCEED;
  } else if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
    restart();
  }
  return rc;
}


void UdpClient::restart() {
  m_socketPtr->close();
  create();
}

void UdpClient::create() {
  m_socketPtr->create(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  // set options
  m_socketPtr->setReuseAddrOpt(true);
  m_socketPtr->setNonBlocking(true);
  m_socketPtr->setRecvBufSize();
  m_socketPtr->setSendBufSize();
  m_socketPtr->setBroadcast(m_broadcasted);
}


bool UdpClient::connected() {
  throw std::logic_error("There is no way for UdpClient to check if you're connected.");
}


bool UdpClient::connectedTo([[maybe_unused]] const sockaddr_in &addr) {
  throw std::logic_error("There is no way for UdpClient to check if you're connected.");
}


IOResult UdpClient::sendto([[maybe_unused]] const sockaddr_in &addr, [[maybe_unused]] const std::string &str) {
  throw std::logic_error("UdpClient doesn't support multiple addresses.");
}


IOResult UdpClient::recvfrom([[maybe_unused]] const sockaddr_in &addr, [[maybe_unused]] std::string &str) {
  throw std::logic_error("UdpClient doesn't support multiple addresses.");
}


sockaddr_in *UdpClient::getAddr() {
  return &m_addr;
}
