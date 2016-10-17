#pragma once
#include <pebble.h>

typedef struct {
  GColor color_background_top;
  GColor color_background_bottom;
  GColor color_text_top;
  GColor color_text_bottom;
  GColor color_text_fill_top;
  GColor color_text_fill_bottom;
  GColor color_the_cut;
  uint8_t font_top;
  uint8_t font_bottom;
} Settings;

void cutting_edge_update();
void cutting_edge_init();
void cutting_edge_deinit();
