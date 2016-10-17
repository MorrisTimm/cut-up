#include <pebble.h>
#include <pebble-fctx/ffont.h>
#include <pebble-fctx/fctx.h>
#include "cutting_edge.h"

extern void start();

#ifdef PBL_BW
#define bw_bitmap_data_get_value(BMP, BPR, X, Y) (((*((BMP)+(Y)*(BPR)+(X)/8)) & (1 << (X)%8)) ? 1 : 0)
#define bw_bitmap_data_set_pixel(BMP, BPR, X, Y) (*((BMP)+(Y)*(BPR)+(X)/8)) |= (1 << (X)%8)
#endif

//#define ALTERNATE_ALIGNMENT

Settings settings;
static Window* my_window;
static FFont* font_peace_sans[2];
static FContext fctx;
GPoint cut[2];
static uint16_t copy_window_start;
static uint16_t copy_window_height;
static GBitmap* copy_bitmap = NULL;
static BitmapLayer* bitmap_layer;

static Layer* top_layer;
static Layer* top_cutting_layer;
static GPath* top_cutting_path = NULL;
static GPathInfo top_cutting_path_info = {
  .num_points = 4,
  .points = (GPoint[]) {{0, 0}, {0, 0}, {0, 0}, {0, 0}}
};
static Layer* top_copy_layer;
char top_text[3] = "23";

static Layer* bottom_layer;
static Layer* bottom_cutting_layer;
static GPath* bottom_cutting_path = NULL;
static GPathInfo bottom_cutting_path_info = {
  .num_points = 4,
  .points = (GPoint[]) {{0, 0}, {0, 0}, {0, 0}, {0, 0}}
};
static Layer* bottom_copy_layer;
char bottom_text[3] = "59";

static Layer* cut_layer;

static void draw_text(GContext* ctx, char* text, uint8_t font, int16_t x, int16_t y, GColor color, GTextAlignment text_alignment, FTextAnchor text_anchor) {
  fctx_init_context(&fctx, ctx);
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, font_peace_sans[font], 105);
  fctx_set_fill_color(&fctx, color);
  fctx_set_pivot(&fctx, FPointZero);
  fctx_set_offset(&fctx, FPointI(x, y));
  fctx_set_rotation(&fctx, 0);
  fctx_draw_string(&fctx, text, font_peace_sans[font], text_alignment, text_anchor);
  fctx_end_fill(&fctx);
  fctx_deinit_context(&fctx);
}

static void top_layer_update_proc(Layer* layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(layer);

  // draw the background
  graphics_context_set_fill_color(ctx, settings.color_background_top);
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, copy_window_start+copy_window_height), 0, 0);

  // draw the text
  if(settings.font_top && !gcolor_equal(settings.color_text_fill_top, settings.color_background_top)) {
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND  
    draw_text(ctx, top_text, 0, bounds.size.w-32, bounds.size.h/2+38, settings.color_text_fill_top, GTextAlignmentRight, FTextAnchorBottom);
#else
    draw_text(ctx, top_text, 0, bounds.size.w-8, bounds.size.h/2+38, settings.color_text_fill_top, GTextAlignmentRight, FTextAnchorBottom);
#endif
#else
#ifdef PBL_ROUND  
    draw_text(ctx, top_text, 0, 12, bounds.size.h/2+38, settings.color_text_fill_top, GTextAlignmentLeft, FTextAnchorBottom);
#else
    draw_text(ctx, top_text, 0, 0, bounds.size.h/2+38, settings.color_text_fill_top, GTextAlignmentLeft, FTextAnchorBottom);
#endif
#endif
  }
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND  
  draw_text(ctx, top_text, settings.font_top, bounds.size.w-32, bounds.size.h/2+38, settings.color_text_top, GTextAlignmentRight, FTextAnchorBottom);
