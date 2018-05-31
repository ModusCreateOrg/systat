/*
 * systat for MacOS and Linux
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
  this->line = NULL;
}

const char *Line::get_token() {
  while (*this->token == ' ' || *this->token == '\t') {
    this->token++;
  }
  if (*this->token == '\0') {
    return NULL;
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
  this->line = NULL;
  this->token = NULL;

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
    this->line = NULL;
  }
  if (this->token) {
    delete this->token;
    this->token = NULL;
  }
  if (this->fp) {
    fclose(this->fp);
  }
  this->fp = NULL;
}

bool Parser::next() {
  if (this->line) {
    delete this->line;
    this->line = NULL;
  }
  char *in = NULL;
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
    this->token = NULL;
  }
  if (!this->line) {
    return NULL;
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
