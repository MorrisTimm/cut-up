/**
 * This file was generated with Enamel : http://gregoiresage.github.io/enamel
 */

#ifndef ENAMEL_H
#define ENAMEL_H

#include <pebble.h>

// -----------------------------------------------------
// Getter for 'color_background_top'
GColor enamel_get_color_background_top();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_text_top'
GColor enamel_get_color_text_top();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_text_outline_top'
GColor enamel_get_color_text_outline_top();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'show_leading_zeroes_top'
bool enamel_get_show_leading_zeroes_top();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'offset_y_text_top'
#define OFFSET_Y_TEXT_TOP_PRECISION 1
int32_t enamel_get_offset_y_text_top();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_the_cut_outline_top'
GColor enamel_get_color_the_cut_outline_top();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_the_cut'
GColor enamel_get_color_the_cut();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_the_cut_disconnected'
GColor enamel_get_color_the_cut_disconnected();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_the_cut_outline_bottom'
GColor enamel_get_color_the_cut_outline_bottom();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_background_bottom'
GColor enamel_get_color_background_bottom();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_text_bottom'
GColor enamel_get_color_text_bottom();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_text_outline_bottom'
GColor enamel_get_color_text_outline_bottom();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'show_leading_zeroes_bottom'
bool enamel_get_show_leading_zeroes_bottom();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'offset_y_text_bottom'
#define OFFSET_Y_TEXT_BOTTOM_PRECISION 1
int32_t enamel_get_offset_y_text_bottom();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'animations'
typedef enum {
	ANIMATIONS_OFF = 0,
	ANIMATIONS_STARTUP_AND_TRANSITIONS = 1,
	ANIMATIONS_STARTUP_ONLY = 2,
} ANIMATIONSValue;
ANIMATIONSValue enamel_get_animations();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'vibrate_on_bt_disconnect'
typedef enum {
	VIBRATE_ON_BT_DISCONNECT_NONE = 0,
	VIBRATE_ON_BT_DISCONNECT_SHORT = 1,
	VIBRATE_ON_BT_DISCONNECT_LONG = 2,
	VIBRATE_ON_BT_DISCONNECT_DOUBLE = 3,
	VIBRATE_ON_BT_DISCONNECT_HEARTBEAT = 4,
	VIBRATE_ON_BT_DISCONNECT_HALFBEAT = 5,
} VIBRATE_ON_BT_DISCONNECTValue;
VIBRATE_ON_BT_DISCONNECTValue enamel_get_vibrate_on_bt_disconnect();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'vibrate_on_bt_reconnect'
typedef enum {
	VIBRATE_ON_BT_RECONNECT_NONE = 0,
	VIBRATE_ON_BT_RECONNECT_SHORT = 1,
	VIBRATE_ON_BT_RECONNECT_LONG = 2,
	VIBRATE_ON_BT_RECONNECT_DOUBLE = 3,
	VIBRATE_ON_BT_RECONNECT_HEARTBEAT = 4,
	VIBRATE_ON_BT_RECONNECT_HALFBEAT = 5,
} VIBRATE_ON_BT_RECONNECTValue;
VIBRATE_ON_BT_RECONNECTValue enamel_get_vibrate_on_bt_reconnect();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'hourly_vibration'
typedef enum {
	HOURLY_VIBRATION_NONE = 0,
	HOURLY_VIBRATION_SHORT = 1,
	HOURLY_VIBRATION_LONG = 2,
	HOURLY_VIBRATION_DOUBLE = 3,
	HOURLY_VIBRATION_HEARTBEAT = 4,
	HOURLY_VIBRATION_HALFBEAT = 5,
} HOURLY_VIBRATIONValue;
HOURLY_VIBRATIONValue enamel_get_hourly_vibration();
// -----------------------------------------------------

void enamel_init();

void enamel_deinit();

typedef void* EventHandle;
typedef void(EnamelSettingsReceivedHandler)(void* context);

EventHandle enamel_settings_received_subscribe(EnamelSettingsReceivedHandler *handler, void *context);
void enamel_settings_received_unsubscribe(EventHandle handle);

#endif