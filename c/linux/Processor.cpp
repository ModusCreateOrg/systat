#include "systat.h"

Processor processor;

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
  printf("%-6s %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f\n", this->name,
         100 * this->user / total, 100 * this->system / total,
         100 * this->nice / total, 100 * this->idle / total,
         100 * this->iowait / total, 100 * this->irq / total,
         100 * this->softirq / total);
}

Processor::Processor() {
  std::map<std::string, CPU *> last, current, delta;
  this->read(this->current);
  this->copy(this->last, this->current);
  this->copy(this->delta, this->current);
}

void Processor::read(std::map<std::string, CPU *> &m) {
  FILE *fp = fopen("/proc/stat", "r");
  char *line = nullptr;
  size_t len = 0;
  while (getline(&line, &len, fp) >= 0) {
    Line l = Line(line);
    const char *token = l.get_token();
    if (!strncmp(token, "cpu", 3)) {
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
  console.bg_white();
  console.fg_black();
  console.print("%6s", "CPU STATES");
  console.clear_eol();
  console.newline();
  console.print("%6s %6s %6s %6s %6s %6s %6s %6s", "", "user", "sys", "nice",
                "idle", "iowait", "irq", "softirq");
  console.clear_eol();
  console.newline();
  console.mode_clear();

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
