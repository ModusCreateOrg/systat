//
// Created by Michael Schwartz on 3/21/18.
//

#include "systat.h"

const char ESC = 0x1b;

const uint8_t ATTR_OFF = 0;
const uint8_t ATTR_BOLD = 1;
const uint8_t ATTR_UNDERSCORE = 4;
const uint8_t ATTR_BLINK = 5;
const uint8_t ATTR_INVERSE = 7;
const uint8_t ATTR_CONCEALED = 8;

// colors
const uint8_t FG_BLACK = 30;
const uint8_t FG_RED = 31;
const uint8_t FG_GREEN = 32;
const uint8_t FG_YELLOW = 33;
const uint8_t FG_BLUE = 34;
const uint8_t FG_MAGENTA = 35;
const uint8_t FG_CYAN = 36;
const uint8_t FG_WHITE = 37;
const uint8_t BG_BLACK = 40;
const uint8_t BG_RED = 41;
const uint8_t BG_GREEN = 42;
const uint8_t BG_YELLOW = 43;
const uint8_t BG_BLUE = 44;
const uint8_t BG_MAGENTA = 45;
const uint8_t BG_CYAN = 46;
const uint8_t BG_WHITE = 47;

Console::Console() {
  this->reset();
  this->clear();
}

Console::~Console() {
  this->reset();
  this->clear();
}

void Console::showCursor(bool on) {
  if (on) {
    printf("%c[?25h", ESC);
    cursor_hidden = false;
  }
  else {
    printf("%c[?25l", ESC);
    cursor_hidden = true;
  }
  fflush(stdout);
}

/** @public **/
void Console::clear() {
  printf("%c[2J", ESC);
  fflush(stdout);
  this->row = this->col = 0;
}

/** @public **/
void Console::reset() {
  this->set_mode(ATTR_OFF, true);
  this->background = this->foreground = 0;
  this->showCursor();
}

/** @public **/
void Console::clear_eol() {
  printf("%c[K", ESC);
  fflush(stdout);
}

/** @public **/
void Console::moveTo(uint8_t row, uint8_t col) {
  printf("%c[%d;%dH", ESC, row, col);
  fflush(stdout);
  this->row = row;
  this->col = col;
}

/** @private */
void Console::set_mode(uint8_t attr, bool on) {
  if (on) {
    printf("%c[%dm", ESC, attr);
  } else {
    printf("%c[=%dl", ESC, attr);
  }
  fflush(stdout);
  switch (attr) {
    case ATTR_OFF:
      this->bold = this->underscore = this->blink = this->inverse = this->concealed = false;
      break;
    case ATTR_BOLD:
      this->bold = on;
      break;
    case ATTR_UNDERSCORE:
      this->underscore = on;
      break;
    case ATTR_BLINK:
      this->blink = on;
      break;
    case ATTR_INVERSE:
      this->inverse = on;
      break;
    case ATTR_CONCEALED:
      this->concealed = on;
      break;
  }
}

void Console::mode_clear() {
  this->set_mode(ATTR_OFF, true);
  fflush(stdout);
}

/** @public **/
void Console::mode_bold(bool on) {
  this->set_mode(ATTR_BOLD, on);
}

/** @public **/
void Console::mode_underscore(bool on) {
  this->set_mode(ATTR_UNDERSCORE, on);
}

/** @public **/
void Console::mode_blink(bool on) {
  this->set_mode(ATTR_BLINK, on);
}

/** @public **/
void Console::mode_inverse(bool on) {
  this->set_mode(ATTR_INVERSE, on);
}

/** @public **/
void Console::mode_concealed(bool on) {
  this->set_mode(ATTR_CONCEALED, on);
}

/** @private */
void Console::set_color(uint8_t color, bool on) {
  if (on) {
    printf("%c[%dm", ESC, color);
  } else {
    printf("%c[%dl", ESC, color);
  }
  fflush(stdout);
}

/** @public **/
void Console::colors_clear() {
  if (this->background) {
    this->set_color(background, false);
  }
  if (this->foreground) {
    this->set_mode(foreground, false);
  }
  this->background = this->foreground = 0;
}

/** @public **/
void Console::bg_clear() {
  if (this->background) {
    this->set_mode(background, false);
  }
  this->background = 0;
}

/** @public **/
void Console::fg_clear() {
  if (this->foreground) {
    this->set_mode(foreground, false);
  }
  this->foreground = 0;
}

/** @public **/
void Console::bg_black() {
  this->set_color(BG_BLACK, true);
  this->background = BG_BLACK;
}

/** @public **/
void Console::fg_black() {
  this->set_color(FG_BLACK, true);
  this->foreground = FG_BLACK;
}

/** @public **/
void Console::bg_red() {
  this->set_color(BG_RED, true);
  this->background = BG_RED;
}

/** @public **/
void Console::fg_red() {
  this->set_color(FG_RED, true);
  this->foreground = FG_RED;
}

/** @public **/
void Console::bg_green() {
  this->set_color(BG_GREEN, true);
  this->background = BG_GREEN;
}

/** @public **/
void Console::fg_green() {
  this->set_color(FG_GREEN, true);
  this->foreground = FG_GREEN;
}

/** @public **/
void Console::bg_yellow() {
  this->set_color(BG_YELLOW, true);
  this->background = BG_YELLOW;
}

/** @public **/
void Console::fg_yellow() {
  this->set_color(FG_YELLOW, true);
  this->foreground = FG_YELLOW;
}

/** @public **/
void Console::bg_blue() {
  this->set_color(BG_BLUE, true);
  this->background = BG_BLUE;
}

/** @public **/
void Console::fg_blue() {
  this->set_color(FG_BLUE, true);
  this->foreground = FG_BLUE;
}

/** @public **/
void Console::bg_magenta() {
  this->set_color(BG_MAGENTA, true);
  this->background = BG_MAGENTA;
}

/** @public **/
void Console::fg_magenta() {
  this->set_color(FG_MAGENTA, true);
  this->foreground = FG_MAGENTA;
}

/** @public **/
void Console::bg_cyan() {
  this->set_color(BG_CYAN, true);
  this->background = BG_CYAN;
}

/** @public **/
void Console::fg_cyan() {
  this->set_color(FG_CYAN, true);
  this->foreground = FG_CYAN;
}

/** @public **/
void Console::bg_white() {
  this->set_color(BG_WHITE, true);
  this->background = BG_WHITE;
}

/** @public **/
void Console::fg_white() {
  this->set_color(FG_WHITE, true);
  this->foreground = FG_WHITE;
}

void Console::print(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  fflush(stdout);
}

void Console::println(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  fputs("\n", stdout);
  fflush(stdout);
}

void Console::newline() {
  fputs("\n", stdout);
  fflush(stdout);
}

Console console;
