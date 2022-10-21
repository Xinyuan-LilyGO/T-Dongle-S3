#include "lv_driver.h"
#include "TFT_eSPI.h"

#define LV_SCREEN_WIDTH  160
#define LV_SCREEN_HEIGHT 80
#define LV_BUF_SIZE      (LV_SCREEN_WIDTH * LV_SCREEN_HEIGHT)

extern TFT_eSPI tft;
extern uint8_t btn_press;

static void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h);
  lv_disp_flush_ready(disp);
}

// static void lv_btn_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
//   if (btn_press) {
//     btn_press = false;
//     data->state = LV_INDEV_STATE_PRESSED;
//     data->key = LV_KEY_NEXT;
//     Serial.println(__FUNCTION__);
//   }
// }

void lvgl_init(void) {
  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t *buf1;

  lv_init();
  buf1 = (lv_color_t *)heap_caps_malloc(LV_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_8BIT);
  assert(buf1);

  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, LV_BUF_SIZE);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = LV_SCREEN_WIDTH;
  disp_drv.ver_res = LV_SCREEN_HEIGHT;
  disp_drv.flush_cb = lv_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // static lv_indev_drv_t indev_drv;
  // lv_indev_drv_init(&indev_drv);
  // indev_drv.type = LV_INDEV_TYPE_KEYPAD;
  // indev_drv.read_cb = lv_btn_read;
  // lv_indev_drv_register(&indev_drv);
}