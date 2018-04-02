#ifndef C_NETWORK_H
#define C_NETWORK_H

#include <map>
#include <string>

struct Interface {
  std::string name;    // interface name (e.g. en0)
  int         flags;
  u_char      type;
  uint8_t     mac[6];   // mac address
  uint64_t    speed;
  uint64_t    packetsIn;
  uint64_t    packetsOut;
  uint64_t    bytesIn;
  uint64_t    bytesOut;
public:
  void diff(Interface *newer, Interface *older);
};

class Network {
private:
  std::map<std::string, Interface *> last, current, delta;

public:
  Network();

protected:
  void read(std::map<std::string, Interface *> &m);

  void copy(std::map<std::string, Interface *> &src, std::map<std::string, Interface *> &dst);

public:
  void update();

  // print network stats, unless test is set,  return # lines (would be) printed
  uint16_t print(bool test);
};

extern Network network;

#endif //C_NETWORK_H
