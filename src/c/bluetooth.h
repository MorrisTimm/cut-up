#pragma once
#include <pebble.h>

typedef struct BluetoothSettings {
  uint8_t vibrate_on_disconnect;
  uint8_t vibrate_on_reconnect;
  bool vibrated_on_disconnection;
} __attribute__((__packed__)) BluetoothSettings;

void bluetooth_init();
void bluetooth_deinit();
bool bluetooth_connected();
void bluetooth_vibrate(uint8_t vibration_pattern);
