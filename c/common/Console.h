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
// use ANSI escape sequences to control output to the console/terminal window.

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
  // console window width and height
  int width, height;

public:
  Console();
  ~Console();

public:
  // print a message and exit cleanly with exit code 1
  void abort(const char *fmt, ...);

public:
  // Call to update Console's notion of width and height.
  // Also clears the screen/window.
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
  // reset any modes that are set.
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
  // reset foreground/background colors to default
  void colors_clear();

  // reset foreground color to default
  void fg_clear();
  // use black as foreground color
  void fg_black();
  // use red as foreground color
  void fg_red();
  // use green as foreground color
  void fg_green();
  // use yellow as foreground color
  void fg_yellow();
  // use blue as foreground color
  void fg_blue();
  // use magenta as foreground color
  void fg_magenta();
  // use cyan as foreground color
  void fg_cyan();
  // use white as foreground color
  void fg_white();

  // reset foreground color to default
  void bg_clear();
  // use black as background color
  void bg_black();
  // use red as background color
  void bg_red();
  // use green as background color
  void bg_green();
  // use yellow as background color
  void bg_yellow();
  // use blue as background color
  void bg_blue();
  // use magenta as background color
  void bg_magenta();
  // use cyan as background color
  void bg_cyan();
  // use white as background color
  void bg_white();
};

extern Console console;

#endif // C_CONSOLE_H
