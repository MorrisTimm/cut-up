#include <pebble.h>
#include <pebble-events/pebble-events.h>
#include "enamel.h"
#include "cut_up.h"
#include "bluetooth.h"
#ifndef PBL_PLATFORM_APLITE // there is no date on Aplite to save RAM
#include "tapping.h"
#endif
//#include <pebble-time-machine/pebble-time-machine.h>

extern Settings settings;
static char current_text[2][3];
extern char update_text[2][3];

static bool is_in_focus = false;
extern BluetoothSettings bluetooth_settings;
#ifndef PBL_PLATFORM_APLITE // there is no date on Aplite to save RAM
extern TappingSettings tapping_settings;
#endif
static bool show_leading_zeroes_top;
static bool show_leading_zeroes_bottom;

static void load_settings() {
  settings.color_background_top = enamel_get_color_background_top();
  settings.color_background_bottom = enamel_get_color_background_bottom();
  settings.color_text_top = enamel_get_color_text_top();
  settings.color_text_bottom = enamel_get_color_text_bottom();
  settings.color_text_outline_top = enamel_get_color_text_outline_top();
  settings.color_text_outline_bottom = enamel_get_color_text_outline_bottom();
  settings.color_the_cut = connection_service_peek_pebble_app_connection() ? enamel_get_color_the_cut() : enamel_get_color_the_cut_disconnected();
  settings.color_the_cut_outline_top = enamel_get_color_the_cut_outline_top();
  settings.color_the_cut_outline_bottom = enamel_get_color_the_cut_outline_bottom();
  settings.animations = enamel_get_animations();
#ifndef PBL_PLATFORM_APLITE // there is no text positioning on Aplite to save RAM
  settings.offset_y_text_top = enamel_get_offset_y_text_top();
  settings.offset_y_text_bottom = enamel_get_offset_y_text_bottom();
#else
  settings.offset_y_text_top = 0;
  settings.offset_y_text_bottom = 0;
#endif
  settings.start_visible = !settings.animations;

  bluetooth_settings.vibrate_on_disconnect = enamel_get_vibrate_on_bt_disconnect();
  bluetooth_settings.vibrate_on_reconnect = enamel_get_vibrate_on_bt_reconnect();

#ifndef PBL_PLATFORM_APLITE // there is no date on Aplite to save RAM
  if(enamel_get_tap_for_date()) {
    tapping_settings.threshold = enamel_get_tapping_threshold();
    tapping_settings.timeout = (enamel_get_date_timeout()*1000)/DATE_TIMEOUT_PRECISION;
    tapping_init();
  } else {
    tapping_deinit();
  }
#endif
}

static void handle_leading_zeroes() {
#ifndef PBL_PLATFORM_APLITE // there is no date on Aplite to save RAM
  if(tapping_active()) {
    uint8_t show_leading_zeroes = enamel_get_date_leading_zeroes();
    if(enamel_get_date_format()) {
      show_leading_zeroes_top = DATE_LEADING_ZEROES_SHOW == show_leading_zeroes || DATE_LEADING_ZEROES_HIDE_FOR_MONTH == show_leading_zeroes;
      show_leading_zeroes_bottom = DATE_LEADING_ZEROES_SHOW == show_leading_zeroes;
    } else {
      show_leading_zeroes_top = DATE_LEADING_ZEROES_SHOW == show_leading_zeroes;
      show_leading_zeroes_bottom = DATE_LEADING_ZEROES_SHOW == show_leading_zeroes || DATE_LEADING_ZEROES_HIDE_FOR_MONTH == show_leading_zeroes;
    }
  } else
#endif
  {
    uint8_t show_leading_zeroes = enamel_get_show_leading_zeroes();
    show_leading_zeroes_top = SHOW_LEADING_ZEROES_SHOW == show_leading_zeroes;
    show_leading_zeroes_bottom = SHOW_LEADING_ZEROES_SHOW == show_leading_zeroes || SHOW_LEADING_ZEROES_HIDE_FOR_HOURS == show_leading_zeroes;
  }

  if(show_leading_zeroes_top) {
    if(' ' == update_text[CUT_UP_TOP][0]) {
      update_text[CUT_UP_TOP][0] = '0';
    }
  } else {
    if('0' == update_text[CUT_UP_TOP][0]) {
      update_text[CUT_UP_TOP][0] = ' ';
    }
  }
  if(show_leading_zeroes_bottom) {
    if(' ' == update_text[CUT_UP_BOTTOM][0]) {
      update_text[CUT_UP_BOTTOM][0] = '0';
    }
  } else {
    if('0' == update_text[CUT_UP_BOTTOM][0]) {
      update_text[CUT_UP_BOTTOM][0] = ' ';
    }
  }
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
  // store current value for comparison
  memcpy(current_text[CUT_UP_TOP], update_text[CUT_UP_TOP], 2);
  memcpy(current_text[CUT_UP_BOTTOM], update_text[CUT_UP_BOTTOM], 2);

#ifndef PBL_PLATFORM_APLITE // there is no date on Aplite to save RAM
  if(tapping_active()) {
    if(enamel_get_date_format()) {
      strftime(update_text[CUT_UP_TOP], 3, "%d", tick_time);
      strftime(update_text[CUT_UP_BOTTOM], 3, "%m", tick_time);
    } else {
      strftime(update_text[CUT_UP_TOP], 3, "%m", tick_time);
      strftime(update_text[CUT_UP_BOTTOM], 3, "%d", tick_time);
    }
  } else
#endif
  {
    strftime(update_text[CUT_UP_TOP], 3, clock_is_24h_style() ? "%H" : "%I", tick_time);
    strftime(update_text[CUT_UP_BOTTOM], 3, "%M", tick_time);
  }

  handle_leading_zeroes();
  cut_up_update(memcmp(current_text[CUT_UP_TOP], update_text[CUT_UP_TOP], 2),
                memcmp(current_text[CUT_UP_BOTTOM], update_text[CUT_UP_BOTTOM], 2), is_in_focus);

  if(0 == tick_time->tm_min && units_changed & MINUTE_UNIT && enamel_get_hourly_vibration()) {
    bluetooth_vibrate(enamel_get_hourly_vibration());
  }
}

