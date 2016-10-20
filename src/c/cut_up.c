#include <pebble.h>
#include <pebble-fctx/ffont.h>
#include <pebble-fctx/fctx.h>
#include "cut_up.h"
#include "bluetooth.h"
#include "enamel.h"

extern void start();

#define bw_bitmap_data_get_value(BMP, BPR, X, Y) (((*((BMP)+(Y)*(BPR)+(X)/8)) & (1 << (X)%8)) ? 1 : 0)
#define bw_bitmap_data_set_pixel(BMP, BPR, X, Y) (*((BMP)+(Y)*(BPR)+(X)/8)) |= (1 << (X)%8)
#define bw_bitmap_data_clear_pixel(BMP, BPR, X, Y) (*((BMP)+(Y)*(BPR)+(X)/8)) &= ~(1 << (X)%8)

//#define ALTERNATE_ALIGNMENT

enum {
  FONT,
  OUTLINE_FONT
};

typedef struct {
  bool out;
  bool follow_up;
} AnimationProperties;

Settings settings;
char update_text[2][3];

static Window* my_window;
static FFont* font_peace_sans[2];
static FContext fctx;
static GPoint cut[2];
static uint16_t copy_window_start;
static uint16_t copy_window_height;
static Layer* mask_layer;
static GBitmap* mask_bitmap = NULL;
static GBitmap* copy_bitmap = NULL;
static Layer* cutting_layer;
static GPath* cutting_path = NULL;
static GPathInfo cutting_path_info = {
  .num_points = 4,
  .points = (GPoint[]) {{0, 0}, {0, 0}, {0, 0}, {0, 0}}
};
static GPoint animation_points[3];
static AnimationProperties animation_properties[2];

static Layer* top_layer;
static Layer* top_copy_layer;
static char top_text[3] = "23";

static Layer* bottom_layer;
static Layer* bottom_copy_layer;
static char bottom_text[3] = "59";

static Layer* cut_layer;

static int16_t unobstructed_offset = 0;
static bool is_in_focus = false;

static void mask_layer_update_proc(Layer* layer, GContext* ctx) {
  // create mask bitmap only when needed
  if(!mask_bitmap) {
    // draw the black & white mask
    graphics_context_set_antialiased(ctx, false);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, GRect(0, copy_window_start, layer_get_bounds(layer).size.w, copy_window_height), 0, 0);
    graphics_context_set_fill_color(ctx, GColorBlack);
    gpath_draw_filled(ctx, cutting_path);
    // create the mask based on the current framebuffer
    GBitmap* buffer = graphics_capture_frame_buffer(ctx);
    GSize buffer_size = gbitmap_get_bounds(buffer).size;
    mask_bitmap = gbitmap_create_blank(GSize(buffer_size.w, copy_window_height), GBitmapFormat1Bit);
#ifdef PBL_COLOR
    uint16_t mask_bytes_per_row = gbitmap_get_bytes_per_row(mask_bitmap);
    for(int i = copy_window_start; i < copy_window_start+copy_window_height; ++i) {
      GBitmapDataRowInfo row_info = gbitmap_get_data_row_info(buffer, i+unobstructed_offset);
      for(int j = row_info.min_x; j <= row_info.max_x; ++j) {
        if(row_info.data[j] == GColorWhite.argb) {
          bw_bitmap_data_set_pixel(gbitmap_get_data(mask_bitmap), mask_bytes_per_row, j, i-copy_window_start);
        }
      }
    }
#else
    uint16_t bytes_per_row = gbitmap_get_bytes_per_row(buffer);
    memcpy(gbitmap_get_data(mask_bitmap), &(gbitmap_get_data(buffer)[(copy_window_start+unobstructed_offset)*bytes_per_row]), copy_window_height*bytes_per_row);
#endif
    graphics_release_frame_buffer(ctx, buffer);
  }

  // draw the background to it is there even if the text layers are moved
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_antialiased(ctx, false);
  graphics_context_set_fill_color(ctx, settings.color_background_top);
  graphics_fill_rect(ctx, bounds, 0, 0);
  graphics_context_set_fill_color(ctx, settings.color_background_bottom);
  gpath_draw_filled(ctx, cutting_path);
  graphics_fill_rect(ctx, GRect(0, cut[0].y, bounds.size.w, bounds.size.h-cut[1].y), 0, 0);
}

