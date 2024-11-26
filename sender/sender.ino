#include <DHT.h>
//#include <Adafruit GFX.h>
//#include <TFT_eSPI.h>
//#include <SPI.h>
//#include <Servo.h>
//#include <IRremote.h>

//MQTT shit.......................................................... 
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
 // init of lib objects
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient); 
	//CHANGE...............................................................................need change
	//wifi
//char ssid[] = "naivetoad";
char ssid[] = "macbook0123";
char pass[] = "12345678";
	//server
const char server[] = "test.mosquitto.org";
int        port     = 1883;
	//topics
const char topic[]  = "esp32/hum";
	//time stuff
const int interval = 5000; // 5 seconds
const long poll_interval = 3000;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

//pin setting....................................................
//sender
#define DHTPIN A1
#define DHTTYPE DHT11
#define lightSensorPIN A0
DHT dht (DHTPIN, DHTTYPE);
//receiver
#define redLight 2
#define yellowLight 3
#define greenLight 4
#define IRemitor 6
#define beeper 8

//func of MQTT......................................................
void connectWifi() {
// attempt to connect to Wi-Fi network:

  Serial.print("Attempting to connect to SSID '");
  Serial.print(ssid);
  Serial.print("' ");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(2000);
  }
  Serial.println();
  
  // successful connection to WiFi
  Serial.print("Connected to the WiFi network '");
  Serial.print(ssid);
  Serial.println("' successfully.");
  Serial.println();

}

void connectMQTT() {
// Attempt to connect to an MQTT server

  Serial.print("Attempting to connect to the MQTT server/server '");
  Serial.print(server);
  Serial.print("'");
  Serial.println();

  if (!mqttClient.connect(server, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    
    while (1);
  }

  Serial.print("Connected to the MQTT server/server '");
  Serial.print(server);
  Serial.println("' successfully.");
  Serial.println();

}
//func of hard ware.................................................
//DHT Humid and Temperature
float readHumid () {
  return dht.readHumidity();
} 
float readTemp () {
  return dht.readTemperature();
}
//Light sensor
int readLight () {
	const int DARKEST = 450;
	const int LIGHTEST = 70;
	int thisValue = map(analogRead(lightSensorPIN),DARKEST,LIGHTEST,0,100);
	if (thisValue < 0) { thisValue = 4; }
	if (thisValue > 100) { thisValue = 100; }
	
  return thisValue;
}
//set light
void setLight (bool red, bool yellow, bool green) {
  digitalWrite(redLight,red);
  digitalWrite(yellowLight,yellow);
  digitalWrite(greenLight,green);
}

//output value 
void displayValue (bool IsDisplayDHT_humid, bool IsDisplayDHT_temp, bool IsDisplayLS) {
  bool IsDisplay = IsDisplayDHT_humid|IsDisplayDHT_temp|IsDisplayLS;
  if (IsDisplayDHT_humid) {
    Serial.print("Humid: ");
    Serial.print(readHumid());
  }
  if (IsDisplayDHT_temp) {
    Serial.print(" Temp: ");
    Serial.print(readTemp());
  }
  if (IsDisplayLS){
    Serial.print(" LightValue: ");
    Serial.print(readLight());
  }
  //end 
  if (IsDisplay) {
    Serial.println();
  }
}
void sendMessage () {
	String payload = "temp:" + String(readTemp()) + "; " +
					"humid:" + String(readHumid()) + "; " +
					"light:" + String(readLight()) + ";" ;
					
	mqttClient.beginMessage(topic);
	mqttClient.print(payload);
	mqttClient.endMessage();				
	Serial.print("Sending message to topic sucessfully:'");
	Serial.print(topic);
	Serial.print("' :");
	Serial.println(payload);
	Serial1.println(payload);
	Serial.println();	
		
}

void setup() {

 //Initialize serial and wait for port to open:
  Serial.begin(9600);
  Serial1.begin(115200);
  
  pinMode(LED_BUILTIN,OUTPUT);
  delay(2000);
  
  Serial.println("******* MQTT Client Publisher *******");
  Serial.println("*                                   *");
  Serial.println("*   One Message every 5 seconds     *");
  Serial.println("*                                   *");
  Serial.println("*  Polling for keep alive : 1 sec   *");
  Serial.println("*                                   *");
  Serial.println("*************************************");
  Serial.println(); 
  delay(5000);

  // connect to WiFi
  connectWifi();

  // attempt to connect to MQTT server network:
  Serial.print("Attempting to connect to the MQTT server: ");
  Serial.println(server);

  if (!mqttClient.connect(server, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.print("Connected successfully to the MQTT server: ");
  Serial.println(server);
  Serial.println();

}

void loop() {
	// call poll() once a second to allow the library to send MQTT keep-alive which
  // avoids being disconnected by the server
  //delay(1000);
  mqttClient.poll();
  currentMillis = millis();
  analogReadResolution(12);
	Serial.print(analogRead(lightSensorPIN));
  if (currentMillis - previousMillis >= interval) {
    // more than interval (5 seconds) have elasped 
    previousMillis = currentMillis;
    sendMessage();	
  }
}
