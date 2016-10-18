#include <pebble.h>
#include <pebble-fctx/ffont.h>
#include <pebble-fctx/fctx.h>
#include "cutup.h"

extern void start();

#define bw_bitmap_data_get_value(BMP, BPR, X, Y) (((*((BMP)+(Y)*(BPR)+(X)/8)) & (1 << (X)%8)) ? 1 : 0)
#define bw_bitmap_data_set_pixel(BMP, BPR, X, Y) (*((BMP)+(Y)*(BPR)+(X)/8)) |= (1 << (X)%8)
#define bw_bitmap_data_clear_pixel(BMP, BPR, X, Y) (*((BMP)+(Y)*(BPR)+(X)/8)) &= ~(1 << (X)%8)

//#define ALTERNATE_ALIGNMENT

enum {
  FONT,
  OUTLINE_FONT
};

Settings settings;
static Window* my_window;
static FFont* font_peace_sans[2];
static FContext fctx;
GPoint cut[2];
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

static Layer* top_layer;
static Layer* top_copy_layer;
char top_text[3] = "23";

static Layer* bottom_layer;
static Layer* bottom_copy_layer;
char bottom_text[3] = "59";

static Layer* cut_layer;

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
      GBitmapDataRowInfo row_info = gbitmap_get_data_row_info(buffer, i);
      for(int j = row_info.min_x; j < row_info.max_x; ++j) {
        if(row_info.data[j] == GColorWhite.argb) {
          bw_bitmap_data_set_pixel(gbitmap_get_data(mask_bitmap), mask_bytes_per_row, j, i-copy_window_start);
        }
      }
    }
#else
    uint16_t bytes_per_row = gbitmap_get_bytes_per_row(buffer);
    memcpy(gbitmap_get_data(mask_bitmap), &(gbitmap_get_data(buffer)[copy_window_start*bytes_per_row]), copy_window_height*bytes_per_row);
#endif
    graphics_release_frame_buffer(ctx, buffer);
  }
}

static void draw_text_with_fctx(GContext* ctx, char* text, uint8_t font, int16_t x, int16_t y, GColor color, GTextAlignment text_alignment, FTextAnchor text_anchor) {
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, font_peace_sans[font], 105);
  fctx_set_fill_color(&fctx, color);
  fctx_set_pivot(&fctx, FPointZero);
  fctx_set_offset(&fctx, FPointI(x, y));
  fctx_set_rotation(&fctx, 0);
  fctx_draw_string(&fctx, text, font_peace_sans[font], text_alignment, text_anchor);
  fctx_end_fill(&fctx);
}