static void draw_text_with_fctx(GContext* ctx, GPoint origin, char* text, uint8_t font, int16_t x, int16_t y, GColor color, GTextAlignment text_alignment, FTextAnchor text_anchor) {
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, font_peace_sans[font], PBL_DISPLAY_WIDTH > 180 ? 140 : 105);
  fctx_set_fill_color(&fctx, color);
  fctx_set_pivot(&fctx, FPointZero);
  fctx_set_offset(&fctx, FPointI(x+origin.x+((GTextAlignmentLeft == text_alignment ? 1 : -1)*(' ' == text[0] ? PBL_DISPLAY_WIDTH/4 : 0)), y+origin.y+unobstructed_offset));
  fctx_set_rotation(&fctx, 0);
  fctx_draw_string(&fctx, text, font_peace_sans[font], text_alignment, text_anchor);
  fctx_end_fill(&fctx);
}

static void draw_cut_line_with_fctx(GContext* ctx, int16_t top_offset, int16_t bottom_offset, GColor color) {
  fctx_begin_fill(&fctx);
  fctx_set_fill_color(&fctx, color);
  fctx_move_to(&fctx, FPointI(cut[0].x, cut[0].y+top_offset+unobstructed_offset));
  fctx_line_to(&fctx, FPointI(cut[1].x, cut[1].y+top_offset+unobstructed_offset));
  fctx_line_to(&fctx, FPointI(cut[1].x, cut[1].y+bottom_offset+unobstructed_offset));
  fctx_line_to(&fctx, FPointI(cut[0].x, cut[0].y+bottom_offset+unobstructed_offset));
  fctx_line_to(&fctx, FPointI(cut[0].x, cut[0].y+top_offset+unobstructed_offset));
  fctx_end_fill(&fctx);
}

static void cutting_layer_update_proc(Layer* layer, GContext* ctx) {
  // cut off the bottom by painting over it in the background color
  graphics_context_set_fill_color(ctx, settings.color_background_bottom);
  graphics_context_set_antialiased(ctx, false);
  gpath_draw_filled(ctx, cutting_path);
  GRect bounds = layer_get_frame(layer);
  bounds.origin.y = cut[0].y;
  bounds.size.h -= cut[0].y;
  graphics_fill_rect(ctx, bounds, 0, 0);
}

static void top_layer_update_proc(Layer* layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(layer);

  // draw the background
  graphics_context_set_fill_color(ctx, settings.color_background_top);
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, copy_window_start+copy_window_height), 0, 0);

  // init fctx once
  fctx_init_context(&fctx, ctx);

  // draw the cuting line border
  if(!gcolor_equal(settings.color_the_cut_outline_top, settings.color_background_top)) {
    draw_cut_line_with_fctx(ctx, PBL_DISPLAY_WIDTH > 180 ? -5 : -4, -1, settings.color_the_cut_outline_top);
  }

  // draw the text
  if(!gcolor_equal(settings.color_text_top, settings.color_background_top) ||
     !gcolor_equal(settings.color_the_cut_outline_top, settings.color_background_top)) {
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND
    draw_text_with_fctx(ctx, bounds.origin, top_text, FONT, bounds.size.w-33, bounds.size.h/2+40, settings.color_text_top, GTextAlignmentRight, FTextAnchorBottom);
#else
    draw_text_with_fctx(ctx, bounds.origin, top_text, FONT, bounds.size.w-8, bounds.size.h/2+bounds.size.h/4, settings.color_text_top, GTextAlignmentRight, FTextAnchorBottom);
#endif
#else
#ifdef PBL_ROUND
    draw_text_with_fctx(ctx, bounds.origin, top_text, FONT, 11, bounds.size.h/2+40, settings.color_text_top, GTextAlignmentLeft, FTextAnchorBottom);
#else
    draw_text_with_fctx(ctx, bounds.origin, top_text, FONT, 0, bounds.size.h/2+bounds.size.h/4, settings.color_text_top, GTextAlignmentLeft, FTextAnchorBottom);
    //draw_text_with_fctx(ctx, bounds.origin, top_text, FONT, 0, bounds.size.h/2+40, settings.color_text_top, GTextAlignmentLeft, FTextAnchorBottom);
#endif
#endif
  }
  if(!gcolor_equal(settings.color_text_outline_top, settings.color_text_top)) {
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND
    draw_text_with_fctx(ctx, bounds.origin, top_text, OUTLINE_FONT, bounds.size.w-33, bounds.size.h/2+40, settings.color_text_outline_top, GTextAlignmentRight, FTextAnchorBottom);
#else
    draw_text_with_fctx(ctx, bounds.origin, top_text, OUTLINE_FONT, bounds.size.w-8, bounds.size.h/2+(bounds.size.h*10)/42, settings.color_text_outline_top, GTextAlignmentRight, FTextAnchorBottom);
#endif
#else
#ifdef PBL_ROUND
    draw_text_with_fctx(ctx, bounds.origin, top_text, OUTLINE_FONT, 11, bounds.size.h/2+40, settings.color_text_outline_top, GTextAlignmentLeft, FTextAnchorBottom);
#else
    draw_text_with_fctx(ctx, bounds.origin, top_text, OUTLINE_FONT, 0, bounds.size.h/2+(bounds.size.h*10)/42, settings.color_text_outline_top, GTextAlignmentLeft, FTextAnchorBottom);
    //draw_text_with_fctx(ctx, bounds.origin, top_text, OUTLINE_FONT, 0, bounds.size.h/2+40, settings.color_text_outline_top, GTextAlignmentLeft, FTextAnchorBottom);
#endif
#endif
  }
  // deinit fctx once
  fctx_deinit_context(&fctx);
}

