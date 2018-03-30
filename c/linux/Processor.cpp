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
  std::map<std::string, CPU *> last, current, delta;
  this->num_cores = this->read(this->current);
  this->copy(this->last, this->current);
  this->copy(this->delta, this->current);
  this->update();
}

uint16_t Processor::read(std::map<std::string, CPU *> &m) {
  uint16_t count = 0;
  FILE *fp = fopen("/proc/stat", "r");
  char *line = nullptr;
  size_t len = 0;
  while (getline(&line, &len, fp) >= 0) {
    Line l = Line(line);
    const char *token = l.get_token();
    if (!strncmp(token, "cpu", 3)) {
      count++;
      CPU *cpu = m[token];
      if (!cpu) {
        cpu = m[token] = new CPU;
        cpu->name = token;
      }
      else {
        delete[] token;
      }
      token = l.get_token();
      cpu->user = atol(token);
      delete[] token;

      token = l.get_token();
      cpu->nice = atol(token);
      delete[] token;

      token = l.get_token();
      cpu->system = atol(token);
      delete[] token;

      token = l.get_token();
      cpu->idle = atol(token);
      delete[] token;

      token = l.get_token();
      cpu->iowait = atol(token);
      delete[] token;

      token = l.get_token();
      cpu->irq = atol(token);
      delete[] token;

      token = l.get_token();
      cpu->softirq = atol(token);
      delete[] token;

      //        cpu->print();
    }
    line = nullptr;
    len = 0;
  }
  fclose(fp);
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

void Processor::print() {
  console.inverseln("%-6s %7s %7s %7s %7s %7s %7s %7s", "CPUS", "User",
                    "System", "Nice", "Idle", "IOWait", "IRQ", "SoftIRQ");

#if 0
    printf("LAST\n");
    for (const auto &kv : this->last) {
      CPU *cpu = (CPU *)kv.second;
      cpu->print();
    }
    printf("CURRENT\n");
    for (const auto &kv : this->current) {
      CPU *cpu = (CPU *)kv.second;
      cpu->print();
    }
#endif

  for (const auto &kv : this->delta) {
    CPU *cpu = (CPU *)kv.second;
    cpu->print();
  }
}
