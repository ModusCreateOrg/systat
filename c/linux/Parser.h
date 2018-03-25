#ifndef PARSER_H
#define PARSER_H

struct Line {
  char *line, *token;
  size_t len;

public:
  Line(char *l);
  ~Line();

  const char *get_token() {
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
    const char *tok = strndup(this->token, ptr - this->token);
    this->token = ptr;
    return tok;
  }
};

#endif
