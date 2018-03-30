#ifndef PARSER_H
#define PARSER_H

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Line {
  char *line, *token;
  size_t len;

public:
  Line(char *l);
  ~Line();

public:
  const char *get_token();
};

class Parser {
protected:
  FILE *fp;
  const char *filename;
  Line *line;
  const char *token;

public:
  Parser(const char *filename);
  ~Parser();

public:
  // next line, returns false if EOF
  bool next();
  const char *get_token();
  uint64_t get_long();
};

#endif
