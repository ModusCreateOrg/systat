#include "systat.h"
#include "Network.h"

Interface::Interface(std::string name, uint8_t *mac, if_msghdr2 *if2m) {
  const if_data64 *data = &if2m->ifm_data;
  this->name = name;
  memcpy(this->mac, mac, 6);
  this->type = data->ifi_type;
  this->flags = if2m->ifm_flags;
  this->speed = data->ifi_baudrate;
  this->stats.packetsIn = this->statsDelta.packetsIn = data->ifi_ipackets;
  this->stats.packetsOut = this->statsDelta.packetsOut = data->ifi_opackets;
  this->stats.bytesIn = this->statsDelta.bytesIn = data->ifi_ibytes;
  this->stats.bytesOut = this->statsDelta.bytesOut = data->ifi_obytes;
}

void Interface::update(if_data64 *data) {
  this->statsDelta.packetsIn = data->ifi_ipackets - this->stats.packetsIn;
  this->statsDelta.packetsOut = data->ifi_opackets - this->stats.packetsOut;
  this->statsDelta.bytesIn = data->ifi_ibytes - this->stats.bytesIn;
  this->statsDelta.bytesOut = data->ifi_obytes - this->stats.bytesOut;

  this->stats.packetsIn = data->ifi_ipackets;
  this->stats.packetsOut = data->ifi_opackets;
  this->stats.bytesIn = data->ifi_ibytes;
  this->stats.bytesOut = data->ifi_obytes;
}

void Interface::print() {
  console.print("%-10s %'13lld %'13lld %'13lld %'13lld\n", this->name.c_str(), this->stats.packetsIn/1024, this->stats.packetsOut/1024, this->stats.bytesIn/1024, this->stats.bytesOut/1024);
}

Network::Network() {
  int mib[] = {CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST2, 0};
  size_t len;
  if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
    fprintf(stderr, "sysctl: %s\n", strerror(errno));
    exit(1);
  }

  char *buf = (char *) malloc(len);
  if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
    fprintf(stderr, "sysctl: %s\n", strerror(errno));
    exit(1);
  }

  char *lim = buf + len;
  char *next = nullptr;

  this->stats.bytesIn = 0;
  this->stats.bytesOut = 0;
  this->stats.packetsIn = 0;
  this->stats.packetsOut = 0;

  for (next = buf; next < lim;) {
    struct if_msghdr *ifm = (struct if_msghdr *) next;
    next += ifm->ifm_msglen;
    if (ifm->ifm_type == RTM_IFINFO2) {
      struct if_msghdr2 *if2m = (struct if_msghdr2 *) ifm;

      struct sockaddr_dl *sdl = (struct sockaddr_dl *) (if2m + 1);
      char n[32];
      strncpy(n, sdl->sdl_data, sdl->sdl_nlen);
      n[sdl->sdl_nlen] = '\0';
      std::string name(n);

      this->interfaces[name] = new Interface(name, (uint8_t *) &sdl->sdl_data[sdl->sdl_nlen], if2m);

      this->stats.bytesIn += if2m->ifm_data.ifi_ibytes;
      this->stats.bytesOut += if2m->ifm_data.ifi_obytes;
      this->stats.packetsIn += if2m->ifm_data.ifi_ipackets;
      this->stats.packetsOut += if2m->ifm_data.ifi_opackets;
    }
  }
  free(buf);
}

void Network::each(void (*f)(Interface *)) {
  for (const auto& kv: this->interfaces) {
    f(kv.second);
  }
};

void Network::update() {
  int mib[] = {CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST2, 0};
  size_t len;
  if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
    fprintf(stderr, "sysctl: %s\n", strerror(errno));
    exit(1);
  }

  char *buf = (char *) malloc(len);
  if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
    fprintf(stderr, "sysctl: %s\n", strerror(errno));
    exit(1);
  }

  char *lim = buf + len;
  char *next = nullptr;

  this->stats.bytesIn = 0;
  this->stats.bytesOut = 0;
  this->stats.packetsIn = 0;
  this->stats.packetsOut = 0;

  for (next = buf; next < lim;) {
    struct if_msghdr *ifm = (struct if_msghdr *) next;
    next += ifm->ifm_msglen;
    if (ifm->ifm_type == RTM_IFINFO2) {
      struct if_msghdr2 *if2m = (struct if_msghdr2 *) ifm;
      struct sockaddr_dl *sdl = (struct sockaddr_dl *) (if2m + 1);
      char n[32];
      strncpy(n, sdl->sdl_data, sdl->sdl_nlen);
      n[sdl->sdl_nlen] = '\0';
      std::string name(n);

      Interface *i = this->interfaces[name];
      if (!i) {
        printf("Shouldn't happen!\n");
        return;
      }
      i->update(&if2m->ifm_data);

      this->stats.bytesIn += if2m->ifm_data.ifi_ibytes;
      this->stats.bytesOut += if2m->ifm_data.ifi_obytes;
      this->stats.packetsIn += if2m->ifm_data.ifi_ipackets;
      this->stats.packetsOut += if2m->ifm_data.ifi_opackets;
    }
  }
  free(buf);
}

void Network::print() {
  this->each([](Interface *i) {
    if (i->flags & IFF_UP && i->stats.bytesIn) {
      i->print();
    }
  });
  printf("total ibytes %qu\tobytes %qu\n", this->stats.bytesIn, this->stats.bytesIn);
}

Network network;


