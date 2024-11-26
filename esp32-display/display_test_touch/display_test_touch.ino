#include <TFT_eSPI.h> //2.5.0
#include "TouchLib.h"
TFT_eSPI tft = TFT_eSPI();

#define TOUCH_MODULES_CST_SELF

TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS820_SLAVE_ADDRESS, PIN_TOUCH_RES);

static void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
#if defined(TOUCH_READ_FROM_INTERRNUPT)
    if (get_int_signal) {
        get_int_signal = false;
        touch.read();
#else
    if (touch.read()) {
#endif
        String str_buf;
        uint8_t fn = touch.getPointNum();
        str_buf += " Finger num : " + String(fn) + " \n";
        for (uint8_t i = 0; i < fn; i++) {
            TP_Point t = touch.getPoint(i);
            str_buf += "x: " + String(t.x) + " y: " + String(t.y) + " p: " + String(t.pressure) + " \n";
        }
        TP_Point t = touch.getPoint(0);
        data->point.x = t.x;
        data->point.y = t.y;
        data->state = LV_INDEV_STATE_PR;
        lv_msg_send(MSG_NEW_TOUCH_POINT, str_buf.c_str());
    } else
        data->state = LV_INDEV_STATE_REL;
}

inited_touch = touch.init();
if (inited_touch) {
    touch.setRotation(1);
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_touchpad_read;
    lv_indev_drv_register(&indev_drv);
}

void setup() {
  // put your setup code here, to run once:
  tft.init();
  tft.fillScreen(TFT_BLACK);
  //top left is 0,0 so board is 170,320
  tft.setCursor(10, 10, 1); //x,y,front type  
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(3); //front size (1-7) bigger is higher
  tft.println("TFT_Test!");

  tft.fillCircle(30,150,10,TFT_BLUE); //x,y,r,color
  tft.drawLine(10,50,118,50,TFT_WHITE); //sx,sy,nx,ny,color
  //tft.drawPixel(70,70,TFT_RED); //x,y,color

  tft.setTextColor(TFT_WHITE, TFT_BLUE); //word color, bg color
  tft.setCursor(10, 80, 1);
  tft.println("GEJC!");

}

void loop() {
  // put your main code here, to run repeatedly:

}
