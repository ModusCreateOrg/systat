#include "systat.h"

Processor processor;

static char *u = nullptr;
static char *ucstring(const char *s) {
  if (u) {
    delete u;
  }
  size_t len = strlen(s);
  u = new char[len + 1];

  for (size_t i = 0; i < len; i++) {
    u[i] = toupper(int(s[i]));
  }
  u[len] = '\0';
  return u;
}

void CPU::diff(CPU *newer, CPU *older) {
  this->user = newer->user - older->user;
  this->nice = newer->nice - older->nice;
  this->system = newer->system - older->system;
  this->idle = newer->idle - older->idle;
  this->iowait = newer->iowait - older->iowait;
  this->irq = newer->irq - older->irq;
  this->softirq = newer->softirq - older->softirq;
}

void CPU::print() {
  double total = this->user + this->system + this->nice + this->idle +
                 this->iowait + this->irq + this->softirq;
  printf("%-6s %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%%\n",
         ucstring(this->name), 100 * this->user / total,
         100 * this->system / total, 100 * this->nice / total,
         100 * this->idle / total, 100 * this->iowait / total,
         100 * this->irq / total, 100 * this->softirq / total);
}

Processor::Processor() {
  this->condensed = false;
  this->num_cores = this->read(this->current);
  this->copy(this->last, this->current);
  this->copy(this->delta, this->current);
  this->update();
}

uint16_t Processor::read(std::map<std::string, CPU *> &m) {
  uint16_t count = 0;
  Parser p("/proc/stat");
  while (p.next()) {
    const char *token = p.get_token();
    if (!strncmp(token, "cpu", 3)) {
      count++;
      CPU *cpu = m[token];
      if (!cpu) {
        cpu = m[token] = new CPU;
        cpu->name = strdup(token);
      }
      cpu->user = p.get_long();
      cpu->nice = p.get_long();
      cpu->system = p.get_long();
      cpu->idle = p.get_long();
      cpu->iowait = p.get_long();
      cpu->irq = p.get_long();
      cpu->softirq = p.get_long();
    }
  }
  return count;
}

void Processor::copy(std::map<std::string, CPU *> &dst,
                     std::map<std::string, CPU *> &src) {
  for (const auto &kv : src) {
    CPU *o = (CPU *)kv.second;
    const char *name = o->name;

    CPU *cpu = dst[name];
    if (!cpu) {
      cpu = dst[name] = new CPU;
      cpu->name = strdup(name);
    }
    cpu->user = o->user;
    cpu->system = o->system;
    cpu->idle = o->idle;
    cpu->nice = o->nice;
    cpu->iowait = o->iowait;
    cpu->irq = o->irq;
    cpu->softirq = o->softirq;
  }
}

void Processor::update() {
  this->copy(this->last, this->current);
  this->read(this->current);

  for (const auto &kv : this->delta) {
    CPU *cpu = (CPU *)kv.second;
    cpu->diff(this->current[cpu->name], this->last[cpu->name]);
  }
}

uint16_t Processor::print() {
  uint16_t count = 0;
  console.inverseln("%-6s %7s %7s %7s %7s %7s %7s %7s", "CPUS", "User",
                    "System", "Nice", "Idle", "IOWait", "IRQ", "SoftIRQ");
  count++;

  for (const auto &kv : this->delta) {
    CPU *cpu = (CPU *)kv.second;
    cpu->print();
    count++;
    if (this->condensed) {
      break;
    }
  }
  return count;
}
