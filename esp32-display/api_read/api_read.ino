#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>


const char* ssid     = "Chu's Home";   //your network SSID
const char* password = "zxc19660330";   //your network password

String url = "https://data.weather.gov.hk/weatherAPI/opendata/weather.php";
String dataType = "rhrread";
String lang = "en";


void setup() {
  Serial.begin(115200);
  setup_wifi();
  http_use();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

  unsigned int temp = doc["temperature"]["data"][15]["value"].as<unsigned int>();
  unsigned int humid = doc["humidity"]["data"][0]["value"].as<unsigned int>();
  String info = doc["icon"].as<String>();
  String weather;
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


  Serial.printf("temp: %d\n", temp);
  Serial.printf("humid: %d\n", humid);
  Serial.printf("weather: %s\n", weather);
}

void loop() {
  // put your main code here, to run repeatedly:

}
