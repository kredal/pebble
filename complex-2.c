#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0xAA, 0x3C, 0x31, 0x4B, 0x8D, 0xA5, 0x41, 0x3C, 0xA7, 0xC5, 0x33, 0xD2, 0xAD, 0x7E, 0x46, 0x76 }
PBL_APP_INFO(MY_UUID, "Complex-2", "Josh Ward", 1, 0 /* App version */, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

TextLayer text_day_layer; //day_layer
TextLayer text_date_layer;
TextLayer text_time_layer;

Layer line_layer;


void line_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_stroke_color(ctx, GColorWhite);

  graphics_draw_line(ctx, GPoint(8, 97), GPoint(131, 97));
  graphics_draw_line(ctx, GPoint(8, 98), GPoint(131, 98));

}

void update_display(PblTm *tick_time) {

  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char day_text[] = "Xxxxxxxxxx 00000";
  static char date_text[] = "Xxxxxxxxx 00";
  static char new_date_text[] = "Xxxxxxxxx 00";

  char *time_format;


  // Only update the date and day strings when they're changed.
  string_format_time(new_date_text, sizeof(date_text), "%B %e", tick_time);
  if (strncmp(new_date_text, date_text, sizeof(date_text)) != 0) {
    strncpy(date_text, new_date_text, sizeof(date_text));
	  text_layer_set_text(&text_date_layer, date_text);
	  string_format_time(day_text, sizeof(day_text), "%a %y%j", tick_time);
	  text_layer_set_text(&text_day_layer, day_text);
  }


  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(&text_time_layer, time_text);

}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Complexity");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);


  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(8, 44, 144-8, 168-44));
  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_date_layer.layer);


  text_layer_init(&text_day_layer, window.layer.frame);
  text_layer_set_text_color(&text_day_layer, GColorWhite);
  text_layer_set_background_color(&text_day_layer, GColorClear);
  layer_set_frame(&text_day_layer.layer, GRect(8, 68, 144-8, 168-68));
  text_layer_set_font(&text_day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_day_layer.layer);


  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(7, 92, 144-7, 168-92));
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(&window.layer, &text_time_layer.layer);


  layer_init(&line_layer, window.layer.frame);
  line_layer.update_proc = &line_layer_update_callback;
  layer_add_child(&window.layer, &line_layer);

  // Avoid blank display on launch
  PblTm tick_time;
  get_time(&tick_time);
  update_display(&tick_time);
  
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)ctx;
  update_display(t->tick_time);
  
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
