#include <TFT_eSPI.h> //2.5.0

TFT_eSPI tft = TFT_eSPI();

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
