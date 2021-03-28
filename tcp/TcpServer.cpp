#include <algorithm>
#include "TcpServer.hpp"

using network::TcpServer;
using network::IOResult;

static bool operator==(const sockaddr_in &lhs, const sockaddr_in &rhs) {
  return lhs.sin_addr.s_addr == rhs.sin_addr.s_addr;
}

TcpServer::TcpServer(const std::string &serverIp, uint16_t port,
                     const std::set<std::string> &a_clientIp,
                     bool acceptsNewAddr, int maxClients):
  m_addr(Socket::getAddr(serverIp, port)),
  m_acceptsNewAddr(acceptsNewAddr),
  m_maxClients(maxClients),
  m_serverSocketPtr(std::make_unique<Socket>())
{
  for(auto &ip: a_clientIp) {
    auto ptr = std::make_unique<TcpClient>(ip, port);
    ma_clientPtr[*ptr->getAddr()] = std::move(ptr);
  }
  restart();
}

IOResult TcpServer::send(const std::string &str) {
  auto rc = IOResult::FAILED;
  TcpClientMap a_succeeded;
  for(auto &[addr, client]: ma_clientPtr) {
    rc = client->send(str);
    switch(rc) {
      case IOResult::SUCCEED:
        [[fallthrough]];
      case IOResult::INPROGRESS:
        a_succeeded.emplace(std::make_pair(*client->getAddr(), std::move(client)));
        break;
      case IOResult::FAILED:
        break;
    }
  }
  updateClients(a_succeeded);
  return rc;
}

IOResult TcpServer::recv(std::string &str) {
  auto rc = IOResult::FAILED;
  str.clear();
  std::string temp;
  TcpClientMap a_succeeded;
  for(auto &[addr, client]: ma_clientPtr) {
    rc = client->recv(temp);
    switch(rc) {
      case IOResult::SUCCEED:
        str = temp;
        [[fallthrough]];
      case IOResult::INPROGRESS:
        a_succeeded[*client->getAddr()] = std::move(client);
        break;
      case IOResult::FAILED:
        break;
    }
  }
  updateClients(a_succeeded);
  return rc;
}

bool TcpServer::connected() {
  return !ma_clientPtr.empty();
}

void TcpServer::create() {
  m_serverSocketPtr->create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  // set options
  m_serverSocketPtr->setNagleOpt(false);
  m_serverSocketPtr->setReuseAddrOpt(true);
  m_serverSocketPtr->setNonBlocking(true);
  m_serverSocketPtr->setRecvBufSize();
}

void TcpServer::restart() {
  m_serverSocketPtr->shutdown();
  m_serverSocketPtr->close();
  create();
  if((SOCKET_ERROR == m_serverSocketPtr->bind(m_addr)) ||
     (SOCKET_ERROR == m_serverSocketPtr->listen(m_maxClients))) {
    m_serverSocketPtr->shutdown();
    m_serverSocketPtr->close();
  }
}

void TcpServer::accept() {
  SOCKET handler = m_serverSocketPtr->accept();
  if(INVALID_SOCKET != handler) {
    std::unique_ptr<Socket> clientSocketPtr = std::make_unique<Socket>();
    clientSocketPtr->set(handler);
    auto addr = clientSocketPtr->getPeerAddr();
    bool known = isKnownAddr(addr);
    if(known || m_acceptsNewAddr) {
      clientSocketPtr->setNagleOpt(false);
      clientSocketPtr->setReuseAddrOpt(true);
      clientSocketPtr->setNonBlocking(true);
      clientSocketPtr->setRecvBufSize();
      auto client = std::make_unique<TcpClient>(std::move(clientSocketPtr));
      ma_clientPtr[addr] = std::move(client);
    }
  }
}

bool TcpServer::isKnownAddr(const sockaddr_in &addr) {
  return std::any_of(ma_clientPtr.begin(), ma_clientPtr.end(),
    [&addr](std::pair<const sockaddr_in, TcpClientPtr> &it) {
    return (it.first == addr);
  });
}

void TcpServer::updateClients(TcpClientMap &a_client) {
  ma_clientPtr.clear();
  ma_clientPtr.swap(a_client);
}


bool TcpServer::connectedTo(const sockaddr_in &addr) {
  auto rc = false;
  auto it = ma_clientPtr.find(addr);
  if(it != ma_clientPtr.end()) {
    rc = it->second->connected();
  }
  return rc;
}


IOResult TcpServer::sendto(const sockaddr_in &addr, const std::string &str) {
  auto it = ma_clientPtr.find(addr);
  if(it != ma_clientPtr.end()) {
    return it->second->send(str);
  } else {
    throw std::runtime_error("Unknown address");
  }
}


IOResult TcpServer::recvfrom(const sockaddr_in &addr, std::string &str) {
  auto it = ma_clientPtr.find(addr);
  if(it != ma_clientPtr.end()) {
    return it->second->recv(str);
  } else {
    throw std::runtime_error("Unknown address");
  }
}


sockaddr_in *network::TcpServer::getAddr() {
  return &m_addr;
}
