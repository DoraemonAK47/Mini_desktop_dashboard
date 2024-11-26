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
char ssid[] = "macbook0123";
char pass[] = "12345678";
	//server
const char server[] = "test.mosquitto.org";
int        port     = 1883;
	//topics
const char topic[]  = "esp32/hum";
	//time stuff
const int interval = 5000; // 5 seconds
const long poll_interval = 5000;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;




//pin setting....................................................
	//sender....
#define DHTPIN A1
#define DHTTYPE DHT11
#define lightSensorPIN A0
DHT dht (DHTPIN, DHTTYPE);
#define redLight 2
#define yellowLight 3
#define greenLight 4
#define IRemitor 6
#define beeper 8

//value
float temperature = 0.0;
float humidity = 0.0;
int lightLevel = 0;

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

void keep_alive() {
  // read current time from system function
  currentMillis = millis();  

  if (currentMillis - previousMillis >= poll_interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;
    // poll the server to "keep alive"
    mqttClient.poll();
  }
}

void onMqttMessage(int messageSize) {
// Process subscribed message as it arrives

  String Topic_string = "";
  String Message_string = "";
  
  Topic_string=mqttClient.messageTopic();
  Serial.print("Received message is on topic '");
  Serial.print(Topic_string);
  Serial.println("'");

  Serial.print("Received message size is '");
  Serial.print(messageSize);
  Serial.println("' bytes");

  // Gather the received message character by character into a string

  for (int i = 0; i < messageSize; i++) {
    Message_string=Message_string+(char)mqttClient.read();
  }
	Message_string.trim();
  Serial.print("Received message is '");
  Serial.print(Message_string);
  Serial.print("'");
  Serial.println();
  

//do shit after receive word.....
  setModule(Message_string);
  
  
  //*/
  delay(10);
}

//convert message 
void setModule (String thisMessage) {
	thisMessage.trim();
	String thisString = "";
	if (thisMessage.startsWith("LED:")) {			//light.....
		
		//show loading.......
		setLight(1,0,0);
		delay(100);
		setLight(1,1,0);
		delay(100);
		setLight(1,1,1);
		delay(100);
		for(int i =0; i<4; i++){
			setLight(1,1,1);
			delay(80);
			setLight(0,0,0);
			delay(80);		
		}
		
		thisString = thisMessage.substring(4);
		thisString.trim();
		if (thisString == "red") {
			setLight(1,0,0);
		} else if (thisString == "yellow") {
			setLight(0,1,0);
		} else if (thisString == "green") {
			setLight(0,0,1);
		} else if (thisString == "off") {
			setLight(0,0,0);
		} else if (thisString == "on") {
			setLight(1,1,1);
		}
		
	} else if (thisMessage.startsWith("temp:")) {	//sender.......
		
		//show loading.......
		setLight(1,0,0);
		delay(100);
		setLight(1,1,0);
		delay(100);
		setLight(1,1,1);
		delay(100);
		for(int i =0; i<4; i++){
			setLight(1,0,1);
			delay(50);
			setLight(0,1,0);
			delay(50);		
		}
		senderMsgConvert(thisMessage);
		setLight(1,0,0);
		delay(500);
		setLight(0,0,0);
		
	} else if (thisMessage.startsWith("espCam:")) { //face lock........
		thisString = thisMessage.substring(7);
		if (thisString == "UNK") {
			for (int i=0; i<10; i++) {
				setLight(1,0,0);
				digitalWrite(8,HIGH);
				delay(50);
				setLight(0,0,0);
				digitalWrite(8,HIGH);
				delay(50);
			}
			
		}
	} 
}

void senderMsgConvert(String message) {
  // find "temp:" and convert
  int tempIndex = message.indexOf("temp:");
  if (tempIndex != -1) {
    int tempEnd = message.indexOf(";", tempIndex); //find;
    String tempValue = message.substring(tempIndex + 5, tempEnd); //take value after temp
    temperature = tempValue.toFloat(); // convert
  }

  // find "humid" and convert
  int humitIndex = message.indexOf("humid:");
  if (humitIndex != -1) {
    int humitEnd = message.indexOf(";", humitIndex); // find ";"
    String humitValue = message.substring(humitIndex + 6, humitEnd); // take value after humid
    humidity = humitValue.toFloat(); // convert
  }

  // find "light" and convert
  int lightIndex = message.indexOf("light:");
  if (lightIndex != -1) {
    int lightEnd = message.indexOf(";", lightIndex); // find ";"
    String lightValue = message.substring(lightIndex + 6, lightEnd); // take value after light
    lightLevel = lightValue.toInt(); // convert
  }

  //print
	
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Light Level: ");
  Serial.println(lightLevel);
	//*/
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
  return analogRead(lightSensorPIN);
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

void setup() {
  Serial.begin(9600);
  //MQTT shit........
  delay(2000);
  Serial.println("********** MQTT Subscribe Part **********");
  Serial.println();
  // connect to WiFi
  connectWifi();
  // connect to MQTT server
  connectMQTT();
  Serial.print("Keep alive interval is set at: ");
  Serial.print(poll_interval);
  Serial.println(" msec");
  Serial.println();
  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);
  Serial.print("Subscribing to topic: '");
  Serial.print(topic);
  Serial.println("'");
  Serial.println();
  // subscribe to a topic
  // the second parameter sets the QoS of the subscription,
  // the library supports subscribing at QoS 0, 1, or 2
  int subscribeQos = 1;
  mqttClient.subscribe(topic,subscribeQos);
  Serial.print("Waiting for messages on topic: '");
  Serial.print(topic);
  Serial.println("' ...");
  Serial.println();
  
  //hardware......
  dht.begin();
  pinMode(beeper, OUTPUT);
  //testing light
  setLight(1,0,0);
  delay(1000);
  setLight(0,1,0);
  delay(1000);
  setLight(0,0,1);
  delay(1000);
}

int i = 0;
void loop() {
  // put your main code here, to run repeatedly:
	keep_alive();		
}
