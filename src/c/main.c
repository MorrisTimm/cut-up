
#include <pebble.h>
#include "cutting_edge.h"
//#include "enamel.h"

extern char top_text[3];
extern char bottom_text[3];
extern Settings settings;

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
  strftime(top_text, 3, clock_is_24h_style() ? "%H" : "%I", tick_time);
  strftime(bottom_text, 3, "%M", tick_time);
  cutting_edge_update();
}

void start() {
  time_t now = time(NULL);
  struct tm* tick_time = localtime(&now);
  tick_handler(tick_time, 0);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

void handle_init(void) {
  //enamel_init();
  //events_app_message_open();
  cutting_edge_init();
}

void handle_deinit(void) {
  cutting_edge_deinit();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
