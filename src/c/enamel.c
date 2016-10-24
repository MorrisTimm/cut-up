/**
 * This file was generated with Enamel : http://gregoiresage.github.io/enamel
 */

#include <pebble.h>
#include <@smallstoneapps/linked-list/linked-list.h>
#include <pebble-events/pebble-events.h>
#include "enamel.h"

#ifndef ENAMEL_MAX_STRING_LENGTH
#define ENAMEL_MAX_STRING_LENGTH 100
#endif

#define ENAMEL_PKEY 3000000000
#define ENAMEL_DICT_PKEY (ENAMEL_PKEY+1)

typedef struct {
	EnamelSettingsReceivedHandler *handler;
	void *context;
} SettingsReceivedState;

static LinkedRoot *s_handler_list;

static EventHandle s_event_handle;

static DictionaryIterator s_dict;
static uint8_t* s_dict_buffer = NULL;
static uint32_t s_dict_size = 0;

static bool s_config_changed;

// -----------------------------------------------------
// Getter for 'value_top'
VALUE_TOPValue enamel_get_value_top(){
	Tuple* tuple = dict_find(&s_dict, 1302333654);
	return tuple ? atoi(tuple->value->cstring) : 0;
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_background_top'
GColor enamel_get_color_background_top(){
	Tuple* tuple = dict_find(&s_dict, 3282856444);
	return tuple ? GColorFromHEX(tuple->value->int32) : GColorFromHEX(0x000000);
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_text_top'
GColor enamel_get_color_text_top(){
	Tuple* tuple = dict_find(&s_dict, 3825572583);
	return tuple ? GColorFromHEX(tuple->value->int32) : GColorFromHEX(0xFFFFFF);
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_text_outline_top'
GColor enamel_get_color_text_outline_top(){
	Tuple* tuple = dict_find(&s_dict, 2679461552);
	return tuple ? GColorFromHEX(tuple->value->int32) : GColorFromHEX(0xFFFFFF);
}
// -----------------------------------------------------

#if !defined(PBL_PLATFORM_APLITE)
// -----------------------------------------------------
// Getter for 'offset_y_text_top'
int32_t enamel_get_offset_y_text_top(){
	Tuple* tuple = dict_find(&s_dict, 711660296);
		
	return tuple ? tuple->value->int32 : 0;
}
// -----------------------------------------------------
#endif

// -----------------------------------------------------
// Getter for 'color_the_cut_outline_top'
GColor enamel_get_color_the_cut_outline_top(){
	Tuple* tuple = dict_find(&s_dict, 1805845070);
	return tuple ? GColorFromHEX(tuple->value->int32) : GColorFromHEX(0x000000);
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_the_cut'
GColor enamel_get_color_the_cut(){
	Tuple* tuple = dict_find(&s_dict, 1009183225);
	return tuple ? GColorFromHEX(tuple->value->int32) : GColorFromHEX(0x000000);
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_the_cut_disconnected'
GColor enamel_get_color_the_cut_disconnected(){
	Tuple* tuple = dict_find(&s_dict, 1436604224);
	return tuple ? GColorFromHEX(tuple->value->int32) : GColorFromHEX(0xFF0000);
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_the_cut_outline_bottom'
GColor enamel_get_color_the_cut_outline_bottom(){
	Tuple* tuple = dict_find(&s_dict, 1636910343);
	return tuple ? GColorFromHEX(tuple->value->int32) : GColorFromHEX(0x000000);
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'value_bottom'
VALUE_BOTTOMValue enamel_get_value_bottom(){
	Tuple* tuple = dict_find(&s_dict, 4275626655);
	return tuple ? atoi(tuple->value->cstring) : 1;
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_background_bottom'
GColor enamel_get_color_background_bottom(){
	Tuple* tuple = dict_find(&s_dict, 427728357);
	return tuple ? GColorFromHEX(tuple->value->int32) : GColorFromHEX(0x000000);
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_text_bottom'
GColor enamel_get_color_text_bottom(){
	Tuple* tuple = dict_find(&s_dict, 1975043952);
	return tuple ? GColorFromHEX(tuple->value->int32) : GColorFromHEX(0x55AAFF);
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'color_text_outline_bottom'
GColor enamel_get_color_text_outline_bottom(){
	Tuple* tuple = dict_find(&s_dict, 3680561045);
	return tuple ? GColorFromHEX(tuple->value->int32) : GColorFromHEX(0x55AAFF);
}
// -----------------------------------------------------

#if !defined(PBL_PLATFORM_APLITE)
// -----------------------------------------------------
// Getter for 'offset_y_text_bottom'
int32_t enamel_get_offset_y_text_bottom(){
	Tuple* tuple = dict_find(&s_dict, 1299163365);
		
	return tuple ? tuple->value->int32 : 0;
}
// -----------------------------------------------------
#endif

// -----------------------------------------------------
// Getter for 'animations'
ANIMATIONSValue enamel_get_animations(){
	Tuple* tuple = dict_find(&s_dict, 2502829527);
	return tuple ? atoi(tuple->value->cstring) : 1;
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'show_leading_zeroes'
SHOW_LEADING_ZEROESValue enamel_get_show_leading_zeroes(){
	Tuple* tuple = dict_find(&s_dict, 1614853234);
	return tuple ? atoi(tuple->value->cstring) : 0;
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'quick_launch_timeout'
int32_t enamel_get_quick_launch_timeout(){
	Tuple* tuple = dict_find(&s_dict, 3053448009);
	return tuple ? tuple->value->int32 : 20;
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'show_time_on_start'
bool enamel_get_show_time_on_start(){
	Tuple* tuple = dict_find(&s_dict, 2230560336);
	return tuple ? tuple->value->int32 == 1 : true;
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'show_leading_zeroes_on_time'
SHOW_LEADING_ZEROES_ON_TIMEValue enamel_get_show_leading_zeroes_on_time(){
	Tuple* tuple = dict_find(&s_dict, 1956971886);
	return tuple ? atoi(tuple->value->cstring) : 0;
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'first_day_of_the_week'
FIRST_DAY_OF_THE_WEEKValue enamel_get_first_day_of_the_week(){
	Tuple* tuple = dict_find(&s_dict, 1349210911);
	return tuple ? atoi(tuple->value->cstring) : 0;
}
// -----------------------------------------------------


static uint16_t prv_get_inbound_size() {
	return 1
		+ 7 + 2
		+ 7 + 4
		+ 7 + 4
		+ 7 + 4
#if !defined(PBL_PLATFORM_APLITE)
		+ 7 + 4
#endif
		+ 7 + 4
		+ 7 + 4
		+ 7 + 4
		+ 7 + 4
		+ 7 + 2
		+ 7 + 4
		+ 7 + 4
		+ 7 + 4
#if !defined(PBL_PLATFORM_APLITE)
		+ 7 + 4
#endif
		+ 7 + 2
		+ 7 + 2
		+ 7 + 4
		+ 7 + 4
		+ 7 + 2
		+ 7 + 2
;
}

static uint32_t prv_map_messagekey(const uint32_t key){
	if( key == MESSAGE_KEY_value_top) return 1302333654;
	if( key == MESSAGE_KEY_color_background_top) return 3282856444;
	if( key == MESSAGE_KEY_color_text_top) return 3825572583;
	if( key == MESSAGE_KEY_color_text_outline_top) return 2679461552;
#if !defined(PBL_PLATFORM_APLITE)
	if( key == MESSAGE_KEY_offset_y_text_top) return 711660296;
#endif
	if( key == MESSAGE_KEY_color_the_cut_outline_top) return 1805845070;
	if( key == MESSAGE_KEY_color_the_cut) return 1009183225;
	if( key == MESSAGE_KEY_color_the_cut_disconnected) return 1436604224;
	if( key == MESSAGE_KEY_color_the_cut_outline_bottom) return 1636910343;
	if( key == MESSAGE_KEY_value_bottom) return 4275626655;
	if( key == MESSAGE_KEY_color_background_bottom) return 427728357;
	if( key == MESSAGE_KEY_color_text_bottom) return 1975043952;
	if( key == MESSAGE_KEY_color_text_outline_bottom) return 3680561045;
#if !defined(PBL_PLATFORM_APLITE)
	if( key == MESSAGE_KEY_offset_y_text_bottom) return 1299163365;
#endif
	if( key == MESSAGE_KEY_animations) return 2502829527;
	if( key == MESSAGE_KEY_show_leading_zeroes) return 1614853234;
	if( key == MESSAGE_KEY_quick_launch_timeout) return 3053448009;
	if( key == MESSAGE_KEY_show_time_on_start) return 2230560336;
	if( key == MESSAGE_KEY_show_leading_zeroes_on_time) return 1956971886;
	if( key == MESSAGE_KEY_first_day_of_the_week) return 1349210911;
	return 0;
}

static void prv_key_update_cb(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context){
}

static bool prv_each_settings_received(void *this, void *context) {
	SettingsReceivedState *state=(SettingsReceivedState *)this;
	state->handler(state->context);
	return true;
}

static void prv_inbox_received_handle(DictionaryIterator *iter, void *context) {
	if(dict_find(iter, MESSAGE_KEY_value_top)){
		if(s_dict_buffer){
			free(s_dict_buffer);
			s_dict_buffer = NULL;
		}
		s_dict_size = dict_size(iter);
		s_dict_buffer = malloc(s_dict_size);

		Tuple *tuple=dict_read_first(iter);
		while(tuple){
			tuple->key = prv_map_messagekey(tuple->key);
			tuple=dict_read_next(iter);
		}

		dict_write_begin(&s_dict, s_dict_buffer, s_dict_size);
		dict_write_end(&s_dict);
		dict_merge(&s_dict, &s_dict_size, iter, false, prv_key_update_cb, NULL);

		if(s_handler_list){
			linked_list_foreach(s_handler_list, prv_each_settings_received, NULL);
		}

		s_config_changed = true;
	}
}

static uint16_t prv_save_generic_data(uint32_t startkey, const void *data, uint16_t size){
	uint16_t offset = 0;
	uint16_t total_w_bytes = 0;
	uint16_t w_bytes = 0;
	while(offset < size){
		w_bytes = size - offset < PERSIST_DATA_MAX_LENGTH ? size - offset : PERSIST_DATA_MAX_LENGTH;
		w_bytes = persist_write_data(startkey + offset / PERSIST_DATA_MAX_LENGTH, data + offset, w_bytes);
		total_w_bytes += w_bytes;
		offset += PERSIST_DATA_MAX_LENGTH;
	}
	return total_w_bytes; 
}

static uint16_t prv_load_generic_data(uint32_t startkey, void *data, uint16_t size){
	uint16_t offset = 0;
	uint16_t total_r_bytes = 0;
	uint16_t expected_r_bytes = 0;
	uint16_t r_bytes = 0;
	while(offset < size){
		if(size - offset > PERSIST_DATA_MAX_LENGTH){
			expected_r_bytes = PERSIST_DATA_MAX_LENGTH;
		}
		else {
			expected_r_bytes = size - offset;
		}
		r_bytes = persist_read_data(startkey + offset / PERSIST_DATA_MAX_LENGTH, data + offset, expected_r_bytes);
		total_r_bytes += r_bytes;
		if(r_bytes != expected_r_bytes){
			break; 
		}
		offset += PERSIST_DATA_MAX_LENGTH;
	}
	return total_r_bytes;
}

void enamel_init(){
	if(persist_exists(ENAMEL_PKEY) && persist_exists(ENAMEL_DICT_PKEY)) 
	{
		s_dict_size = persist_read_int(ENAMEL_PKEY);
		s_dict_buffer = malloc(s_dict_size);
		prv_load_generic_data(ENAMEL_DICT_PKEY, s_dict_buffer, s_dict_size);
	}
	else {
		s_dict_size = 0;
		s_dict_buffer = NULL;
	}

	dict_read_begin_from_buffer(&s_dict, s_dict_buffer, s_dict_size);
	
	s_config_changed = false;
	s_event_handle = events_app_message_register_inbox_received(prv_inbox_received_handle, NULL);
	events_app_message_request_inbox_size(prv_get_inbound_size());
}

void enamel_deinit(){
	if(s_config_changed){
		persist_write_int(ENAMEL_PKEY, s_dict_size);
		prv_save_generic_data(ENAMEL_DICT_PKEY, s_dict_buffer, s_dict_size);
	}

	if(s_dict_buffer){
		free(s_dict_buffer);
		s_dict_buffer = NULL;
	}

	s_config_changed = false;
	events_app_message_unsubscribe(s_event_handle);
}

EventHandle enamel_settings_received_subscribe(EnamelSettingsReceivedHandler *handler, void *context) {
	if (!s_handler_list) {
		s_handler_list = linked_list_create_root();
	}

	SettingsReceivedState *this = malloc(sizeof(SettingsReceivedState));
	this->handler = handler;
	this->context = context;
	linked_list_append(s_handler_list, this);

	return this;
}

void enamel_settings_received_unsubscribe(EventHandle handle) {
	int16_t index = linked_list_find(s_handler_list, handle);
	if (index == -1) {
		return;
	}

	free(linked_list_get(s_handler_list, index));
	linked_list_remove(s_handler_list, index);
	if (linked_list_count(s_handler_list) == 0) {
		free(s_handler_list);
		s_handler_list = NULL;
	}
}