/*
 * systat for MacOS
 *
 * Programmed by Mike Schwartz <mike@moduscreate.com>
 *
 * Command line tool that refreshes the terminal/console window each second,
 * showing uptime, load average, CPU usage/stats, Memory/Swap usage, Disk
 * Activity (per drive/device), Virtual Memory activity (paging/swapping), and
 * Network traffic (per interface).
 *
 * Run this on a busy system and you can diagnose if:
 * 1) System is CPU bound
 * 2) System is RAM bound
 * 3) System is Disk bound
 * 4) System is Paging/Swapping heavily
 * 5) System is Network bound
 *
 * To exit, hit ^C.
 *
 * This file is based upon httpL//github.com/sklinkert/mac-iostat
 * which is based upon Apple's iostat.c
 * which is based upon BSD's iostat.c
 *
 * See copyright info in iostat.txt and original iostat.c included in this repo.
 */
#ifndef C_NETWORK_H
#define C_NETWORK_H

#include <map>
#include <string>

struct Interface {
  std::string name; // interface name (e.g. en0)
  int flags;
  u_char type;
  uint8_t mac[6]; // mac address
  uint64_t speed;
  uint64_t packetsIn;
  uint64_t packetsOut;
  uint64_t bytesIn;
  uint64_t bytesOut;

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

  void copy(std::map<std::string, Interface *> &src,
            std::map<std::string, Interface *> &dst);

public:
  void update();

  // print network stats, unless test is set,  return # lines (would be) printed
  uint16_t print(bool test);
};

extern Network network;

#endif // C_NETWORK_H
