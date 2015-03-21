#include "pebble.h"

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ICONS 3
#define NUM_FIRST_MENU_ITEMS 15

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static GBitmap *s_menu_icons[NUM_MENU_ICONS];
static GBitmap *s_background_bitmap;

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

      menu_cell_basic_header_draw(ctx, cell_layer, "Some example items");
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          // This is a basic menu item with a title and subtitle
          menu_cell_title_draw(ctx, cell_layer, "First Item");
          break;
        case 1:
          // This is a basic menu icon with a cycling icon
          menu_cell_title_draw(ctx, cell_layer, "Second Item");
          break;
        case 2: 
          {
            // Here we use the graphics context to draw something different
            // In this case, we show a strip of a watchface's background
           menu_cell_title_draw(ctx, cell_layer, "Third Item");
          }
          break;
        case 3:
          // This is a basic menu item with a title and subtitle
          menu_cell_title_draw(ctx, cell_layer, "Final Item");
          break;
        default:
        menu_cell_title_draw(ctx, cell_layer, "xxxxx Item");
      }
}


//clicked!
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
    // Error creating outbound message
    return;
  }
  int ID = cell_index->row;
  dict_write_int(iter, 1 , &ID, sizeof(int), true);
  dict_write_end(iter);

  app_message_outbox_send();

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
