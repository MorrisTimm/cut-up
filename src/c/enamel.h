/**
 * This file was generated with Enamel : http://gregoiresage.github.io/enamel
 */

#ifndef ENAMEL_H
#define ENAMEL_H

#include <pebble.h>

// -----------------------------------------------------
// Getter for 'value_top'
typedef enum {
	VALUE_TOP_DAY_OF_THE_MONTH = 0,
	VALUE_TOP_MONTH = 1,
	VALUE_TOP_YEAR = 2,
	VALUE_TOP_BATTERY_LEVEL = 3,
	VALUE_TOP_CALENDAR_WEEK__ISO_8601_ = 4,
	VALUE_TOP_CALENDAR_WEEK__US_ = 5,
	VALUE_TOP_DAY_OF_THE_WEEK = 6,
} VALUE_TOPValue;
VALUE_TOPValue enamel_get_value_top();
// -----------------------------------------------------

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
// Getter for 'value_bottom'
typedef enum {
	VALUE_BOTTOM_DAY_OF_THE_MONTH = 0,
	VALUE_BOTTOM_MONTH = 1,
	VALUE_BOTTOM_YEAR = 2,
	VALUE_BOTTOM_BATTERY_LEVEL = 3,
	VALUE_BOTTOM_CALENDAR_WEEK__ISO_8601_ = 4,
	VALUE_BOTTOM_CALENDAR_WEEK__US_ = 5,
	VALUE_BOTTOM_DAY_OF_THE_WEEK = 6,
} VALUE_BOTTOMValue;
VALUE_BOTTOMValue enamel_get_value_bottom();
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
// Getter for 'show_leading_zeroes'
typedef enum {
	SHOW_LEADING_ZEROES_SHOW = 0,
	SHOW_LEADING_ZEROES_HIDE_ON_TOP = 1,
	SHOW_LEADING_ZEROES_HIDE_ON_BOTTOM = 2,
	SHOW_LEADING_ZEROES_HIDE = 3,
} SHOW_LEADING_ZEROESValue;
SHOW_LEADING_ZEROESValue enamel_get_show_leading_zeroes();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'quick_launch_timeout'
#define QUICK_LAUNCH_TIMEOUT_PRECISION 10
int32_t enamel_get_quick_launch_timeout();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'first_day_of_the_week'
typedef enum {
	FIRST_DAY_OF_THE_WEEK_MONDAY = 0,
	FIRST_DAY_OF_THE_WEEK_SUNDAY = 1,
} FIRST_DAY_OF_THE_WEEKValue;
FIRST_DAY_OF_THE_WEEKValue enamel_get_first_day_of_the_week();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'show_time_on_start'
bool enamel_get_show_time_on_start();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'show_leading_zeroes_on_time'
typedef enum {
	SHOW_LEADING_ZEROES_ON_TIME_SHOW = 0,
	SHOW_LEADING_ZEROES_ON_TIME_HIDE_FOR_HOURS = 1,
	SHOW_LEADING_ZEROES_ON_TIME_HIDE = 2,
} SHOW_LEADING_ZEROES_ON_TIMEValue;
SHOW_LEADING_ZEROES_ON_TIMEValue enamel_get_show_leading_zeroes_on_time();
// -----------------------------------------------------

void enamel_init();

void enamel_deinit();

typedef void* EventHandle;
typedef void(EnamelSettingsReceivedHandler)(void* context);

EventHandle enamel_settings_received_subscribe(EnamelSettingsReceivedHandler *handler, void *context);
void enamel_settings_received_unsubscribe(EventHandle handle);

#endif