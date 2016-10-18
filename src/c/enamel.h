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
// Getter for 'color_the_cut'
GColor enamel_get_color_the_cut();
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

void enamel_init();

void enamel_deinit();

typedef void* EventHandle;
typedef void(EnamelSettingsReceivedHandler)(void* context);

EventHandle enamel_settings_received_subscribe(EnamelSettingsReceivedHandler *handler, void *context);
void enamel_settings_received_unsubscribe(EventHandle handle);

#endif