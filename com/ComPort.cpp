#include <system_error>
#include <cstring>
#include <sys/epoll.h>

#include "ComPort.hpp"

using network::ComPort;
using network::IOResult;

ComPort::ComPort(const std::string& port, int rcv_timeout_ms,
                 network::ComPort::BaudRate rate,
                 network::ComPort::DataBits db, network::ComPort::Parity parity,
                 network::ComPort::StopBits sb, size_t rcvBufSize,
                 network::ComPort::Mode mode):
  m_port(port),
  m_rcvBufSize(rcvBufSize),
  m_rcv_timeout_ms(rcv_timeout_ms),
  m_mode(mode),
  m_rate(rate),
  m_db(db),
  m_parity(parity),
  m_sb(sb),
  m_handle(-1)
{
  openPort();
  setup();
}

IOResult ComPort::send(const std::string &str) {
  auto rc = IOResult::FAILED;
  auto bytes = ::write(m_handle, reinterpret_cast<const void*>(str.c_str()), str.size());
  if(-1 == bytes) {
    throw std::system_error(errno, std::generic_category(), strerror(errno));
  } else if(bytes == static_cast<ssize_t>(str.size())) {
    rc = IOResult::SUCCEED;
  }
  return rc;
}

IOResult ComPort::recv(std::string &str) {
  auto rc = IOResult::FAILED;
  str.clear();
  int epollfd = ::epoll_create1(0);
  if(-1 == epollfd) {
    throw std::system_error(errno, std::generic_category(), strerror(errno));
  }
  epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = m_handle;
  if(-1 == ::epoll_ctl(epollfd, EPOLL_CTL_ADD, m_handle, &ev)) {
    throw std::system_error(errno, std::generic_category(), strerror(errno));
  }
  epoll_event events[MAX_EVENTS];
  int n = ::epoll_wait(epollfd, events, MAX_EVENTS, m_rcv_timeout_ms);
  if(-1 == n) {
    throw std::system_error(errno, std::generic_category(), strerror(errno));
  }
  for(int i = 0; i < n; ++i) {
    if(events[i].data.fd == m_handle) {
      char *buf = new char[m_rcvBufSize];
      auto bytes = ::read(m_handle, reinterpret_cast<void*>(buf), m_rcvBufSize);
      if(bytes >= 0) {
        str.append(buf, static_cast<size_t>(bytes));
        rc = IOResult::SUCCEED;
      } else {
        ::tcflush(m_handle, TCIOFLUSH);
      }
      delete[] buf;
    }
  }
  return rc;
}

std::string ComPort::getPort() {
  return m_port;
}

void ComPort::restart() {
    closePort();
    openPort();
    setup();
}

void ComPort::openPort() {
  m_handle = ::open(m_port.c_str(), O_RDWR|O_NOCTTY|O_NDELAY);
  if(m_handle < 0) {
    throw std::system_error(errno, std::generic_category(), strerror(errno));
  }
}

void ComPort::closePort() {
  if(::close(m_handle) < 0) {
    throw std::system_error(errno, std::generic_category(), strerror(errno));
  }
}

void ComPort::setup() {
  termios tty;
  ::tcgetattr(m_handle, &tty);

  // mode
  // baud rate
  ::cfsetispeed(&tty, static_cast<speed_t>(m_rate));
  ::cfsetospeed(&tty, static_cast<speed_t>(m_rate));
  // data bits
  tty.c_cflag &= ~(PARENB|PARODD|CSTOPB|CSIZE);
  tty.c_cflag |= static_cast<tcflag_t>(m_db);
  // parity
  tty.c_cflag |= static_cast<tcflag_t>(m_parity);
  // stop bits
  tty.c_cflag |= static_cast<tcflag_t>(m_sb);

  tty.c_cflag |= (CLOCAL|CREAD);
  tty.c_lflag &= ~(CRTSCTS|ICANON|ECHO|ECHOE|ISIG);
  tty.c_iflag &= ~(INLCR|ICRNL|IXOFF|IXON);
  tty.c_oflag &= OPOST;
  tty.c_oflag &= ~(OLCUC|ONLCR|OCRNL);

  if(::tcsetattr(m_handle, TCSANOW, &tty) < 0) {
    throw std::system_error(errno, std::generic_category(), strerror(errno));
  }
  ::fcntl(m_handle, F_SETFL, 0);
}


bool ComPort::connected() {
  throw std::logic_error("There is no way for serial port to check if you're connected.");
}


bool ComPort::connectedTo([[maybe_unused]] const sockaddr_in &addr) {
  throw std::logic_error("There is no way for serial port to check if you're connected.");
}


IOResult ComPort::sendto([[maybe_unused]] const sockaddr_in &addr, [[maybe_unused]] const std::string &str) {
  throw std::logic_error("There is no way for serial port to send to address.");
}


IOResult ComPort::recvfrom([[maybe_unused]] const sockaddr_in &addr, [[maybe_unused]] std::string &str) {
  throw std::logic_error("There is no way for serial port to recv from address.");
}
