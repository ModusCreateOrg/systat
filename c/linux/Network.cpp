#include "systat.h"

Network network;

void NetStats::diff(NetStats *newer, NetStats *older) {
  this->rx_bytes = newer->rx_bytes - older->rx_bytes;
  this->rx_packets = newer->rx_packets - older->rx_packets;
  this->rx_errors = newer->rx_errors - older->rx_errors;
  this->rx_drop = newer->rx_drop - older->rx_drop;
  this->rx_fifo = newer->rx_fifo - older->rx_fifo;
  this->rx_frame = newer->rx_frame - older->rx_frame;
  this->rx_compressed = newer->rx_compressed - older->rx_compressed;
  this->rx_multicast = newer->rx_multicast - older->rx_multicast;
  this->tx_bytes = newer->tx_bytes - older->tx_bytes;
  this->tx_packets = newer->tx_packets - older->tx_packets;
  this->tx_errors = newer->tx_errors - older->tx_errors;
  this->tx_drop = newer->tx_drop - older->tx_drop;
  this->tx_fifo = newer->tx_fifo - older->tx_fifo;
  this->tx_calls = newer->tx_calls - older->tx_calls;
  this->tx_carrier = newer->tx_carrier - older->tx_carrier;
  this->tx_compressed = newer->tx_compressed - older->tx_compressed;
}

Network::Network() {
  //
  this->num_interfaces = this->read(this->current);
  this->copy(this->last, this->current);
  this->copy(this->delta, this->current);
  this->update();
}

Network::~Network() {
  //
}

void Network::copy(std::map<std::string, NetStats *> &dst,
                   std::map<std::string, NetStats *> &src) {
  for (const auto &kv : src) {
    NetStats *other = (NetStats *)kv.second;
    const char *name = other->name;

    NetStats *iface = dst[name];
    if (!iface) {
      iface = dst[name] = new NetStats;
      iface->name = strdup(name);
    }
    iface->rx_bytes = other->rx_bytes;
    iface->rx_packets = other->rx_packets;
    iface->rx_errors = other->rx_errors;
    iface->rx_drop = other->rx_drop;
    iface->rx_fifo = other->rx_fifo;
    iface->rx_frame = other->rx_frame;
    iface->rx_compressed = other->rx_compressed;
    iface->rx_multicast;
    iface->tx_bytes = other->tx_bytes;
    iface->tx_packets = other->tx_packets;
    iface->tx_errors = other->tx_errors;
    iface->tx_drop = other->tx_drop;
    iface->tx_fifo = other->tx_fifo;
    iface->tx_calls = other->tx_calls;
    iface->tx_carrier = other->tx_carrier;
    iface->tx_compressed = other->tx_compressed;
  }
}

void Network::update() {
  this->copy(this->last, this->current);
  this->read(this->current);

  for (const auto &kv : this->delta) {
    NetStats *iface = (NetStats *)kv.second;
    iface->diff(this->current[iface->name], this->last[iface->name]);
  }
}

uint16_t Network::read(std::map<std::string, NetStats *> &m) {
  //
  Parser p("/proc/net/dev");
  // skip first two lines (which are headers, not data)
  if (!p.next() || !p.next()) {
    console.abort("Invalid /proc/net/dev format");
  }
  uint16_t count = 0;
  while (p.next()) {
    char *token = (char *)p.get_token();
    token[strlen(token) - 1] = '\0';
    NetStats *stats = m[token];
    if (!stats) {
      count++;
      stats = m[token] = new NetStats;
      stats->name = strdup(token);
    }
    stats->rx_bytes = p.get_long();
    stats->rx_packets = p.get_long();
    stats->rx_errors = p.get_long();
    stats->rx_drop = p.get_long();
    stats->rx_fifo = p.get_long();
    stats->rx_frame = p.get_long();
    stats->rx_compressed = p.get_long();
    stats->rx_multicast = p.get_long();
    stats->tx_bytes = p.get_long();
    stats->tx_packets = p.get_long();
    stats->tx_errors = p.get_long();
    stats->tx_drop = p.get_long();
    stats->tx_fifo = p.get_long();
    stats->tx_calls = p.get_long();
    stats->tx_carrier = p.get_long();
    stats->tx_compressed = p.get_long();
  }
  return count;
}

uint16_t Network::print() {
  uint16_t count = 0;
  console.inverseln("%-10s %12s %12s %12s %12s %12s %12s", "NETWORK",
                    "Read (KB/s)", "Write (KB/s)", "RX Packets", "RX Errors",
                    "TX Packets", "TX Errors");
  count++;

  for (const auto &kv : this->delta) {
    NetStats *delta = (NetStats *)kv.second,
             *current = this->current[delta->name];

    if (strcmp(delta->name, "lo") && current->tx_packets) {
      console.println("%-10s %12.2f %12.2f %12d %12d %12d %12d", delta->name,
                      double(delta->rx_bytes) / 1024,
                      double(delta->tx_bytes) / 1024, current->rx_packets,
                      current->rx_errors, current->tx_packets,
                      current->tx_errors);
    }
    count++;
  }
  return count;
}
