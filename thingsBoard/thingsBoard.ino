
//Version 6
#include <ArduinoJson.h>

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define WIFI_AP "Pablo"
#define WIFI_PASSWORD "pablopablo"

#define NODE_NAME "Blackout-Machine"  
#define NODE_TOKEN "HNRJK5bhyvIBkwvBziv8"

char thingsboardServer[] = "demo.thingsboard.io";
char requestTopic[] = "v1/devices/me/rpc/request/+";
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  connectToWiFi();
  client.setServer( thingsboardServer, 1883 );

  // agregado para recibir callbacks
  client.setCallback(on_message);
   
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }
  client.loop();
}

void on_message(const char* topic, byte* payload, unsigned int length) 
{
  // Mostrar datos recibidos del servidor
  Serial.println("On message");  

  const int capacity = JSON_OBJECT_SIZE(16);
  StaticJsonDocument<capacity> doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());\
    return;
  }
  String params = doc["params"];
  Serial.println(params);
    
  const size_t ARRAY_CAPACITY = JSON_ARRAY_SIZE(25);
  StaticJsonDocument<ARRAY_CAPACITY> docArray;
  deserializeJson(docArray, params, DeserializationOption::NestingLimit(4));
  
  // extract the values
  JsonArray jArray = docArray.as<JsonArray>();
  for(JsonVariant recipeStepElement : jArray) {
    JsonArray recipeStepArray = recipeStepElement.as<JsonArray>();
    
    String recipeStepStr = recipeStepElement.as<String>();
    Serial.println(recipeStepStr);
    Serial.print("Ingredient: ");
    Serial.println(recipeStepArray[0].as<int>());
    Serial.print("Time: ");
    Serial.println(recipeStepArray[1].as<int>());
  }
}


void reconnect() {
  int statusWifi = WL_IDLE_STATUS;
  // Loop until we're reconnected
  while (!client.connected()) {
    statusWifi = WiFi.status();
    connectToWiFi();
    
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect(NODE_NAME, NODE_TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
      
      // Suscribir al Topico de request
      client.subscribe(requestTopic); 
      
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

void connectToWiFi()
{
  Serial.println("Connecting to WiFi ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}
