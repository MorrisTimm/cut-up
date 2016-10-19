#include <pebble.h>
#include <pebble-events/pebble-events.h>
#include "enamel.h"
#include "cut_up.h"
#include "bluetooth.h"

extern Settings settings;
extern char update_text[2][3];
extern BluetoothSettings bluetooth_settings;
static uint8_t hourly_vibration;

static void load_settings() {
  settings.color_background_top = enamel_get_color_background_top();
  settings.color_background_bottom = enamel_get_color_background_bottom();
  settings.color_text_top = enamel_get_color_text_top();
  settings.color_text_bottom = enamel_get_color_text_bottom();
  settings.color_text_outline_top = enamel_get_color_text_outline_top();
  settings.color_text_outline_bottom = enamel_get_color_text_outline_bottom();
  settings.color_the_cut = enamel_get_color_the_cut();
  settings.color_the_cut_disconnected = enamel_get_color_the_cut_disconnected();
  settings.color_the_cut_outline_top = enamel_get_color_the_cut_outline_top();
  settings.color_the_cut_outline_bottom = enamel_get_color_the_cut_outline_bottom();
  settings.animations = enamel_get_animations();
  
  bluetooth_settings.vibrate_on_disconnect = enamel_get_vibrate_on_bt_disconnect();
  bluetooth_settings.vibrate_on_reconnect = enamel_get_vibrate_on_bt_reconnect();
  
  hourly_vibration = enamel_get_hourly_vibration();
}

static void enamel_settings_received_handler(void *context){
  load_settings();
  cut_up_update(false, false);
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
  strftime(update_text[0], 3, clock_is_24h_style() ? "%H" : "%I", tick_time);
  strftime(update_text[1], 3, "%M", tick_time);
#if 1 // for testing animations
  cut_up_update(units_changed & HOUR_UNIT, units_changed & MINUTE_UNIT);
#else
  cut_up_update(units_changed & MINUTE_UNIT, units_changed & SECOND_UNIT && 0 == (tick_time->tm_sec % 10));
#endif
  if(hourly_vibration && 0 == tick_time->tm_min && units_changed & MINUTE_UNIT) {
    bluetooth_vibrate(hourly_vibration);
  }
}

void start() {
  time_t now = time(NULL);
  struct tm* tick_time = localtime(&now);
  tick_handler(tick_time, 0);
#if 1 // for testing animations
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
#else
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
#endif
}

void handle_init(void) {
  enamel_init();
  enamel_settings_received_subscribe(enamel_settings_received_handler, NULL);
  events_app_message_open();
  load_settings();
  cut_up_init();
}

void handle_deinit(void) {
  cut_up_deinit();
  enamel_deinit();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
