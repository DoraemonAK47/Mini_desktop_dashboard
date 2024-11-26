#include <lvgl.h> //using 8.3.11 own
#include <TFT_eSPI.h>
#include "ui.h"

void setup() {
  Serial.begin(115200);
  lv_init();

}

void loop() {
  lv_task_handler();
  lv_tick_inc(5);
  delay(5);
}
