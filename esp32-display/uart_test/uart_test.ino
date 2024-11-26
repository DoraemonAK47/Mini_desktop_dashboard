#include <HardwareSerial.h>

HardwareSerial roomSerial(2);

int now2 = 0;
int pass2 = 0;
int intver2 = 1000;

int Rtemp = 0;
int Rhumid = 0;
int RlightLevel = 0;
String temp_string = "   ";

void read_serial(){
  if(roomSerial.available())
    {
      String data = roomSerial.readStringUntil('\n');
      Serial.println(data);
      senderMsgConvert(data);
    }
}

void senderMsgConvert(String message) {
  // find "temp:" and convert
  int tempIndex = message.indexOf("temp:");
  if (tempIndex != -1) {
    int tempEnd = message.indexOf(";", tempIndex); //find;
    String tempValue = message.substring(tempIndex + 5, tempEnd); //take value after temp
    Rtemp = tempValue.toInt(); // convert
  }

  // find "humid" and convert
  int humitIndex = message.indexOf("humid:");
  if (humitIndex != -1) {
    int humitEnd = message.indexOf(";", humitIndex); // find ";"
    String humitValue = message.substring(humitIndex + 6, humitEnd); // take value after humid
    Rhumid = humitValue.toInt(); // convert
  }

  // find "light" and convert
  int lightIndex = message.indexOf("light:");
  if (lightIndex != -1) {
    int lightEnd = message.indexOf(";", lightIndex); // find ";"
    String lightValue = message.substring(lightIndex + 6, lightEnd); // take value after light
    RlightLevel = lightValue.toInt(); // convert
  }
    //print
  Serial.print("Temperature: ");
  Serial.println(Rtemp);
  Serial.print("Humidity: ");
  Serial.println(Rhumid);
  Serial.print("Light Level: ");
  Serial.println(RlightLevel);
	//*/
}


void setup() {
    Serial.begin(115200);
    roomSerial.begin(115200, SERIAL_8N1, 43, 44);
    Serial.println("Lets Start");
    read_serial();
}

void loop() {
  now2 = millis();
  if (now2-pass2 > intver2){
    read_serial();
    pass2 = now2;
  }
}