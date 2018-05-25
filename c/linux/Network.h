/*
 * systat for Linux
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
 */

#ifndef NETWORK_H
#define NETWORK_H

struct NetStats {
  const char *name;
  uint64_t rx_bytes, rx_packets, rx_errors, rx_drop, rx_fifo, rx_frame,
      rx_compressed, rx_multicast;
  uint64_t tx_bytes, tx_packets, tx_errors, tx_drop, tx_fifo, tx_calls,
      tx_carrier, tx_compressed;

public:
  void diff(NetStats *newer, NetStats *older);
};

class Network {
public:
  uint16_t num_interfaces;

private:
  std::map<std::string, NetStats *> last, current, delta;

public:
  Network();
  ~Network();

private:
  uint16_t read(std::map<std::string, NetStats *> &m);
  void copy(std::map<std::string, NetStats *> &dst,
            std::map<std::string, NetStats *> &src);

public:
  void update();
  uint16_t print(bool test);
};

extern Network network;

#endif