static void cutting_layer_update_proc(Layer* layer, GContext* ctx) {
  // cut off the bottom by painting over it in the background color
  graphics_context_set_fill_color(ctx, settings.color_background_bottom);
  graphics_context_set_antialiased(ctx, false);
  gpath_draw_filled(ctx, cutting_path);
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
    fctx_begin_fill(&fctx);
    fctx_set_fill_color(&fctx, settings.color_the_cut_outline_top);
    fctx_move_to(&fctx, FPointI(cut[0].x, cut[0].y-5));
    fctx_line_to(&fctx, FPointI(cut[1].x, cut[1].y-5));
    fctx_line_to(&fctx, FPointI(cut[1].x, cut[1].y-2));
    fctx_line_to(&fctx, FPointI(cut[0].x, cut[0].y-2));
    fctx_line_to(&fctx, FPointI(cut[0].x, cut[0].y-5));
    fctx_end_fill(&fctx);
  }
  
  // draw the text
  if(!gcolor_equal(settings.color_text_top, settings.color_background_top) ||
     !gcolor_equal(settings.color_the_cut_outline_top, settings.color_background_top)) {
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND  
    draw_text_with_fctx(ctx, top_text, FONT, bounds.size.w-32, bounds.size.h/2+38, settings.color_text_top, GTextAlignmentRight, FTextAnchorBottom);
#else
    draw_text_with_fctx(ctx, top_text, FONT, bounds.size.w-8, bounds.size.h/2+38, settings.color_text_top, GTextAlignmentRight, FTextAnchorBottom);
#endif
#else
#ifdef PBL_ROUND  
    draw_text_with_fctx(ctx, top_text, FONT, 12, bounds.size.h/2+38, settings.color_text_top, GTextAlignmentLeft, FTextAnchorBottom);
#else
    draw_text_with_fctx(ctx, top_text, FONT, 0, bounds.size.h/2+38, settings.color_text_top, GTextAlignmentLeft, FTextAnchorBottom);
#endif
#endif
  }
  if(!gcolor_equal(settings.color_text_outline_top, settings.color_text_top)) {
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND  
    draw_text_with_fctx(ctx, top_text, OUTLINE_FONT, bounds.size.w-32, bounds.size.h/2+38, settings.color_text_outline_top, GTextAlignmentRight, FTextAnchorBottom);
#else
    draw_text_with_fctx(ctx, top_text, OUTLINE_FONT, bounds.size.w-8, bounds.size.h/2+38, settings.color_text_outline_top, GTextAlignmentRight, FTextAnchorBottom);
#endif
#else
#ifdef PBL_ROUND  
    draw_text_with_fctx(ctx, top_text, OUTLINE_FONT, 12, bounds.size.h/2+38, settings.color_text_outline_top, GTextAlignmentLeft, FTextAnchorBottom);
#else
    draw_text_with_fctx(ctx, top_text, OUTLINE_FONT, 0, bounds.size.h/2+38, settings.color_text_outline_top, GTextAlignmentLeft, FTextAnchorBottom);
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
    GBitmapDataRowInfo from_row_info = gbitmap_get_data_row_info(buffer, i);
    GBitmapDataRowInfo to_row_info = gbitmap_get_data_row_info(copy_bitmap, i-copy_window_start);
    for(int j = from_row_info.min_x; j < from_row_info.max_x; ++j) {
      to_row_info.data[j] = from_row_info.data[j];
    }
  }
#else
  uint16_t bytes_per_row = gbitmap_get_bytes_per_row(buffer);
  memcpy(gbitmap_get_data(copy_bitmap), &(gbitmap_get_data(buffer)[copy_window_start*bytes_per_row]), copy_window_height*bytes_per_row);
#endif
  graphics_release_frame_buffer(ctx, buffer);
}

