#include <WiFi.h>
#include <PubSubClient.h>

// Replace the next variables with your SSID/Password combination
const char* ssid     = "Chu's Home";   //your network SSID
const char* password = "zxc19660330";   //your network password


// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
int value = 0;
const int interval = 5000; // 3 seconds
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

const char* topic_send = "esp32/hum";
//const char* topic_rev =  "esp32/test2";

const char* Message1 = "ON"; 
const char* Message2 = "OFF"; 
const char* Message3 = "UNK"; 
const char* rev[4];
int rev_size = sizeof(rev);

//value
float temperature = 0.0;
float humidity = 0.0;
int lightLevel = 0;

// LED Pin
const int ledPin = 4;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic_send);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  //rev[1]: update room temp
  //rev[2]: update room humid
  //rev[3]: update room light level
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(topic_send);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setModule (String thisMessage) {
	thisMessage.trim();
	String thisString = "";
	if (thisMessage.startsWith("LED:")) {			//light.....
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
		}
		
	} else if (thisMessage.startsWith("temp:")) {	//sender.......
		senderMsgConvert(thisMessage);
		setLight(1,0,0);
	
	} else if (thisMessage.startsWith("espCam:")) { //face lock........
		thisString = thisMessage.substring(7);
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


void loop() {
  if (!client.connected()) {
    reconnect();
    }
  client.loop();

  
  }
}