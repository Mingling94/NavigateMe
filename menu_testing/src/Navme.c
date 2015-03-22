#include "pebble.h"

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ICONS 3
#define NUM_FIRST_MENU_ITEMS 15
#define KEY_DATA 5

static Window *s_main_window;     // This is the main window which is a menu of names hard coded in
static Window *direction_window;  // This is the new window with the directionRep 
static MenuLayer *s_menu_layer;   // Menu layer with all the options
static TextLayer *directionRep;   // This is the direction rep for the page with the arrow
static GBitmap *s_menu_icons[NUM_MENU_ICONS]; 
static GBitmap *s_background_bitmap;

int angle;
int dist;

//This is the number of menu selections 
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}
// This is how long the list is in the menu
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
      return NUM_FIRST_MENU_ITEMS;
}
// Hieght of menu
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// This is the header for the menu on the main menu
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {

      menu_cell_basic_header_draw(ctx, cell_layer, "Active Friends");
}

// Manual input for the selections in main menu of names
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          // This is a basic menu item with a title and subtitle
          menu_cell_title_draw(ctx, cell_layer, "Joey ");
          break;
        case 1:
          // This is a basic menu icon with a cycling icon
          menu_cell_title_draw(ctx, cell_layer, "Ming");
          break;
        case 2: 
          {
            // Here we use the graphics context to draw something different
            // In this case, we show a strip of a watchface's background
           menu_cell_title_draw(ctx, cell_layer, "Shin");
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

//Recieves input from the .js in form of tuples and prints it. 
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Get the first pair
  Tuple *t = dict_read_first(iterator);

  // Process all pairs present
  while (t != NULL) {
    // Long lived buffer
    static char s_buffer[64];

    // Process this pair's key
    switch (t->key) {
      case KEY_ANG:
        // Copy value and display
        snprintf(s_buffer, sizeof(s_buffer), "Received '%s'", t->value->cstring);
        text_layer_set_text(directionRep, s_buffer);
        angle = t->value->int;
        break;
      case KEY_DIST:
        dist = t->value->cstring;
        break;
    }

    // Get next pair, if any
    t = dict_read_next(iterator);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

// This window load is for the direction window 
static void window_load(Window *window) {
  directionRep = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(directionRep, GColorClear);
  text_layer_set_text_color(directionRep, GColorBlack);
  text_layer_set_text(directionRep, "angle is "+ angle + " dist is "+ dist);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(directionRep));
}
static void window_unload(Window *window) {
  window_destroy(direction_window);
}

//Singifies that the button is clicked
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
    
    // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  
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