#else
  draw_text(ctx, top_text, settings.font_top, bounds.size.w-8, bounds.size.h/2+38, settings.color_text_top, GTextAlignmentRight, FTextAnchorBottom);
#endif
#else
#ifdef PBL_ROUND  
  draw_text(ctx, top_text, settings.font_top, 12, bounds.size.h/2+38, settings.color_text_top, GTextAlignmentLeft, FTextAnchorBottom);
#else
  draw_text(ctx, top_text, settings.font_top, 0, bounds.size.h/2+38, settings.color_text_top, GTextAlignmentLeft, FTextAnchorBottom);
#endif
#endif
}

static void top_cutting_layer_update_proc(Layer* layer, GContext* ctx) {
  // cut off the bottom by painting over it in the background color
  graphics_context_set_fill_color(ctx, settings.color_background_bottom);
  graphics_context_set_antialiased(ctx, false);
  gpath_draw_filled(ctx, top_cutting_path);
}

static void top_copy_layer_update_proc(Layer* layer, GContext* ctx) {
  GBitmap* buffer = graphics_capture_frame_buffer(ctx);
  GSize buffer_size = gbitmap_get_bounds(buffer).size;
  // create backbuffer bitmap only once
  if(!copy_bitmap) {
    copy_bitmap = gbitmap_create_blank(GSize(buffer_size.w, copy_window_height), PBL_IF_COLOR_ELSE(GBitmapFormat8Bit, GBitmapFormat1Bit));
    bitmap_layer_set_bitmap(bitmap_layer, copy_bitmap);
  }
  // copy the center window from the framebuffer into the backbuffer bitmap
  for(int i = copy_window_start; i < copy_window_start+copy_window_height; ++i) {
#ifdef PBL_COLOR
    GBitmapDataRowInfo from_row_info = gbitmap_get_data_row_info(buffer, i);
    GBitmapDataRowInfo to_row_info = gbitmap_get_data_row_info(copy_bitmap, i-copy_window_start);
    for(int j = from_row_info.min_x; j < from_row_info.max_x; ++j) {
      to_row_info.data[j] = from_row_info.data[j];
    }
#else
    uint16_t bytes_per_row = gbitmap_get_bytes_per_row(buffer);
    memcpy(gbitmap_get_data(copy_bitmap), &(gbitmap_get_data(buffer)[copy_window_start*bytes_per_row]), copy_window_height*bytes_per_row);
#endif
  }
  graphics_release_frame_buffer(ctx, buffer);
}

