#pragma once
#include <pebble.h>
#ifndef PBL_PLATFORM_APLITE // there is no date on Aplite to save RAM

typedef struct TappingSettings {
  uint8_t threshold;
  uint16_t timeout;
} __attribute__((__packed__)) TappingSettings;

void tapping_init();
void tapping_deinit();
bool tapping_active();
#endif
