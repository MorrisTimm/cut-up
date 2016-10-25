#include <pebble.h>
#ifndef PBL_PLATFORM_APLITE // there is no date on Aplite to save RAM
#include "tapping.h"
#include "enamel.h"

extern void tapping_listener(bool active);

TappingSettings tapping_settings;

static AppTimer* active_timer = NULL;
static AppTimer* counter_timer = NULL;
static bool active = false;
static uint8_t counter;

static void active_timer_handler(void* data) {
  active_timer = NULL;
  active = false;
  tapping_listener(active);
}

static void counter_timer_handler(void* data) {
  counter_timer = NULL;
  counter = 0;
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  if(++counter >= enamel_get_tapping_threshold()) {
    active = true;
    counter = 0;
    tapping_listener(active);
    if(counter_timer) {
      app_timer_cancel(counter_timer);
      counter_timer = NULL;
    }
    if(!active_timer || !app_timer_reschedule(active_timer, tapping_settings.timeout)) {
      active_timer = app_timer_register(tapping_settings.timeout, active_timer_handler, NULL);
    }
  } else {
    if(!counter_timer || !app_timer_reschedule(counter_timer, 500)) {
      counter_timer = app_timer_register(tapping_settings.timeout, counter_timer_handler, NULL);
    }
  }
}

void tapping_init() {
  active_timer = NULL;
  active = false;
  counter = 0;
  accel_tap_service_subscribe(tap_handler);
}

void tapping_deinit() {
  accel_tap_service_unsubscribe();
  active = false;
}

bool tapping_active() {
  return active;
}
#endif
