
//Version 6
#include <ArduinoJson.h>

#include <AccelStepper.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define WIFI_AP "Pablo"
#define WIFI_PASSWORD "pablopablo"

#define NODE_NAME "Blackout-Machine"  
#define NODE_TOKEN "HNRJK5bhyvIBkwvBziv8"

#define avoidPin D0
#define pump1Pin D1
#define pump2Pin D2
#define pump3Pin D3
// Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
//PROBAR CAMBIAR D6 por D7 (ANDA MEJOR)?
AccelStepper stepper(AccelStepper::FULL4WIRE, D5, D6, D7, D8);


char thingsboardServer[] = "demo.thingsboard.io";
char requestTopic[] = "v1/devices/me/rpc/request/+";
WiFiClient wifiClient;
PubSubClient client(wifiClient);

//STATES
int RECIVING_STATE = 0;
int GO_TO_ING_STATE = 1;
int SERVING_STATE = 2;
int CALIBRATING_STATE = 3;
int state = RECIVING_STATE;

//INGREDIENT DATA
JsonArray ingredientArray;
int ingredientNo = 0;

void setup() {
  Serial.begin(9600);
  
  stepper.setMaxSpeed(1000);
  stepper.setSpeed(512);
  stepper.setAcceleration(128); 
   
  connectToWiFi();
  client.setServer( thingsboardServer, 1883 );
  client.setCallback(on_message);
}

void loop() {
  if(state == RECIVING_STATE){
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }else{
    manageDiskLoop();
  }
}

void manageDiskLoop(){
    if(state == GO_TO_ING_STATE){
      int ingredientPos = ingredientArray[ingredientNo][0];
      Serial.println(ingredientPos);
      moveDiskToPosition(ingredientPos);
    
    }else if(state == SERVING_STATE){
      serve();
      
    }else if(state == CALIBRATING_STATE){
      int avoidVal = digitalRead(avoidPin);
      if(avoidVal == LOW){
        Serial.println("Detectado");
        ingredientNo = 0; 
        state = RECIVING_STATE;
      }else{
        Serial.println("REUBICANDO");
        relocate();    
      }
    }
}


void moveDiskToPosition(int positionToMoveDisk){
  boolean arrivedAtIng = stepper.currentPosition() == positionToMoveDisk;

  if (arrivedAtIng) {
    state = SERVING_STATE;
  
  }else{
    Serial.println("Going to ing:" + positionToMoveDisk);
    stepper.setMaxSpeed(1000);
    stepper.setSpeed(512);
    stepper.setAcceleration(512); 
    stepper.moveTo(positionToMoveDisk);
    stepper.run();   
  }
}

void serve(){
  int serveTime = ingredientArray[ingredientNo][1];
  int pumpPin = getPumpPinForIng(ingredientArray[ingredientNo][2]);
  openPumpForTime(pumpPin, serveTime);
  ingredientNo++;
  bool moreIng = !ingredientArray.size() == ingredientNo;
  if(!moreIng){
    state = CALIBRATING_STATE;
    ingredientNo = 0;
  }
}

int getPumpPinForIng(int ingredient){
    if(ingredient == 1){
      return 1;
    }else if(ingredient == 2){
      return 2;
    }else if(ingredient == 3){
      return 3;
    }
}

void openPumpForTime(int pumpPin, int seconds){
 //TODO change 
  analogWrite(pump1Pin, 400);
  delay(seconds * 1000);
  analogWrite(pump1Pin, 0);
}

void relocate(){
  stepper.setSpeed(512);  
  stepper.runSpeed();
}


void on_message(const char* topic, byte* payload, unsigned int length) 
{
  // Mostrar datos recibidos del servidor
  Serial.println("On message");  

  const int capacity = JSON_OBJECT_SIZE(50);
  StaticJsonDocument<capacity> doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());\
    return;
  }
  String params = doc["params"];
  Serial.println(params);
    
  const size_t ARRAY_CAPACITY = JSON_ARRAY_SIZE(50);
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
  ingredientArray = jArray;
  state = GO_TO_ING_STATE;
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
