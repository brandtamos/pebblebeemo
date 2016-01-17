#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static Layer *s_canvas;

static GPath *s_blue_triangle_path = NULL;
static const GPathInfo TRIANGLE_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{135, 132}, {128, 142}, {142, 142}}
};
//no-op function for use in conditional macros
static void nop(){}
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  //redraw graphic layer
  layer_mark_dirty(s_canvas);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
static void draw_bw_outlines(Layer *layer, GContext *ctx){
  graphics_draw_round_rect(ctx, GRect(12, 30, 122, 70), 8);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(20, 140, 10, 30), 2, GCornersAll);
  graphics_fill_rect(ctx, GRect(10, 150, 30, 10), 2, GCornersAll);
}
static void bt_handler(bool connected) {
  layer_mark_dirty(s_canvas);
}
static void draw_round_beemo(Layer *layer, GContext *ctx){
  //FYI, GRects are like this: (x of top left corner, y of top left corner, width, height)
  GRect bounds = layer_get_bounds(layer);
  int16_t width = bounds.size.w;
  int16_t midx = width / 2;
  bool btConnected = connection_service_peek_pebble_app_connection();
  BatteryChargeState charge_state = battery_state_service_peek();
  
  //draw face background
  graphics_context_set_fill_color(ctx, GColorPastelYellow); 
  graphics_fill_rect(ctx, GRect(30, 30, 122, 70), 8, GCornersAll);

  
  //draw eyes
  //if BT is connected, eyes open, otherwise eyes closed
  graphics_context_set_fill_color(ctx, GColorBlack); 
  
  if(btConnected){
    graphics_fill_circle(ctx, GPoint(midx - 50,50), 6);
    graphics_fill_circle(ctx, GPoint(midx + 50,50), 6);
  }
  else{
    graphics_fill_rect(ctx, GRect(midx - 50, 50, 15, 5), 0, GCornersAll);
    graphics_fill_rect(ctx, GRect(midx + 35, 50, 15, 5), 0, GCornersAll);
  }
  
  //draw mouth, smile, straight, or frown depending on battery state
  if(charge_state.charge_percent > 50){
    graphics_fill_radial(ctx, GRect(71, 35, 40, 40), GOvalScaleModeFitCircle, 5, DEG_TO_TRIGANGLE(90), DEG_TO_TRIGANGLE(270));
  }
  else if(charge_state.charge_percent > 25){
    graphics_fill_rect(ctx, GRect(71, 65, 40, 6), 0, GCornersAll);
  }
  else{
    graphics_fill_radial(ctx, GRect(71, 55, 40, 40), GOvalScaleModeFitCircle, 5, DEG_TO_TRIGANGLE(270), DEG_TO_TRIGANGLE(450));
  }
  
  //draw slot
  graphics_context_set_fill_color(ctx, GColorDarkGreen);
  graphics_fill_rect(ctx, GRect(32, 105, 60, 8), 2, GCornersAll);
  
  //draw dark blue circle on right side
  graphics_context_set_fill_color(ctx, GColorOxfordBlue); 
  graphics_fill_circle(ctx, GPoint(140,108), 5);
  
  //draw yellow d-pad
  graphics_context_set_fill_color(ctx, GColorYellow);
  graphics_fill_rect(ctx, GRect(40, 140, 10, 30), 2, GCornersAll);
  graphics_fill_rect(ctx, GRect(30, 150, 30, 10), 2, GCornersAll);
  
  //draw red circle
  graphics_context_set_fill_color(ctx, GColorRed); 
  graphics_fill_circle(ctx, GPoint(138,157), 8);
  
  //draw green circle
  graphics_context_set_fill_color(ctx, GColorIslamicGreen); 
  graphics_fill_circle(ctx, GPoint(148,143), 5);
  
  //draw blue triangle
  graphics_context_set_fill_color(ctx, GColorBlue); 
  s_blue_triangle_path = gpath_create(&TRIANGLE_PATH_INFO);
  gpath_draw_filled(ctx, s_blue_triangle_path);
}
static void draw_square_beemo(Layer *layer, GContext *ctx){
  //FYI, GRects are like this: (x of top left corner, y of top left corner, width, height)
  GRect bounds = layer_get_bounds(layer);
  int16_t width = bounds.size.w;
  int16_t midx = width / 2;
  BatteryChargeState charge_state = battery_state_service_peek();
  bool btConnected = connection_service_peek_pebble_app_connection();
  
  //draw face background
  graphics_context_set_fill_color(ctx, GColorPastelYellow); 
  graphics_fill_rect(ctx, GRect(12, 30, 122, 70), 8, GCornersAll);
  
  //draw eyes and mouth
  graphics_context_set_fill_color(ctx, GColorBlack); 
  
  //draw eyes
  //if BT is connected, eyes open, otherwise eyes closed
  if(btConnected){
    graphics_fill_circle(ctx, GPoint(midx - 45,50), 7);
    graphics_fill_circle(ctx, GPoint(midx + 45,50), 7);
  }
  else{
    graphics_fill_rect(ctx, GRect(midx - 45, 50, 15, 5), 0, GCornersAll);
    graphics_fill_rect(ctx, GRect(midx + 33, 50, 15, 5), 0, GCornersAll);
  }
  
  //draw mouth, smile, straight, or frown depending on battery state
  if(charge_state.charge_percent > 50){
    graphics_fill_radial(ctx, GRect(54, 35, 40, 40), GOvalScaleModeFitCircle, 5, DEG_TO_TRIGANGLE(90), DEG_TO_TRIGANGLE(270));
  }
  else if(charge_state.charge_percent > 25){
    graphics_fill_rect(ctx, GRect(54, 65, 40, 6), 0, GCornersAll);
  }
  else{
    graphics_fill_radial(ctx, GRect(54, 55, 40, 40), GOvalScaleModeFitCircle, 5, DEG_TO_TRIGANGLE(270), DEG_TO_TRIGANGLE(450));
  }
  
  //draw slot
  graphics_context_set_fill_color(ctx, GColorDarkGreen);
  graphics_fill_rect(ctx, GRect(14, 105, 60, 8), 2, GCornersAll);
  
  //draw dark blue circle on right side
  graphics_context_set_fill_color(ctx, GColorOxfordBlue); 
  graphics_fill_circle(ctx, GPoint(120,108), 5);
  
  //draw yellow d-pad
  graphics_context_set_fill_color(ctx, GColorYellow);
  graphics_fill_rect(ctx, GRect(15, 136, 10, 30), 2, GCornersAll);
  graphics_fill_rect(ctx, GRect(5, 146, 30, 10), 2, GCornersAll);
  
  //draw red circle
  graphics_context_set_fill_color(ctx, GColorRed); 
  graphics_fill_circle(ctx, GPoint(125,157), 8);
  
  //draw green circle
  graphics_context_set_fill_color(ctx, GColorIslamicGreen); 
  graphics_fill_circle(ctx, GPoint(136,143), 5);
  
  //draw blue triangle
  graphics_context_set_fill_color(ctx, GColorBlue); 
  s_blue_triangle_path = gpath_create(&TRIANGLE_PATH_INFO);
  gpath_move_to(s_blue_triangle_path, GPoint(-15, 0));
  gpath_draw_filled(ctx, s_blue_triangle_path);
  
  //draw needed outlines if display is b/w
  PBL_IF_BW_ELSE(draw_bw_outlines(layer, ctx), nop());
}
static void layer_update_proc(Layer *layer, GContext *ctx) {
  PBL_IF_ROUND_ELSE(draw_round_beemo(layer, ctx), draw_square_beemo(layer, ctx));
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //create layer for drawing
  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, layer_update_proc);
  layer_add_child(window_layer, s_canvas);
  
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, 110, bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  //text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  layer_destroy(s_canvas);

}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_set_background_color(s_main_window, GColorCyan);
  window_stack_push(s_main_window, true);
  
  bt_handler(connection_service_peek_pebble_app_connection());
  //subscribe to bluetooth events
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bt_handler
  });
  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
  //unsubscribe from bt events
  connection_service_unsubscribe();
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}