#include <algorithm>
#include <cstring>
#include <stdexcept>

#include "UdpServer.hpp"

using network::UdpServer;
using network::IOResult;

namespace {
  bool _is_any_ip_broadcast(const std::set<std::string> &a_ip) {
    return std::any_of(a_ip.begin(), a_ip.end(),
      [](const std::string &ip) {
        return (".255" == ip.substr(ip.size()-4));
    });
  }

  bool operator==(const sockaddr_in &lhs, const sockaddr_in &rhs) {
    return lhs.sin_addr.s_addr == rhs.sin_addr.s_addr;
  }
}

UdpServer::UdpServer(const std::string &serverIp, uint16_t port,
                     size_t maxClients, const std::set<std::string> &a_clientIp,
                     bool sendingToNewAddr, bool receivingFromNewAddr):
  m_addr(Socket::getAddr(serverIp, port)),
  m_maxClients(maxClients),
  m_sendingToNewAddr(sendingToNewAddr),
  m_receivingFromNewAddr(receivingFromNewAddr),
  m_broadcasted(_is_any_ip_broadcast(a_clientIp)),
  m_socketPtr(std::make_unique<Socket>())
{
  if(!m_sendingToNewAddr) {
    for(auto &ip: a_clientIp) {
      ma_clientAddr.emplace_back(Socket::getAddr(ip, port));
    }
  }
  restart();
}


IOResult UdpServer::send(const std::string &str) {
  auto rc = !ma_clientAddr.empty() ? IOResult::SUCCEED : IOResult::FAILED;
  if(IOResult::SUCCEED == rc) {
    size_t failed = 0UL;
    for(auto &addr: ma_clientAddr) {
      if(IOResult::FAILED == this->sendto(addr, str)) {
        ++failed;
      }
    }
    if(failed == ma_clientAddr.size() && (errno != EINPROGRESS) && (errno != EWOULDBLOCK)) {
      rc = IOResult::FAILED;
      restart();
    }
  }
  return rc;
}


IOResult UdpServer::recv(std::string &str) {
  auto rc = IOResult::FAILED;
  sockaddr_in addr;
  auto bytes = m_socketPtr->recvfrom(addr, str, MSG_NOSIGNAL);
  if(bytes >= 0) {
    if(isKnownAddr(addr)) {
      rc = IOResult::SUCCEED;
    } else {
      if(m_sendingToNewAddr && (ma_clientAddr.size() < m_maxClients)) {
        ma_clientAddr.emplace_back(addr);
      }
      if(m_receivingFromNewAddr) {
        rc = IOResult::SUCCEED;
      }
    }
  } else if((errno != EINPROGRESS) && (errno != EWOULDBLOCK)) {
    restart();
  }
  return rc;
}


void UdpServer::restart() {
 m_socketPtr->close();
 create();
 if(SOCKET_ERROR == m_socketPtr->bind(m_addr)) {
   m_socketPtr->close();
 }
}


void UdpServer::create() {
  m_socketPtr->create(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  // set options
  m_socketPtr->setReuseAddrOpt(true);
  m_socketPtr->setNonBlocking(true);
  m_socketPtr->setRecvBufSize();
  m_socketPtr->setSendBufSize();
}


bool UdpServer::isKnownAddr(sockaddr_in &addr) {
  return std::any_of(ma_clientAddr.begin(), ma_clientAddr.end(),
    [&addr](const sockaddr_in &it) {
      return (it.sin_addr.s_addr == addr.sin_addr.s_addr) &&
             (it.sin_port == addr.sin_port);
  });
}


bool UdpServer::connected() {
  throw std::logic_error("There is no way for UdpServer to check if you're connected.");
}


bool UdpServer::connectedTo([[maybe_unused]] const sockaddr_in &addr) {
  throw std::logic_error("There is no way for UdpServer to check if you're connected.");
}


IOResult UdpServer::sendto(const sockaddr_in &addr, const std::string &str) {
  auto target = addr;
  auto bytes = m_socketPtr->sendto(target, str, MSG_NOSIGNAL);
  return (bytes == static_cast<ssize_t>(str.size())) ? IOResult::SUCCEED : IOResult::FAILED;
}


IOResult UdpServer::recvfrom(const sockaddr_in &addr, std::string &str) {
  for(auto &client: ma_clientAddr) {
    if(client == addr) {
      auto bytes = m_socketPtr->recvfrom(client, str, MSG_NOSIGNAL);
      return (bytes > 0) ? IOResult::SUCCEED : IOResult::FAILED;
    }
  }
  throw std::runtime_error("Unknown address");
}


sockaddr_in *UdpServer::getAddr() {
  return &m_addr;
}
