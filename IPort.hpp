#ifndef NETWORK_IPORT_HPP
#define NETWORK_IPORT_HPP

#include <string>
#include <netinet/in.h>

namespace network {

  enum class IOResult: int {
    FAILED = -1,
    SUCCEED = 1,
    INPROGRESS
  };

  class IPort {
    public:
      virtual ~IPort() = default;
      virtual bool connected() = 0;
      virtual bool connectedTo(const sockaddr_in &addr) = 0;
      virtual IOResult send(const std::string &str) = 0;
      virtual IOResult recv(std::string &str) = 0;
      virtual IOResult sendto(const sockaddr_in &addr, const std::string &str) = 0;
      virtual IOResult recvfrom(const sockaddr_in &addr, std::string &str) = 0;
  };

  class ISocketPort: public IPort {
    public:
      ~ISocketPort() override = default;
      virtual sockaddr_in *getAddr() = 0;
  };

} // end namespace network

#endif //NETWORK_IPORT_HPP
