#include "pebble.h"

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ICONS 3
#define NUM_FIRST_MENU_ITEMS 15
#define KEY_DIST 3
#define KEY_ANG 1


static Window *s_main_window;
static Window *direction_window;
static MenuLayer *s_menu_layer;
static TextLayer *direction_text;

static GBitmap *s_menu_icons[NUM_MENU_ICONS];
static GBitmap *s_background_bitmap;

int angle;
int dist;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
      return NUM_FIRST_MENU_ITEMS;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {

      menu_cell_basic_header_draw(ctx, cell_layer, "Friends List");
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          // This is a basic menu item with a title and subtitle
          menu_cell_title_draw(ctx, cell_layer, "Joey");
          break;
        case 1:
          // This is a basic menu icon with a cycling icon
          menu_cell_title_draw(ctx, cell_layer, "Ming");
          break;
        case 2: 
          {
            // Here we use the graphics context to draw something different
            // In this case, we show a strip of a watchface's background
           menu_cell_title_draw(ctx, cell_layer, "Xin");
          }
          break;
        case 3:
          // This is a basic menu item with a title and subtitle
          menu_cell_title_draw(ctx, cell_layer, "James");
          break;
        default:
        menu_cell_title_draw(ctx, cell_layer, "xxxxx Item");
      }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Get the first pair
  //Tuple *t = dict_read_first(iterator);
  
  Tuple *dist_tuple = dict_find(iterator, KEY_DIST);
  Tuple *angle_tuple = dict_find(iterator , KEY_ANG); 
  static char s_buffer[255];
  if(dist_tuple){
    snprintf(s_buffer, sizeof(s_buffer), "Your friend is '%d' miles away. ", dist_tuple->value->uint8*69);
        text_layer_set_text(direction_text, s_buffer);
      APP_LOG(APP_LOG_LEVEL_INFO, s_buffer);
  }
  if(angle_tuple){
    snprintf(s_buffer, sizeof(s_buffer), "%sTurn towards '%d' degrees",s_buffer, angle_tuple->value->uint8);
        text_layer_set_text(direction_text, s_buffer);
      APP_LOG(APP_LOG_LEVEL_INFO, s_buffer);
  }
  
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!xxxxxxx");
}


static void window_load(Window *window) {
  direction_text = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(direction_text
  , GColorClear);
  text_layer_set_text_color(direction_text
  , GColorBlack);
  text_layer_set_text(direction_text
  , "Direction");
  app_message_register_inbox_received(inbox_received_callback);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(direction_text
  ));
 
}
static void window_unload(Window *window) {
  window_destroy(direction_window);
}

//clicked!
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
    // Error creating outbound message
    return;
  }
  
  dict_write_int(iter, 1 , &(cell_index->row), sizeof(int), true);
  dict_write_end(iter);

  app_message_outbox_send();
  
  direction_window=window_create();
    window_set_window_handlers(direction_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(direction_window, true);

}

static void main_window_load(Window *window) {

  // Now we prepare to initialize the menu layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void main_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(s_menu_layer);

  // Cleanup the menu icons
  for (int i = 0; i < NUM_MENU_ICONS; i++) {
    gbitmap_destroy(s_menu_icons[i]);
  }

  gbitmap_destroy(s_background_bitmap);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  app_message_open(64, 64);

}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
