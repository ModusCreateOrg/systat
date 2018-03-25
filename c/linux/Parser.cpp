#include "systat.h"

Line::Line(char *l) {
  size_t len = strlen(l);
  if (len && l[len - 1] == '\n') {
    l[len - 1] = '\0';
    len--;
  }
  this->len = len;
  this->line = l;
  this->token = l;
}

Line::~Line() { delete this->line; }