static void bottom_layer_update_proc(Layer* layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(layer);

  // draw the background
  graphics_context_set_fill_color(ctx, settings.color_background_bottom);
  graphics_fill_rect(ctx, GRect(0, copy_window_start, bounds.size.w, bounds.size.h-copy_window_start), 0, 0);

  // init fctx once
  fctx_init_context(&fctx, ctx);

  // draw the cuting line border
  if(!gcolor_equal(settings.color_the_cut_outline_bottom, settings.color_background_bottom)) {
    fctx_begin_fill(&fctx);
    fctx_set_fill_color(&fctx, settings.color_the_cut_outline_bottom);
    fctx_move_to(&fctx, FPointI(cut[0].x, cut[0].y+2));
    fctx_line_to(&fctx, FPointI(cut[1].x, cut[1].y+2));
    fctx_line_to(&fctx, FPointI(cut[1].x, cut[1].y+5));
    fctx_line_to(&fctx, FPointI(cut[0].x, cut[0].y+5));
    fctx_line_to(&fctx, FPointI(cut[0].x, cut[0].y+2));
    fctx_end_fill(&fctx);
  }
  
  // draw the text
  if(!gcolor_equal(settings.color_text_bottom, settings.color_background_bottom) ||
     !gcolor_equal(settings.color_the_cut_outline_bottom, settings.color_background_bottom)) {
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND  
    draw_text_with_fctx(ctx, bottom_text, FONT, bounds.size.w-147, bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#else
    draw_text_with_fctx(ctx, bottom_text, FONT, 10, bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#endif
#else
#ifdef PBL_ROUND  
    draw_text_with_fctx(ctx, bottom_text, FONT, bounds.size.w-12,bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#else
    draw_text_with_fctx(ctx, bottom_text, FONT, bounds.size.w,bounds.size.h/2-12, settings.color_text_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#endif
#endif
  }
  if(!gcolor_equal(settings.color_text_outline_bottom, settings.color_text_bottom)) {
#ifdef ALTERNATE_ALIGNMENT
#ifdef PBL_ROUND  
    draw_text_with_fctx(ctx, bottom_text, OUTLINE_FONT, bounds.size.w-147, bounds.size.h/2-12, settings.color_text_outline_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#else
    draw_text_with_fctx(ctx, bottom_text, OUTLINE_FONT,  10, bounds.size.h/2-12, settings.color_text_outline_bottom, GTextAlignmentLeft, FTextAnchorCapTop);
#endif
#else
#ifdef PBL_ROUND  
    draw_text_with_fctx(ctx, bottom_text, OUTLINE_FONT, bounds.size.w-12,bounds.size.h/2-12, settings.color_text_outline_bottom, GTextAlignmentRight, FTextAnchorCapTop);
#else
    draw_text_with_fctx(ctx, bottom_text, OUTLINE_FONT, bounds.size.w,bounds.size.h/2-12, settings.color_text_outline_bottom, GTextAlignmentRight, FTextAnchorCapTop);
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
    GBitmapDataRowInfo to_row_info = gbitmap_get_data_row_info(buffer, i);
    for(int j = from_row_info.min_x; j < from_row_info.max_x; ++j) {
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
          bw_bitmap_data_set_pixel(bytes, bytes_per_row, j, i);
        } else {
          bw_bitmap_data_clear_pixel(bytes, bytes_per_row, j, i);
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
  Layer* root_layer = window_get_root_layer(window);
  GRect root_bounds = layer_get_bounds(root_layer);

  // filled font and outline font
  font_peace_sans[FONT] = ffont_create_from_resource(RESOURCE_ID_FONT_PEACE_SANS);
  font_peace_sans[OUTLINE_FONT] = ffont_create_from_resource(RESOURCE_ID_FONT_PEACE_SANS_OUTLINE);
  
  // the endpoints of the cutting line
  cut[0] = GPoint(0, root_bounds.size.h/2+10);
  cut[1] = GPoint(root_bounds.size.w, root_bounds.size.h/2-10);

  // this is fine tuned to save RAM, it only needs to cover the overlapping area
  copy_window_start = cut[1].y-10;
  copy_window_height = cut[0].y-cut[1].y+10;
  
  // create the cutting path
  cutting_path_info.points[0] = cut[0];
  cutting_path_info.points[1] = cut[1];
  cutting_path_info.points[2] = GPoint(root_bounds.size.w, copy_window_start+copy_window_height);
  cutting_path_info.points[3] = GPoint(0, copy_window_start+copy_window_height);
  cutting_path = gpath_create(&cutting_path_info);

  // creates a mask to do the cutting
  mask_layer = layer_create(root_bounds);
  layer_set_update_proc(mask_layer, mask_layer_update_proc);
  layer_add_child(root_layer, mask_layer);

  // draws the top text
  top_layer = layer_create(root_bounds);
  layer_set_update_proc(top_layer, top_layer_update_proc);
  layer_add_child(root_layer, top_layer);

  // cuts the bottom of the top text
  cutting_layer = layer_create(root_bounds);
  layer_set_update_proc(cutting_layer, cutting_layer_update_proc);
  layer_add_child(root_layer, cutting_layer);
  
  // copies the cut top text to backbuffer
  top_copy_layer = layer_create(root_bounds);
  layer_set_update_proc(top_copy_layer, top_copy_layer_update_proc);
  layer_add_child(root_layer, top_copy_layer);

  // draws the bottom text
  bottom_layer = layer_create(root_bounds);
  layer_set_update_proc(bottom_layer, bottom_layer_update_proc);
  layer_add_child(root_layer, bottom_layer);

  // copies the cut top text from the backbuffer over the bottom text
  bottom_copy_layer = layer_create(root_bounds);
  layer_set_update_proc(bottom_copy_layer, bottom_copy_layer_update_proc);
  layer_add_child(root_layer, bottom_copy_layer);

  // draws the cut line
  cut_layer = layer_create(root_bounds);
  layer_set_update_proc(cut_layer, cut_layer_update_proc);
  layer_add_child(root_layer, cut_layer);
  
  start();
}

void cutting_edge_update() {
  layer_mark_dirty(window_get_root_layer(my_window));
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
