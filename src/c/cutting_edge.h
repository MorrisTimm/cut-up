#pragma once
#include <pebble.h>

typedef struct {
  GColor color_background_top;
  GColor color_background_bottom;
  GColor color_text_top;
  GColor color_text_bottom;
  GColor color_text_outline_top;
  GColor color_text_outline_bottom;
  GColor color_the_cut;
} Settings;

void cutting_edge_update();
void cutting_edge_init();
void cutting_edge_deinit();
