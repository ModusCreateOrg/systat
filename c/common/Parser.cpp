#include "Parser.h"

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

Line::~Line() {
  if (this->line) {
    delete this->line;
  }
  this->line = nullptr;
}

const char *Line::get_token() {
  while (*this->token == ' ' || *this->token == '\t') {
    this->token++;
  }
  if (*this->token == '\0') {
    return nullptr;
  }
  char *ptr = this->token;
  while (*ptr && *ptr != ' ') {
    ptr++;
  }
  const size_t len = ptr - this->token;
  char *tok = new char[len + 1];
  strncpy(tok, this->token, len);
  tok[len] = '\0';
  this->token = ptr;
  return tok;
}

Parser::Parser(const char *filename) {
  this->filename = strdup(filename);
  this->line = nullptr;
  this->token = nullptr;

  this->fp = fopen(filename, "r");
  if (!this->fp) {
    printf("Can't open %s\n", filename);
    exit(1);
  }
}

Parser::~Parser() {
  delete this->filename;
  if (this->line) {
    delete this->line;
    this->line = nullptr;
  }
  if (this->token) {
    delete this->token;
    this->token = nullptr;
  }
  if (this->fp) {
    fclose(this->fp);
  }
  this->fp = nullptr;
}

bool Parser::next() {
  if (this->line) {
    delete this->line;
    this->line = nullptr;
  }
  char *in = nullptr;
  size_t len = 0;
  if (getline(&in, &len, this->fp) >= 0) {
    this->line = new Line(in);
    return true;
  }
  return false;
}

const char *Parser::get_token() {
  if (this->token) {
    delete this->token;
    this->token = nullptr;
  }
  if (!this->line) {
    return nullptr;
  }
  return this->token = this->line->get_token();
}

uint64_t Parser::get_long() {
  const char *token = this->get_token();
  if (token) {
    return atol(token);
  }
  return 0;
}