static void top_copy_layer_update_proc(Layer* layer, GContext* ctx) {
  GBitmap* buffer = graphics_capture_frame_buffer(ctx);
  GSize buffer_size = gbitmap_get_bounds(buffer).size;
  // create backbuffer bitmap only once
  if(!copy_bitmap) {
    copy_bitmap = gbitmap_create_blank(GSize(buffer_size.w, copy_window_height), PBL_IF_COLOR_ELSE(GBitmapFormat8Bit, GBitmapFormat1Bit));
  }
  // copy the center window from the framebuffer into the backbuffer bitmap
#ifdef PBL_COLOR
  for(int i = copy_window_start; i < copy_window_start+copy_window_height; ++i) {
    GBitmapDataRowInfo from_row_info = gbitmap_get_data_row_info(buffer, i+unobstructed_offset);
    GBitmapDataRowInfo to_row_info = gbitmap_get_data_row_info(copy_bitmap, i-copy_window_start);
    for(int j = from_row_info.min_x; j <= from_row_info.max_x; ++j) {
      to_row_info.data[j] = from_row_info.data[j];
    }
  }
#else
  uint16_t bytes_per_row = gbitmap_get_bytes_per_row(buffer);
  memcpy(gbitmap_get_data(copy_bitmap), &(gbitmap_get_data(buffer)[(copy_window_start+unobstructed_offset)*bytes_per_row]), copy_window_height*bytes_per_row);
#endif
  graphics_release_frame_buffer(ctx, buffer);
}

