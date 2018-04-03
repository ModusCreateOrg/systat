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

// GENERAL PURPOSE CONSOLE

#ifndef C_CONSOLE_H
#define C_CONSOLE_H

#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>

class Console {
private:
  bool aborting;
  // ciuror location
  uint8_t row, col;
  // modes
  bool bold, underscore, blink, inverse, concealed;
  bool cursor_hidden;
  // colors
  uint8_t background, foreground;

public:
  int width, height;

public:
  Console();
  ~Console();

public:
  void abort(const char *fmt, ...);

public:
  void resize();

public:
  // enable/disable cursor
  void show_cursor(bool on = true);

  // clear screen
  void clear();

  // reset all modes and colors, enable cursor
  void reset();

  // clear to end of line
  void clear_eol();

  // address cursor
  void moveTo(uint8_t row, uint8_t col);

  // printf style output to terminal
  void print(const char *fmt, ...);
  // printf style output to terminal, with newline
  void println(const char *fmt, ...);
  // print line inverse, with newline
  void inverseln(const char *fmt, ...);
  // emit a newline
  void newline();

private:
  void set_mode(uint8_t attr, bool on);

public:
  void mode_clear();
  // turn on/off bold
  void mode_bold(bool on = true);

  // turn on/off underscore
  void mode_underscore(bool on = true);

  // turn on/off blink
  void mode_blink(bool on = true);

  // turn on/off inverse
  void mode_inverse(bool on = true);

  // turn on/off concealed
  void mode_concealed(bool on = true);

private:
  void set_color(uint8_t color, bool on);

public:
  void colors_clear();

  void fg_clear();
  void fg_black();
  void fg_red();
  void fg_green();
  void fg_yellow();
  void fg_blue();
  void fg_magenta();
  void fg_cyan();
  void fg_white();

  void bg_clear();
  void bg_black();
  void bg_red();
  void bg_green();
  void bg_yellow();
  void bg_blue();
  void bg_magenta();
  void bg_cyan();
  void bg_white();
};

extern Console console;

#endif // C_CONSOLE_H
