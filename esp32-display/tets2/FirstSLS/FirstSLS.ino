#include "LV_Helper.h"
#include "ui.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

HardwareSerial roomSerial(2);

//const char* ssid     = "EIE_Lab2";   //your network SSID
const char* ssid     = "macbook0123";   //your network SSID
const char* password = "12345678";   //your network password

String url = "https://data.weather.gov.hk/weatherAPI/opendata/weather.php";
String dataType = "rhrread";
String lang = "en";

const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "stdtime.gov.hk");

String today;
String todate;
String toHour;

char msg[50];
int value = 0;
const int interval = 5000; // 3 seconds
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

const char* topic_send = "esp32/hum";
//const char* topic_rev =  "esp32/test2";

const char* Message1 = "espCam:ON"; 
const char* Message2 = "espCam:OFF"; 
const char* Message3 = "espCam:UNK"; 
const char* rev[4];
int rev_size = sizeof(rev);

int now = 0;
int pass = 0;
int intver = 5000;

int now1 = 0;
int now2 = 0;

int pass1 = 0;
int intver1 = 5000;

int pass2 = 0;
int intver2 = 60000;

//value
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
  //Serial.print("Temperature: ");
  //Serial.println(Rtemp);
  lv_arc_set_value(ui_Arc5, Rtemp); //temp ar5
  lv_event_send(ui_Arc5, LV_EVENT_VALUE_CHANGED, NULL);
  //Serial.print("Humidity: ");
  //Serial.println(Rhumid);
  lv_arc_set_value(ui_Arc2, Rhumid); //humid arc2
  lv_event_send(ui_Arc2, LV_EVENT_VALUE_CHANGED, NULL);
  //Serial.print("Light Level: ");
  //Serial.println(RlightLevel);

  if (RlightLevel<5){
    lv_obj_clear_state(ui_Switch3, LV_STATE_CHECKED);
    lv_slider_set_value(ui_LightLevel, 0, LV_ANIM_OFF); //set RlightLevel
  }else if(RlightLevel>=10){
    lv_obj_add_state(ui_Switch3, LV_STATE_CHECKED);
    lv_slider_set_value(ui_LightLevel, RlightLevel, LV_ANIM_OFF); //set RlightLevel
  }

	//*/
}

void setup() {
    Serial.begin(115200);
    Serial.println("Lets Start");
    roomSerial.begin(115200, SERIAL_8N1, 43, 44);
    
    lv_helper();
    ui_init();

    analogWrite(38,120);

    setup_wifi();
    setup_ntp();
    http_use();
    ntp_use();
    //client.setServer(mqtt_server, 1883);
    //client.setCallback(callback);
    //client.connect("ESP8266Client");
    //client.subscribe(topic_send);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_ntp(){
  timeClient.begin();
  timeClient.setTimeOffset(28800);
}

void http_use(){
  //http
  HTTPClient http;
  http.begin(url+"?dataType="+dataType+"&lang="+lang);
  int httpCode = http.GET();
  Serial.printf("http code: %d", httpCode);
  String response = http.getString();
  Serial.print("response: ");
  Serial.println(response);
  http.end();

  DynamicJsonDocument doc(1024);

  deserializeJson(doc, response);

  unsigned int temp_int = doc["temperature"]["data"][15]["value"].as<unsigned int>();
  unsigned int humid_int = doc["humidity"]["data"][0]["value"].as<unsigned int>();
  String info = doc["icon"].as<String>();

  char* weather;
  if(info == "[50]"){  weather = "Sunny";}
  if(info == "[51]") { weather = "Sunny Periods";}
  if(info == "[52]") { weather = "Sunny Intervals";}
  if(info == "[53]") { weather = "Sunny Periods with A Few Showers";}
  if(info == "[54]") { weather = "Sunny Intervals with Showers";}
  if(info == "[60]") { weather = "Cloudy";}
  if(info == "[61]") { weather = "Overcast";}
  if(info == "[62]") { weather = "Light Rain";}
  if(info == "[63]") { weather = "Rain";}
  if(info == "[64]") { weather = "Heavy Rain";}
  if(info == "[65]") { weather = "Thunderstorms";}
  if(info == "[81]") { weather = "Dry";}
  if(info == "[82]") { weather = "Humid";}
  if(info == "[92]") { weather = "Cool";}
  if(info == "[93]") { weather = "Cold";}


  Serial.printf("temp: %d\n", temp_int);
  lv_slider_set_value(ui_Slider2, temp_int, LV_ANIM_OFF); //temp silde
  lv_event_send(ui_Slider2, LV_EVENT_VALUE_CHANGED, NULL);

  Serial.printf("humid: %d\n", humid_int);
  lv_slider_set_value(ui_Slider1, humid_int, LV_ANIM_OFF); //humid silde
  lv_event_send(ui_Slider1, LV_EVENT_VALUE_CHANGED, NULL);

  Serial.printf("weather: %s\n", weather);
  lv_label_set_text(ui_Label_WeatherS, weather); //set weather
}