static void bottom_layer_update_proc(Layer* layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(layer);

  // draw the background
  graphics_context_set_fill_color(ctx, settings.color_background_bottom);
  gpath_draw_filled(ctx, cutting_path);

  // init fctx once
  fctx_init_context(&fctx, ctx);

  // draw the cuting line border
  if(!gcolor_equal(settings.color_the_cut_outline_bottom, settings.color_background_bottom)) {
    draw_cut_line_with_fctx(ctx, PBL_DISPLAY_WIDTH > 180 ? 3 : 2, PBL_DISPLAY_WIDTH > 180 ?  7 :  5, settings.color_the_cut_outline_bottom);
  }

  // draw the text
  if(!gcolor_equal(settings.color_text_bottom, settings.color_background_bottom) ||
     !gcolor_equal(settings.color_the_cut_outline_bottom, settings.color_background_bottom)) {
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND
    draw_text_with_fctx(ctx, bounds.origin, bottom_text, FONT, bounds.size.w-147, bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#else
    draw_text_with_fctx(ctx, bounds.origin, bottom_text, FONT, 10, bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#endif
#else
#ifdef PBL_ROUND
    draw_text_with_fctx(ctx, bounds.origin, bottom_text, FONT, bounds.size.w-12, bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#else
    draw_text_with_fctx(ctx, bounds.origin, bottom_text, FONT, bounds.size.w, bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#endif
#endif
  }
  if(!gcolor_equal(settings.color_text_outline_bottom, settings.color_text_bottom)) {
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND
    draw_text_with_fctx(ctx, bounds.origin, bottom_text, OUTLINE_FONT, bounds.size.w-147, bounds.size.h/2-12, settings.color_text_outline_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#else
    draw_text_with_fctx(ctx, bounds.origin, bottom_text, OUTLINE_FONT,  10, bounds.size.h/2-bounds.size.h/14, settings.color_text_outline_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#endif
#else
#ifdef PBL_ROUND
    draw_text_with_fctx(ctx, bounds.origin, bottom_text, OUTLINE_FONT, bounds.size.w-12, bounds.size.h/2-12, settings.color_text_outline_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#else
    draw_text_with_fctx(ctx, bounds.origin, bottom_text, OUTLINE_FONT, bounds.size.w, bounds.size.h/2-bounds.size.h/14, settings.color_text_outline_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#endif
#endif
  }
  // deinit fctx once
  fctx_deinit_context(&fctx);
}

static void bottom_copy_layer_update_proc(Layer* layer, GContext* ctx) {
  // prepare the mask
  uint16_t mask_bytes_per_row = gbitmap_get_bytes_per_row(mask_bitmap);
  uint8_t* mask_bytes = gbitmap_get_data(mask_bitmap);
  GBitmap* buffer = graphics_capture_frame_buffer(ctx);
#ifdef PBL_BW
  GSize buffer_size = gbitmap_get_bounds(buffer).size;
#endif
  // copy from the backbuffer bitmap in the framebuffer
  for(int i = copy_window_start; i < copy_window_start+copy_window_height; ++i) {
#ifdef PBL_COLOR
    GBitmapDataRowInfo from_row_info = gbitmap_get_data_row_info(copy_bitmap, i-copy_window_start);
    GBitmapDataRowInfo to_row_info = gbitmap_get_data_row_info(buffer, i+unobstructed_offset);
    for(int j = from_row_info.min_x; j <= from_row_info.max_x; ++j) {
      // use the mask to leave the relevant pixels from the bottom layer intact
      if(bw_bitmap_data_get_value(mask_bytes, mask_bytes_per_row, j, i-copy_window_start)) {
        to_row_info.data[j] = from_row_info.data[j];
      }
    }
#else
    uint16_t bytes_per_row = gbitmap_get_bytes_per_row(buffer);
    uint8_t* bytes = gbitmap_get_data(buffer);
    uint16_t copy_bytes_per_row = gbitmap_get_bytes_per_row(copy_bitmap);
    uint8_t* copy_bytes = gbitmap_get_data(copy_bitmap);
    for(int j = 0; j < buffer_size.w; ++j) {
      // use the mask to leave the relevant pixels from the bottom layer intact
      if(bw_bitmap_data_get_value(mask_bytes, mask_bytes_per_row, j, i-copy_window_start)) {
        if(bw_bitmap_data_get_value(copy_bytes, copy_bytes_per_row, j, i-copy_window_start)) {
          bw_bitmap_data_set_pixel(bytes, bytes_per_row, j, i+unobstructed_offset);
        } else {
          bw_bitmap_data_clear_pixel(bytes, bytes_per_row, j, i+unobstructed_offset);
        }
      }
    }
#endif
  }
  graphics_release_frame_buffer(ctx, buffer);
}

static void cut_layer_update_proc(Layer* layer, GContext* ctx) {
  // draw the cuting line
  fctx_init_context(&fctx, ctx);
  draw_cut_line_with_fctx(ctx, -1, PBL_DISPLAY_WIDTH > 180 ?  3 :  2, bluetooth_connected() ? settings.color_the_cut : settings.color_the_cut_disconnected);
  fctx_deinit_context(&fctx);
  
#if 0 // center lines to help with adjustment
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_context_set_stroke_color(ctx, GColorYellow);
  graphics_draw_line(ctx, GPoint(0, bounds.size.h/2), GPoint(bounds.size.w, bounds.size.h/2));
  graphics_draw_line(ctx, GPoint(bounds.size.w/2, 0), GPoint(bounds.size.w/2, bounds.size.h));
#endif
#if 0 // copy window frame for debugging
  graphics_context_set_stroke_width(ctx, 1);
  graphics_context_set_stroke_color(ctx, GColorYellow);
  graphics_draw_rect(ctx, GRect(0, copy_window_start+unobstructed_offset, PBL_DISPLAY_WIDTH, copy_window_height));
#endif
}

#if PBL_API_EXISTS(unobstructed_area_service_subscribe) || defined PEEK_TEST
static void unobstructed_area_change(AnimationProgress progress, void* context) {
  Layer* root_layer = window_get_root_layer(my_window);
  GRect root_bounds = layer_get_bounds(root_layer);
  unobstructed_offset = -(root_bounds.size.h/2-layer_get_unobstructed_bounds(root_layer).size.h/2);
  root_bounds.origin.y += unobstructed_offset;
  layer_set_frame(mask_layer, root_bounds);
}
#endif // PBL_API_EXISTS(unobstructed_area_service_subscribe)

static void animate(bool top, bool in, bool follow_up);
static void animation_stopped(Animation *animation, bool finished, void *context) {
  int index = (int)context;
  if(animation_properties[index].out && animation_properties[index].follow_up) {
    if(0 == index) {
      layer_set_bounds(top_layer, GRect(animation_points[2].x, animation_points[2].y, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT));
      memcpy(top_text, update_text[0], 2);
    }else {
      layer_set_bounds(bottom_layer, GRect(animation_points[1].x, animation_points[1].y, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT));
      memcpy(bottom_text, update_text[1], 2);
    }
    animate(0 == index, true, true);
  }
}

static void animate(bool top, bool in, bool follow_up) {
  PropertyAnimation* animation;
  animation = property_animation_create_bounds_origin(top ? top_layer : bottom_layer, NULL, in ? &animation_points[0] : top ? &animation_points[1] : &animation_points[2] );
  animation_set_duration((Animation*)animation, 2*PBL_DISPLAY_WIDTH);
  animation_set_curve((Animation*)animation, (!in || top) ? AnimationCurveEaseIn : AnimationCurveEaseOut);
  animation_properties[top ? 0 : 1].out = !in;
  animation_properties[top ? 0 : 1].follow_up = follow_up;
  animation_set_handlers((Animation*)animation, (AnimationHandlers) {
    .stopped = animation_stopped
  }, (void*)(top ? 0 : 1));
  animation_schedule((Animation*)animation);
}

static void did_focus_handler(bool in_focus) {
  // update if the focus has been regained
  if(in_focus) {
    if(ANIMATIONS_STARTUP_AND_TRANSITIONS != settings.animations) {
      app_focus_service_unsubscribe();
    }
    is_in_focus = in_focus;
    animate(true, true, false);
    animate(false, true, false);
  }
}

static void will_focus_handler(bool in_focus) {
  // update when the watchface is about to lose focus
  if(!in_focus) {
    is_in_focus = in_focus;
    animate(true, false, false);
    animate(false, false, false);
  }
}

static void my_window_load(Window *window) {
  Layer* root_layer = window_get_root_layer(window);
  GRect root_bounds = layer_get_bounds(root_layer);

  // filled font and outline font
  font_peace_sans[FONT] = ffont_create_from_resource(RESOURCE_ID_FONT_PEACE_SANS);
  font_peace_sans[OUTLINE_FONT] = ffont_create_from_resource(RESOURCE_ID_FONT_PEACE_SANS_OUTLINE);

#if PBL_API_EXISTS(unobstructed_area_service_subscribe) || defined PEEK_TEST
  GRect unobstructed_bounds = layer_get_unobstructed_bounds(root_layer);
  unobstructed_offset = -(root_bounds.size.h/2-unobstructed_bounds.size.h/2);

  UnobstructedAreaHandlers handlers = {
    .change = unobstructed_area_change,
  };
  unobstructed_area_service_subscribe(handlers, NULL);
#endif // PBL_API_EXISTS(unobstructed_area_service_subscribe)

  // the endpoints of the cutting line
  cut[0] = GPoint(0, root_bounds.size.h/2+10);
  cut[1] = GPoint(root_bounds.size.w, root_bounds.size.h/2-10);

  // this is fine tuned to save RAM, it only needs to cover the overlapping area
  copy_window_start = cut[1].y-17;
  copy_window_height = cut[0].y-copy_window_start;

  // create the cutting path
  cutting_path_info.points[0] = cut[0];
  cutting_path_info.points[1] = cut[1];
  cutting_path_info.points[2] = GPoint(root_bounds.size.w, copy_window_start+copy_window_height);
  cutting_path_info.points[3] = GPoint(0, copy_window_start+copy_window_height);
  cutting_path = gpath_create(&cutting_path_info);

  // setup the animation start and end points
  animation_points[0] = GPointZero;
  animation_points[1] = GPoint(-root_bounds.size.w, 20);
  animation_points[2] = GPoint(root_bounds.size.w, -20);

  // creates a mask to do the cutting
  mask_layer = layer_create(GRect(root_bounds.origin.x, root_bounds.origin.y+unobstructed_offset, root_bounds.size.w, root_bounds.size.h));
  layer_set_update_proc(mask_layer, mask_layer_update_proc);
  layer_add_child(root_layer, mask_layer);

  // draws the top text
  top_layer = layer_create(root_bounds);
  layer_set_update_proc(top_layer, top_layer_update_proc);
  layer_add_child(mask_layer, top_layer);

  // cuts the bottom of the top text
  cutting_layer = layer_create(root_bounds);
  layer_set_update_proc(cutting_layer, cutting_layer_update_proc);
  layer_add_child(mask_layer, cutting_layer);

  // copies the cut top text to backbuffer
  top_copy_layer = layer_create(root_bounds);
  layer_set_update_proc(top_copy_layer, top_copy_layer_update_proc);
  layer_add_child(mask_layer, top_copy_layer);

  // draws the bottom text
  bottom_layer = layer_create(root_bounds);
  layer_set_update_proc(bottom_layer, bottom_layer_update_proc);
  layer_add_child(mask_layer, bottom_layer);

  // copies the cut top text from the backbuffer over the bottom text
  bottom_copy_layer = layer_create(root_bounds);
  layer_set_update_proc(bottom_copy_layer, bottom_copy_layer_update_proc);
  layer_add_child(mask_layer, bottom_copy_layer);

  // draws the cut line
  cut_layer = layer_create(root_bounds);
  layer_set_update_proc(cut_layer, cut_layer_update_proc);
  layer_add_child(mask_layer, cut_layer);

  if(settings.animations) {
    layer_set_bounds(top_layer, GRect(animation_points[2].x, animation_points[2].y, root_bounds.size.w, root_bounds.size.h));
    layer_set_bounds(bottom_layer, GRect(animation_points[1].x, animation_points[1].y, root_bounds.size.w, root_bounds.size.h));
    app_focus_service_subscribe_handlers((AppFocusHandlers) {
      .did_focus = did_focus_handler,
      .will_focus = will_focus_handler,
    });
  }

  bluetooth_init();
  start();
}

void cut_up_update(bool top, bool bottom) {
  if(ANIMATIONS_STARTUP_AND_TRANSITIONS == settings.animations && (top || bottom) && is_in_focus) {
    if(top) {
      animate(true, false, true);
    }
    if(bottom) {
      animate(false, false, true);
    }
  } else {
    memcpy(top_text, update_text[0], 2);
    memcpy(bottom_text, update_text[1], 2);
  }
  layer_mark_dirty(window_get_root_layer(my_window));
}

void cut_up_init() {
  my_window = window_create();
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = my_window_load,
    //.unload = TODO
  });
  window_stack_push(my_window, true);
}

void cut_up_deinit() {
  bluetooth_deinit();
  window_destroy(my_window);
}

void bluetooth_listener(bool connected) {
  cut_up_update(false, false);
}
