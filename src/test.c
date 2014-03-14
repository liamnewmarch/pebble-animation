/*
 * Copyright (c) 2014 Liam Newmarch. All rights reserved.
 */ 

#include <pebble.h>
#include <stdlib.h>

#define COUNT_POINTS 2


// GLOBAL VARs

// Layers
static Window *window;
static Layer *layer;

// Tick timer
static AppTimer *timer;

// Ints
static int x = 0;
static int y = 0;
static int delay = 1000 / 30;
static int size = 20;

// Objects
static GPoint *points[COUNT_POINTS];

// On every timer tick

static void timer_tick_handle(void *data) {
  // Mark the layer as dirty (which triggers my_layer_draw)
  layer_mark_dirty(layer);
  // Set up the next timeout
  timer = app_timer_register(delay, (AppTimerCallback) timer_tick_handle, NULL);
}


// Draw a new frame

static void my_layer_draw(Layer *layer, GContext *ctx) {
  // Get bounds of the layer we’re dealing with
  GRect bounds = layer_get_bounds(layer);

  for (int i = 0; i < COUNT_POINTS; i++) {
    if (y == 0) {
      x = rand() % bounds.size.w;
    }
    y = (y + 2) % (bounds.size.h + size);
  }

  // Get midpoint
  const int half_w = bounds.size.w / 2;
  const int half_h = bounds.size.h / 2;

  // Draw a black filled rectangle with sharp corners
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Draw a white filled circle a radius of half the layer height
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(x, y - size / 2), size / 2);
}


// On window load

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_add_child(window_layer, layer);
  layer_set_update_proc(layer, my_layer_draw);
  // points[0] = GPoint(0, 0);
  timer = app_timer_register(delay, (AppTimerCallback) timer_tick_handle, NULL);
}


// On window unload

static void window_unload(Window *window) {
  app_timer_cancel(timer);
  layer_destroy(layer);
}


// On watchface create

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  const bool animated = true;
  window_stack_push(window, animated);
}


// On watchface destroy

static void deinit(void) {
  window_destroy(window);
}


// Bootstrap

int main(void) {
  init();
  app_event_loop();
  deinit();
}
