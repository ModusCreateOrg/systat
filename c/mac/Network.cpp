/*
 * This source is based upon httpL//github.com/sklinkert/mac-iostat
 * which is based upon Apple's iostat.c
 * which is based upon BSD's iostat.c
 *
 * See copyright info in iostat.txt.
 */

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
 */
#include "systat.h"

Network network;

void Interface::diff(Interface *newer, Interface *older) {
  this->packetsIn  = newer->packetsIn - older->packetsIn;
  this->packetsOut = newer->packetsOut - older->packetsOut;
  this->bytesIn    = newer->bytesIn - older->bytesIn;
  this->bytesOut   = newer->bytesOut - older->bytesOut;
}

void Network::read(std::map<std::string, Interface *> &m) {
  int    mib[] = {CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST2, 0};
  size_t len;
  if (sysctl(mib, 6, nullptr, &len, nullptr, 0) < 0) {
    fprintf(stderr, "sysctl: %s\n", strerror(errno));
    exit(1);
  }

  auto *buf = new char[len];
  if (sysctl(mib, 6, buf, &len, nullptr, 0) < 0) {
    fprintf(stderr, "sysctl: %s\n", strerror(errno));
    exit(1);
  }

  char *lim  = buf + len;
  char *next = nullptr;

  for (next = buf; next < lim;) {
    if_msghdr *ifm = (struct if_msghdr *) next;
    next += ifm->ifm_msglen;
    if (ifm->ifm_type == RTM_IFINFO2) {
      auto *if2m = (struct if_msghdr2 *) ifm;

      auto *sdl = (struct sockaddr_dl *) (if2m + 1);
      char n[32];
      strncpy(n, sdl->sdl_data, sdl->sdl_nlen);
      n[sdl->sdl_nlen] = '\0';
      std::string name(n);

      Interface *iface = m[name];
      if (!iface) {
        iface = m[name] = new Interface;
        iface->name = strdup(name.c_str());
      }

      auto *data = &if2m->ifm_data;
      memcpy(iface->mac, &sdl->sdl_data[sdl->sdl_nlen], 6);
      iface->type       = data->ifi_type;
      iface->flags      = if2m->ifm_flags;
      iface->speed      = data->ifi_baudrate;
      iface->packetsIn  = data->ifi_ipackets;
      iface->packetsOut = data->ifi_opackets;
      iface->bytesIn    = data->ifi_ibytes;
      iface->bytesOut   = data->ifi_obytes;
    }
  }
  delete[] buf;
}

Network::Network() {
  this->read(this->current);
  this->copy(this->last, this->current);
  this->copy(this->delta, this->current);
  this->update();
}

void Network::copy(std::map<std::string, Interface *> &dst, std::map<std::string, Interface *> &src) {
  for (const auto &kv: src) {
    auto      *s = (Interface *) kv.second;
    Interface *d = dst[s->name];
    if (!d) {
      d = dst[s->name] = new Interface;
      d->name = strdup(s->name.c_str());
    }
    d->type      = s->type;
    d->flags     = s->flags;
    memcpy(d->mac, s->mac, 6);
    d->speed      = s->speed;
    d->packetsIn  = s->packetsIn;
    d->packetsOut = s->packetsOut;
    d->bytesIn    = s->bytesIn;
    d->bytesOut   = s->bytesOut;
  }
}

void Network::update() {
  this->copy(this->last, this->current);
  this->read(this->current);

  for (auto &kv: this->delta) {
    Interface *i     = (Interface *) kv.second;
    Interface *newer = this->current[i->name],
              *older = this->last[i->name];
    i->diff(newer, older);
  }
}

uint16_t Network::print(bool test) {
  uint16_t count = 0;
  if (!test) {
    console.inverseln("%-10s %13s %13s %13s %13s %13s %13s", "NETWORK", "Read (B/s)", "Write (B/s)", "RX Packets",
                      "TX Packets", "Total RX", "Total TX");
  }
  count++;

  for (const auto &kv: this->delta) {
    Interface *i = (Interface *) kv.second;
    const char *name = i->name.c_str();
    if (!strncmp(name, "utun", 4) || !strncmp(name, "awdl", 4) || !strncmp(name, "lo", 2)) {
      continue;
    }
    Interface *c = (Interface *)this->current[name];
    if (i->flags & IFF_UP && this->current[i->name]->packetsIn) {
      if (!test) {
        console.println("%-10s %'13lld %'13lld %'13lld %'13lld %'13lld %'13lld",
                        i->name.c_str(),
                        i->bytesIn,
                        i->bytesOut,
                        i->packetsIn,
                        i->packetsOut,
                        c->packetsIn,
                        c->packetsOut
        );
      }
      count++;
    }
//  printf("total ibytes %qu\tobytes %qu\n", this->stats.bytesIn, this->stats.bytesIn);
  }
  return count;
}