static void enamel_settings_received_handler(void *context) {
  load_settings();

  if(ANIMATIONS_ON != settings.animations) {
    app_focus_service_unsubscribe();
    if(ANIMATIONS_STARTUP_AND_TRANSITIONS != settings.animations) {
      settings.animations = ANIMATIONS_OFF;
    }
  }

  struct tm* tick_time;
#ifdef TIME_MACHINE
  tick_time = time_machine_get_time();
#else
  time_t now = time(NULL);
  tick_time = localtime(&now);
#endif
  tick_handler(tick_time, 0);
}

static void did_focus_handler(bool in_focus) {
  // show text if the watchface gained focus
  if(!is_in_focus && in_focus) {
    is_in_focus = in_focus;
    static bool startup = true;
    if(startup) {
      startup = false;
      time_t now = time(NULL);
      struct tm* tick_time = localtime(&now);
      tick_handler(tick_time, 0);
#ifdef TIME_MACHINE
      light_enable(true);
      struct tm end_time = *tick_time;
      tick_time->tm_hour = 15;
      tick_time->tm_min = 58;
      end_time.tm_hour = 16;
      end_time.tm_min = 01;
      time_machine_init_loop(tick_time, &end_time, TIME_MACHINE_MINUTES, 2000);
      //time_machine_init(tick_time, TIME_MACHINE_MINUTES, 1000);
      time_machine_tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
#else
      tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
#endif
      if(ANIMATIONS_ON != settings.animations) {
        app_focus_service_unsubscribe();
        if(ANIMATIONS_STARTUP_AND_TRANSITIONS != settings.animations) {
          settings.animations = ANIMATIONS_OFF;
        }
      }
    } else {
      cut_up_update(true, true, true);
    }
  }
}

static void will_focus_handler(bool in_focus) {
  // hide text if the watchface is about to lose focus
  if(is_in_focus && !in_focus) {
    is_in_focus = in_focus;
    cut_up_update(true, true, false);
  }
}

void start() {
  bluetooth_init();

  app_focus_service_subscribe_handlers((AppFocusHandlers) {
    .did_focus = did_focus_handler,
    .will_focus = will_focus_handler,
  });
}

#ifndef PBL_PLATFORM_APLITE // there is no date on Aplite to save RAM
void tapping_listener(bool active) {
  struct tm* tick_time;
#ifdef TIME_MACHINE
  tick_time = time_machine_get_time();
#else
  time_t now = time(NULL);
  tick_time = localtime(&now);
#endif
  tick_handler(tick_time, 0);
}
#endif

void bluetooth_listener(bool connected) {
  settings.color_the_cut = connected ? enamel_get_color_the_cut() : enamel_get_color_the_cut_disconnected();
  cut_up_update(false, false, true);
}

void handle_init(void) {
  enamel_init();
  enamel_settings_received_subscribe(enamel_settings_received_handler, NULL);
  events_app_message_open();
  load_settings();
  cut_up_init();
}

void handle_deinit(void) {
  bluetooth_deinit();
  cut_up_deinit();
  enamel_deinit();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
