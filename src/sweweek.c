#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0xDA, 0xEC, 0xA1, 0xDB, 0xBD, 0xE5, 0xE1, 0xFC, 0xA7, 0xC5, 0x33, 0xD2, 0xAD, 0x7E, 0x47, 0x77 }
PBL_APP_INFO(MY_UUID, "SweWeek", "ripped from SimpleWeek", 1, 0 /* App version */, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

TextLayer text_day_layer;
TextLayer text_week_layer;
TextLayer text_date_layer;
TextLayer text_time_layer;
TextLayer text_year_layer;
TextLayer text_daynum_layer;

Layer line_layer;

static const char *day_names[] = {
  "söndag", "måndag", "tisdag", "onsdag", "torsdag", "fredag", "lördag"
};

static const char *month_names[] = {
  "januari", "februari", "mars", "april", "maj", "juni", 
  "juli", "augusti", "september", "oktober", "november", "december"
};

void line_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_stroke_color(ctx, GColorWhite);

  graphics_draw_line(ctx, GPoint(8, 51), GPoint(131, 51));
  graphics_draw_line(ctx, GPoint(8, 52), GPoint(131, 52));
  graphics_draw_line(ctx, GPoint(8, 115), GPoint(131, 115));
  graphics_draw_line(ctx, GPoint(8, 116), GPoint(131, 116));

}

void update_display(PblTm *tick_time) {
  // Need to be static because they're used by the system later.
  static char time_text[]     = "00:00";
  static char day_text[]      = "                  ";
  static char week_text[]     = "v.00";
  static char date_text[]     = "                  ";
  static char new_date_text[] = "                  ";	
  static char year_text[]     = "                  ";
  static char daynum_text[]   = "                  ";

  int month = tick_time->tm_mon;
  string_format_time(new_date_text, sizeof(date_text), "%e ", tick_time);
  strcat(new_date_text, month_names[month]);

  // Only update the date/day/week strings when they're changed.
  if (strncmp(new_date_text, date_text, sizeof(date_text)) != 0) {
	  string_format_time(week_text, sizeof(week_text), "v.%V", tick_time);
	  text_layer_set_text(&text_week_layer, week_text);

	  int dayofweek = tick_time->tm_wday;
	  strcpy(day_text, day_names[dayofweek]);
	  text_layer_set_text(&text_day_layer, day_text);

	  strncpy(date_text, new_date_text, sizeof(date_text));
	  text_layer_set_text(&text_date_layer, date_text);
	  
	  string_format_time(year_text, sizeof(year_text), "%Y-%m-%d", tick_time);
	  text_layer_set_text(&text_year_layer, year_text);
	  
	  string_format_time(daynum_text, sizeof(daynum_text), "%j", tick_time);
	  text_layer_set_text(&text_daynum_layer, daynum_text);
  }

  char *time_format;
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

  window_init(&window, "SimpleWeek");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  // week
  text_layer_init(&text_week_layer, window.layer.frame);
  text_layer_set_text_color(&text_week_layer, GColorWhite);
  text_layer_set_background_color(&text_week_layer, GColorClear);
  layer_set_frame(&text_week_layer.layer, GRect(96, 8, 144-96, 168-8));
  text_layer_set_font(&text_week_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_REGULAR_18)));
  layer_add_child(&window.layer, &text_week_layer.layer);
	
  // daynumber
  text_layer_init(&text_daynum_layer, window.layer.frame);
  text_layer_set_text_color(&text_daynum_layer, GColorWhite);
  text_layer_set_background_color(&text_daynum_layer, GColorClear);
  layer_set_frame(&text_daynum_layer.layer, GRect(100, 24, 144-100, 168-24));
  text_layer_set_font(&text_daynum_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_REGULAR_18)));
  layer_add_child(&window.layer, &text_daynum_layer.layer);

  // weekday
  text_layer_init(&text_day_layer, window.layer.frame);
  text_layer_set_text_color(&text_day_layer, GColorWhite);
  text_layer_set_background_color(&text_day_layer, GColorClear);
  layer_set_frame(&text_day_layer.layer, GRect(8, 8, 144-8, 168-8));
  text_layer_set_font(&text_day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_REGULAR_18)));
  layer_add_child(&window.layer, &text_day_layer.layer);

  // day/month
  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(8, 24, 144-8, 168-24));
  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_REGULAR_18)));
  layer_add_child(&window.layer, &text_date_layer.layer);

  // time
  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(6, 53, 144-6, 168-53));
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CONTINUUM_BOLD_48)));
//  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_BOLD_47)));	
  layer_add_child(&window.layer, &text_time_layer.layer);
	
  // year
  text_layer_init(&text_year_layer, window.layer.frame);
  text_layer_set_text_color(&text_year_layer, GColorWhite);
  text_layer_set_background_color(&text_year_layer, GColorClear);
  layer_set_frame(&text_year_layer.layer, GRect(22, 140, 144-22, 168-140));
  text_layer_set_font(&text_year_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_REGULAR_18)));
  layer_add_child(&window.layer, &text_year_layer.layer);


  // line
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