static void bottom_layer_update_proc(Layer* layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(layer);

  // draw the background
  graphics_context_set_fill_color(ctx, settings.color_background_bottom);
  graphics_fill_rect(ctx, GRect(0, copy_window_start, bounds.size.w, bounds.size.h-copy_window_start), 0, 0);

  // draw the text
  if(settings.font_bottom && !gcolor_equal(settings.color_text_fill_bottom, settings.color_background_bottom)) {
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND  
    draw_text(ctx, bottom_text, 0, bounds.size.w-147, bounds.size.h/2-12, settings.color_text_fill_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#else
    draw_text(ctx, bottom_text, 0, 10, bounds.size.h/2-12, settings.color_text_fill_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#endif
#else
#ifdef PBL_ROUND  
    draw_text(ctx, bottom_text, 0, bounds.size.w-12,bounds.size.h/2-12, settings.color_text_fill_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#else
    draw_text(ctx, bottom_text, 0, bounds.size.w,bounds.size.h/2-12, settings.color_text_fill_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#endif
#endif
  }
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND  
  draw_text(ctx, bottom_text, settings.font_bottom, bounds.size.w-147, bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#else
  draw_text(ctx, bottom_text, settings.font_bottom,  10, bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#endif
#else
#ifdef PBL_ROUND  
  draw_text(ctx, bottom_text, settings.font_bottom, bounds.size.w-12,bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#else
  draw_text(ctx, bottom_text, settings.font_bottom, bounds.size.w,bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#endif
#endif
}

static void bottom_cutting_layer_update_proc(Layer* layer, GContext* ctx) {
  // cut off the top by painting over it in the background color
  graphics_context_set_fill_color(ctx, settings.color_background_top);
  graphics_context_set_antialiased(ctx, false);
  gpath_draw_filled(ctx, bottom_cutting_path);
}

static void bottom_copy_layer_update_proc(Layer* layer, GContext* ctx) {
  GBitmap* buffer = graphics_capture_frame_buffer(ctx);
#ifdef PBL_BW
  GSize buffer_size = gbitmap_get_bounds(buffer).size;
#endif
  
  // copy from the center window in the framebuffer into the backbuffer bitmap
  for(int i = copy_window_start; i < copy_window_start+copy_window_height; ++i) {
#ifdef PBL_COLOR
    GBitmapDataRowInfo from_row_info = gbitmap_get_data_row_info(buffer, i);
    GBitmapDataRowInfo to_row_info = gbitmap_get_data_row_info(copy_bitmap, i-copy_window_start);
    for(int j = from_row_info.min_x; j < from_row_info.max_x; ++j) {
      // only copy foreground pixels to leave the pixels from the top layer
      if(   settings.color_background_top.argb != from_row_info.data[j]
         && settings.color_background_bottom.argb == to_row_info.data[j]) {
        to_row_info.data[j] = from_row_info.data[j];
      }
    }
#else
    uint16_t bytes_per_row = gbitmap_get_bytes_per_row(buffer);
    uint8_t* bytes = gbitmap_get_data(buffer);
    uint16_t copy_bytes_per_row = gbitmap_get_bytes_per_row(copy_bitmap);
    uint8_t* copy_bytes = gbitmap_get_data(copy_bitmap);
    for(int j = 0; j < buffer_size.w; ++j) {
      // only copy foreground pixels to leave the pixels from the top layer
      if(   bw_bitmap_data_get_value(bytes, bytes_per_row, j, i)
        && !bw_bitmap_data_get_value(copy_bytes, copy_bytes_per_row, j, i-copy_window_start)) {
          bw_bitmap_data_set_pixel(copy_bytes, copy_bytes_per_row, j, i-copy_window_start);
      }
    }
#endif
  }
  graphics_release_frame_buffer(ctx, buffer);
  layer_mark_dirty(bitmap_layer_get_layer(bitmap_layer));
}

static void cut_layer_update_proc(Layer* layer, GContext* ctx) {
  // draw the cuting line
  fctx_init_context(&fctx, ctx);
  fctx_begin_fill(&fctx);
  fctx_set_fill_color(&fctx, settings.color_the_cut);
  fctx_move_to(&fctx, FPointI(cut[0].x, cut[0].y-2));
  fctx_line_to(&fctx, FPointI(cut[1].x, cut[1].y-2));
  fctx_line_to(&fctx, FPointI(cut[1].x, cut[1].y+2));
  fctx_line_to(&fctx, FPointI(cut[0].x, cut[0].y+2));
  fctx_line_to(&fctx, FPointI(cut[0].x, cut[0].y-2));
  fctx_end_fill(&fctx);
  fctx_deinit_context(&fctx);
  
#if 0 // center lines to help with adjustment
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_context_set_stroke_color(ctx, GColorYellow);
  graphics_draw_line(ctx, GPoint(0, bounds.size.h/2), GPoint(bounds.size.w, bounds.size.h/2));
  graphics_draw_line(ctx, GPoint(bounds.size.w/2, 0), GPoint(bounds.size.w/2, bounds.size.h));
#endif
}

static void my_window_load(Window *window) {
  // settings
  settings.color_background_top = GColorBlack;
  settings.color_background_bottom = GColorBlack;
  settings.color_text_top = GColorWhite;
  settings.color_text_bottom = GColorPictonBlue;
  settings.color_text_fill_top = GColorBlack;
  settings.color_text_fill_bottom = GColorBlack;
  settings.color_the_cut = GColorWhite;
  settings.font_top = 1;
  settings.font_bottom = 0;
  
  Layer* root_layer = window_get_root_layer(window);
  GRect root_bounds = layer_get_bounds(root_layer);

  font_peace_sans[0] = ffont_create_from_resource(RESOURCE_ID_FONT_PEACE_SANS);
  font_peace_sans[1] = ffont_create_from_resource(RESOURCE_ID_FONT_PEACE_SANS_OUTLINE);
  
  // the endpoints of the cutting line
  cut[0] = GPoint(0, root_bounds.size.h/2+10);
  cut[1] = GPoint(root_bounds.size.w, root_bounds.size.h/2-10);

  // this is fine tuned to save RAM, it only needs to cover the overlapping area
  copy_window_start = cut[1].y-10;
  copy_window_height = cut[0].y-cut[1].y+10;

  top_layer = layer_create(root_bounds);
  layer_set_update_proc(top_layer, top_layer_update_proc);
  layer_add_child(root_layer, top_layer);

  top_cutting_path_info.points[0] = cut[0];
  top_cutting_path_info.points[1] = cut[1];
  top_cutting_path_info.points[2] = GPoint(root_bounds.size.w, copy_window_start+copy_window_height);
  top_cutting_path_info.points[3] = GPoint(0, copy_window_start+copy_window_height);
  top_cutting_path = gpath_create(&top_cutting_path_info);
  
  top_cutting_layer = layer_create(root_bounds);
  layer_set_update_proc(top_cutting_layer, top_cutting_layer_update_proc);
  layer_add_child(root_layer, top_cutting_layer);
  
  top_copy_layer = layer_create(root_bounds);
  layer_set_update_proc(top_copy_layer, top_copy_layer_update_proc);
  layer_add_child(root_layer, top_copy_layer);

  bottom_layer = layer_create(root_bounds);
  layer_set_update_proc(bottom_layer, bottom_layer_update_proc);
  layer_add_child(root_layer, bottom_layer);

  bottom_cutting_path_info.points[0] = cut[0];
  bottom_cutting_path_info.points[1] = cut[1];
  bottom_cutting_path_info.points[2] = GPoint(root_bounds.size.w, copy_window_start);
  bottom_cutting_path_info.points[3] = GPoint(0, copy_window_start);
  bottom_cutting_path = gpath_create(&bottom_cutting_path_info);
  
  bottom_cutting_layer = layer_create(root_bounds);
  layer_set_update_proc(bottom_cutting_layer, bottom_cutting_layer_update_proc);
  layer_add_child(root_layer, bottom_cutting_layer);
  
  bottom_copy_layer = layer_create(root_bounds);
  layer_set_update_proc(bottom_copy_layer, bottom_copy_layer_update_proc);
  layer_add_child(root_layer, bottom_copy_layer);

  bitmap_layer = bitmap_layer_create(GRect(0, copy_window_start, root_bounds.size.w, copy_window_height));
  bitmap_layer_set_background_color(bitmap_layer, GColorClear);
#ifdef PBL_COLOR  
  bitmap_layer_set_compositing_mode(bitmap_layer, GCompOpSet);
#endif
  layer_add_child(root_layer, bitmap_layer_get_layer(bitmap_layer));

  cut_layer = layer_create(root_bounds);
  layer_set_update_proc(cut_layer, cut_layer_update_proc);
  layer_add_child(root_layer, cut_layer);
  
  start();
}

void cutting_edge_update() {
  layer_mark_dirty(top_layer);
  layer_mark_dirty(bottom_layer);
}

void cutting_edge_init() {
  my_window = window_create();
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = my_window_load,
    //.unload = TODO
  });
  window_stack_push(my_window, true);
}

void cutting_edge_deinit() {
  window_destroy(my_window);
}