void ntp_use(){
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();

  struct tm *ptm = gmtime((time_t*)&epochTime);
  int currentYear = ptm->tm_year + 1900;
  int currentMonth = ptm->tm_mon + 1;
  int currentMonthDay = ptm->tm_mday;
  int currentWeekDay = ptm->tm_wday;
  int currentHour = ptm->tm_hour;
  int currentMin = ptm->tm_min;
  int currentSec = ptm->tm_sec;

  const char weekdays[8][10] = {"0", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

  Serial.println("Epoch Time: " + String(epochTime));
  Serial.println(timeClient.getFormattedTime());

  today = String(currentMonthDay) + "/" + String(currentMonth) + "  " + String(weekdays[currentWeekDay]);
  toHour = String(currentHour) + ":" + String(currentMin);

  Serial.println("Current Week Day: " + today);
  Serial.println("Current Hours: " + toHour);
  lv_label_set_text(ui_Time1, toHour.c_str());
  lv_label_set_text(ui_Time3, toHour.c_str());
  lv_label_set_text(ui_Time, toHour.c_str());

  lv_label_set_text(ui_Date1, today.c_str());
  lv_label_set_text(ui_Date3, today.c_str());
  lv_label_set_text(ui_Date, today.c_str());
}

/*void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic_send);
  Serial.print(". Message: ");
  String messageTemp;
    
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  temp_string = message[5] + message[6];
  Rtemp = temp_string.toInt();
  temp_string = message[18] + message[19];
  Rhumid = temp_string.toInt();
  temp_string = message[31] + message[32] + message[33];
  RlightLevel = temp_string.toInt();

  Serial.printf("temp: %d\n", Rtemp);
  lv_arc_set_value(ui_Arc5, Rtemp); //temp ar5
  lv_event_send(ui_Arc5, LV_EVENT_VALUE_CHANGED, NULL);

  Serial.printf("humid: %d\n", Rhumid);
  lv_arc_set_value(ui_Arc2, Rhumid); //humid arc2
  lv_event_send(ui_Arc2, LV_EVENT_VALUE_CHANGED, NULL);

  Serial.printf("RlightLevel: %d\n", RlightLevel);
  lv_slider_set_value(ui_LightLevel, RlightLevel, LV_ANIM_OFF); //set RlightLevel


  if(rev[0] == Message3){
      //set dashboard
    }
  //rev[1]: update room temp
  //rev[2]: update room humid
  //rev[3]: update room light level
}*/

/*void reconnect() {
  // Loop until we're reconnected

    while (!client.connected()) {
        now = millis();
        if (now-pass > intver){
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        WiFi.mode(WIFI_STA);
        if (client.connect("ESP8266Client")) {
          Serial.println("connected");
          // Subscribe
          client.subscribe(topic_send);
        } else {
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 5 seconds");
        }pass = now;
      }else{break;}
    
  }
}*/


void loop() {
  now1 = millis();
  if (now1-pass1 > intver1){
    read_serial();
    pass1 = now1;
  }
  lv_task_handler();
  client.loop();
  
  now2 = millis();
  if (now2-pass2 > intver2){
    http_use();
    ntp_use();
    pass2 = now2;
  }
  //read_serial();
  //Rupdate();
  delay(5);
  
  

}
