#pragma once
#include <pebble.h>

enum {
  CUT_UP_TOP,
  CUT_UP_BOTTOM
};

typedef struct {
  GColor color_background_top;
  GColor color_background_bottom;
  GColor color_text_top;
  GColor color_text_bottom;
  GColor color_text_outline_top;
  GColor color_text_outline_bottom;
  GColor color_the_cut;
  GColor color_the_cut_outline_top;
  GColor color_the_cut_outline_bottom;
  GColor color_the_cut_disconnected;
  uint8_t animations;
} Settings;

void cut_up_update(bool hours, bool minutes);
void cut_up_init();
void cut_up_deinit();
