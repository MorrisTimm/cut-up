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

#if !defined(PBL_PLATFORM_APLITE)
// -----------------------------------------------------
// Getter for 'offset_y_text_top'
#define OFFSET_Y_TEXT_TOP_PRECISION 1
int32_t enamel_get_offset_y_text_top();
#endif
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

#if !defined(PBL_PLATFORM_APLITE)
// -----------------------------------------------------
// Getter for 'offset_y_text_bottom'
#define OFFSET_Y_TEXT_BOTTOM_PRECISION 1
int32_t enamel_get_offset_y_text_bottom();
#endif
// -----------------------------------------------------

#if !defined(PBL_PLATFORM_APLITE)
// -----------------------------------------------------
// Getter for 'tap_for_date'
bool enamel_get_tap_for_date();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'tapping_threshold'
#define TAPPING_THRESHOLD_PRECISION 1
int32_t enamel_get_tapping_threshold();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'date_timeout'
#define DATE_TIMEOUT_PRECISION 10
int32_t enamel_get_date_timeout();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'date_format'
typedef enum {
	DATE_FORMAT_MONTH_ON_TOP = 0,
	DATE_FORMAT_MONTH_AT_BOTTOM = 1,
} DATE_FORMATValue;
DATE_FORMATValue enamel_get_date_format();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'date_leading_zeroes'
typedef enum {
	DATE_LEADING_ZEROES_SHOW = 0,
	DATE_LEADING_ZEROES_HIDE_FOR_MONTH = 1,
	DATE_LEADING_ZEROES_HIDE = 2,
} DATE_LEADING_ZEROESValue;
DATE_LEADING_ZEROESValue enamel_get_date_leading_zeroes();
// -----------------------------------------------------

#endif
// -----------------------------------------------------
// Getter for 'animations'
typedef enum {
	ANIMATIONS_OFF = 0,
	ANIMATIONS_ON = 1,
	ANIMATIONS_STARTUP_ONLY = 2,
	ANIMATIONS_STARTUP_AND_TRANSITIONS = 3,
} ANIMATIONSValue;
ANIMATIONSValue enamel_get_animations();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'show_leading_zeroes'
typedef enum {
	SHOW_LEADING_ZEROES_SHOW = 0,
	SHOW_LEADING_ZEROES_HIDE_FOR_HOURS = 1,
	SHOW_LEADING_ZEROES_HIDE = 2,
} SHOW_LEADING_ZEROESValue;
SHOW_LEADING_ZEROESValue enamel_get_show_leading_zeroes();
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