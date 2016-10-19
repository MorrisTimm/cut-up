#include <pebble.h>
#include "bluetooth.h"
#include "enamel.h"

extern void bluetooth_listener(bool connected);

BluetoothSettings bluetooth_settings;

static const uint32_t heartbeat_segments[] = { 200, 100, 200, 400, 200, 100, 200 };
static VibePattern heartbeat_pattern = {
  .durations = heartbeat_segments,
  .num_segments = ARRAY_LENGTH(heartbeat_segments),
};
static VibePattern halfbeat_pattern = {
  .durations = heartbeat_segments,
  .num_segments = 3,
};

static bool bt_connected;

void bluetooth_vibrate(uint8_t vibration_pattern) {
  // check no vibration
  if(VIBRATE_ON_BT_DISCONNECT_NONE == vibration_pattern) {
    return;
  }

  // check vibration pattern
  switch(vibration_pattern) {
    case VIBRATE_ON_BT_DISCONNECT_SHORT: {
      vibes_short_pulse();
      break;
    }
    case VIBRATE_ON_BT_DISCONNECT_LONG: {
      vibes_long_pulse();
      break;
    }
    case VIBRATE_ON_BT_DISCONNECT_DOUBLE: {
      vibes_double_pulse();
      break;
    }
    case VIBRATE_ON_BT_DISCONNECT_HEARTBEAT: {
      vibes_enqueue_custom_pattern(heartbeat_pattern);
      break;
    }
    case VIBRATE_ON_BT_DISCONNECT_HALFBEAT: {
      vibes_enqueue_custom_pattern(halfbeat_pattern);
      break;
    }
  }
}

static void bt_handler(bool connected) {
  if(connected) {
    if(bluetooth_settings.vibrated_on_disconnection) {
      bluetooth_settings.vibrated_on_disconnection = false;
      bluetooth_vibrate(bluetooth_settings.vibrate_on_reconnect);
    }
    if(!bt_connected) {
      bt_connected = true;
      bluetooth_vibrate(bluetooth_settings.vibrate_on_reconnect);
    }
  } else {
    bt_connected = false;
    if(!bluetooth_settings.vibrated_on_disconnection) {
      bluetooth_settings.vibrated_on_disconnection = true;
      bluetooth_vibrate(bluetooth_settings.vibrate_on_disconnect);
    }
  }
  bluetooth_listener(connected);
}

void bluetooth_init() {
  bluetooth_settings.vibrated_on_disconnection = persist_read_bool(1);
  bt_connected = bluetooth_connection_service_peek();
  bt_handler(bt_connected);
  bluetooth_connection_service_subscribe(bt_handler);
}

void bluetooth_deinit() {
  persist_write_bool(1, bluetooth_settings.vibrated_on_disconnection);
}

bool bluetooth_connected() {
  return bt_connected;
}
