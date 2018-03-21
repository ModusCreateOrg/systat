//
// Created by Michael Schwartz on 3/21/18.
//

#ifndef C_NETWORK_H
#define C_NETWORK_H

#include <map>
#include <string>

struct NetworkStats {
  uint64_t packetsIn;
  uint64_t packetsOut;
  uint64_t bytesIn;
  uint64_t bytesOut;
};

struct Interface {
  std::string name;    // interface name (e.g. en0)
  int flags;
  u_char type;
  uint8_t mac[6];   // mac address
  uint64_t speed;
  NetworkStats stats;
  NetworkStats statsDelta;
public:
  Interface(std::string name, uint8_t *mac, if_msghdr2 *if2m);
  void update(if_data64 *data);
  void print();
};

class Network {
private:
  std::map<std::string, Interface *> interfaces;
public:
  NetworkStats stats;
  NetworkStats statsDelta;

public:
  Network();
  void update();
  void each(void (*f)(Interface *));
  void print();
};

extern Network network;

#endif //C_NETWORK_H
