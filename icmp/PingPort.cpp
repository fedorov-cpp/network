#include <stdexcept>
#include <cstring>
#include <netdb.h>
#include "PingPort.hpp"

using network::PingPort;
using network::Socket;
using network::IOResult;

namespace {
  std::string _url_to_ip(const std::string &url) {
    std::string ret;
    hostent *he;
    in_addr **addr_list;

    he = gethostbyname(url.c_str());
    if(nullptr == he) {
      throw std::runtime_error{
        std::string(__FUNCTION__) + ": got invalid url=" + url
      };
    }

    addr_list = reinterpret_cast<in_addr **>(he->h_addr_list);
    if(addr_list[0]) {
      ret = std::string(inet_ntoa(*addr_list[0]));
    }
    return ret;
  }
}

PingPort::PingPort(const std::string &ip,
                   size_t timeout_ms):
  m_sequence(0U),
  m_timeout_ms(timeout_ms),
  m_interval(timeout_ms/2),
  m_connected(false),
  m_start_point(std::chrono::high_resolution_clock::now())
{
  ::memset(&m_addr, 0, sizeof(sockaddr_in));
  ::memset(&m_hdr, 0, sizeof(icmphdr));
  m_addr.sin_family = AF_INET;
  in_addr dst;
  ::memset(&dst, 0, sizeof(in_addr));
  auto _ip = _url_to_ip(ip);
  if (0 == inet_aton(_ip.c_str(), &dst)) {
    throw std::runtime_error{
      std::string(__FUNCTION__)+": got invalid ip="+_ip
    };
  }
  m_addr.sin_addr = dst;

  m_hdr.type = ICMP_ECHO;
  m_hdr.un.echo.id = 333;

  restart();
  send(std::string(PING_REQ_STR, PING_REQ_STR_SIZE));
}

void PingPort::restart() {
  m_socket.close();
  m_socket.create(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
  m_socket.setNonBlocking(true);
}

IOResult PingPort::send(const std::string &str) {
  m_hdr.un.echo.sequence = m_sequence++;
  std::string data;
  data.append(reinterpret_cast<char*>(&m_hdr), sizeof(m_hdr));
  data.append(str);
  auto bytes = m_socket.sendto(m_addr, data, MSG_NOSIGNAL);
  IOResult rc = (bytes > 0) ? IOResult::SUCCEED : IOResult::FAILED;
  if (IOResult::FAILED == rc) restart();
  return rc;
}

IOResult PingPort::recv(std::string &str) {
  IOResult rc = IOResult::FAILED;
  std::string data;
  auto bytes = m_socket.recv(str, 0);
  icmphdr recvHdr;
  ::memset(&recvHdr, 0, sizeof(icmphdr));
  if ((bytes > 0) && (static_cast<size_t>(bytes) >= sizeof(recvHdr))) {
    memcpy(&recvHdr, data.c_str(), sizeof(recvHdr));
    if (ICMP_ECHOREPLY == recvHdr.type) {
      rc = IOResult::SUCCEED;
    }
  }
  return rc;
}

bool PingPort::connected() {
  std::chrono::high_resolution_clock::time_point temp =
    std::chrono::high_resolution_clock::now();
  std::chrono::milliseconds passed =
    std::chrono::duration_cast<std::chrono::milliseconds>(temp - m_start_point);
  if (static_cast<size_t>(passed.count()) > m_interval) {
    std::string str;
    m_connected = (IOResult::SUCCEED == recv(str));
    m_start_point = temp;
    send(std::string(PING_REQ_STR, PING_REQ_STR_SIZE));
  }
  return m_connected;
}


bool PingPort::connectedTo([[maybe_unused]] const sockaddr_in &addr) {
  throw std::logic_error("PingPort doesn't support multiple addresses.");
}

IOResult PingPort::sendto([[maybe_unused]] const sockaddr_in &addr, [[maybe_unused]] const std::string &str) {
  throw std::logic_error("PingPort doesn't support multiple addresses.");
}


IOResult PingPort::recvfrom([[maybe_unused]] const sockaddr_in &addr, [[maybe_unused]] std::string &str) {
  throw std::logic_error("PingPort doesn't support multiple addresses.");
}
