#include "pebble.h"

Window *window;

TextLayer *text_day_layer;
TextLayer *text_week_layer;
TextLayer *text_date_layer;
TextLayer *text_time_layer;
TextLayer *text_year_layer;
TextLayer *text_daynum_layer;

Layer *line_layer;

static const char *day_names[] = {
  "söndag", "måndag", "tisdag", "onsdag", "torsdag", "fredag", "lördag"
};

static const char *month_names[] = {
  "jan", "febr", "mars", "april", "maj", "juni", 
  "juli", "aug", "sept", "okt", "nov", "dec"
};

void line_layer_update_callback(struct Layer *me, GContext *ctx) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(8, 51), GPoint(131, 51));
  graphics_draw_line(ctx, GPoint(8, 52), GPoint(131, 52));
  graphics_draw_line(ctx, GPoint(8, 115), GPoint(131, 115));
  graphics_draw_line(ctx, GPoint(8, 116), GPoint(131, 116));
}

void update_display(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char time_text[]     = "00:00";
  static char day_text[]      = "                  ";
  static char week_text[]     = "v.00";
  static char date_text[]     = "                  ";
  static char new_date_text[] = "                  ";	
  static char year_text[]     = "                  ";
  static char daynum_text[]   = "                  ";

  int month = tick_time->tm_mon;
  strftime(new_date_text, sizeof(date_text), "%e ", tick_time);
  strcat(new_date_text, month_names[month]);

  // Only update the date/day/week strings when they're changed.
  if (strncmp(new_date_text, date_text, sizeof(date_text)) != 0) {
	  strftime(week_text, sizeof(week_text), "v.%V", tick_time);
	  text_layer_set_text(text_week_layer, week_text);

	  int dayofweek = tick_time->tm_wday;
	  strcpy(day_text, day_names[dayofweek]);
	  text_layer_set_text(text_day_layer, day_text);

	  strncpy(date_text, new_date_text, sizeof(date_text));
	  text_layer_set_text(text_date_layer, date_text);
	  
	  strftime(year_text, sizeof(year_text), "%Y-%m-%d", tick_time);
	  text_layer_set_text(text_year_layer, year_text);
	  
	  strftime(daynum_text, sizeof(daynum_text), "%j", tick_time);
	  text_layer_set_text(text_daynum_layer, daynum_text);
  }

  char *time_format;
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }
  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(text_time_layer, time_text);
}

void handle_init() {
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  // week
  text_week_layer = text_layer_create(GRect(96, 8, 144-96, 168-8));
  text_layer_set_text_color(text_week_layer, GColorWhite);
  text_layer_set_background_color(text_week_layer, GColorClear);
  text_layer_set_font(text_week_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_REGULAR_18)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_week_layer));
	
  // daynumber
  text_daynum_layer = text_layer_create(GRect(100, 24, 144-100, 168-24));
  text_layer_set_text_color(text_daynum_layer, GColorWhite);
  text_layer_set_background_color(text_daynum_layer, GColorClear);
  text_layer_set_font(text_daynum_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_REGULAR_18)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_daynum_layer));

  // weekday
  text_day_layer = text_layer_create(GRect(8, 8, 144-8, 168-8));
  text_layer_set_text_color(text_day_layer, GColorWhite);
  text_layer_set_background_color(text_day_layer, GColorClear);
  text_layer_set_font(text_day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_REGULAR_18)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_day_layer));

  // day/month
  text_date_layer = text_layer_create(GRect(8, 24, 144-8, 168-24));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_REGULAR_18)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_date_layer));

  // time
  text_time_layer = text_layer_create(GRect(6, 53, 144-6, 168-53));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CONTINUUM_BOLD_48)));
//  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_BOLD_47)));	
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_time_layer));
	
  // year
  text_year_layer = text_layer_create(GRect(10, 140, 144-20, 168-140));
  text_layer_set_text_alignment(text_year_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_year_layer, GColorWhite);
  text_layer_set_background_color(text_year_layer, GColorClear);
  text_layer_set_font(text_year_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_REGULAR_18)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_year_layer));

  // line
  line_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(line_layer, line_layer_update_callback);
  layer_add_child(window_get_root_layer(window), line_layer);

  tick_timer_service_subscribe(MINUTE_UNIT, update_display);

  // Avoid blank display on launch
  time_t t = time(NULL);
  update_display(localtime(&t), MINUTE_UNIT);
}

void handle_deinit() {
  layer_destroy(line_layer);
  text_layer_destroy(text_daynum_layer);
  text_layer_destroy(text_year_layer);
  text_layer_destroy(text_time_layer);
  text_layer_destroy(text_date_layer);
  text_layer_destroy(text_week_layer);
  text_layer_destroy(text_day_layer);
  window_destroy(window);
}

int main() {
  handle_init();
  app_event_loop();
  handle_deinit();
  return 0;
}
