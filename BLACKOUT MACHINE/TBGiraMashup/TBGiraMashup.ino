
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

//EXTRA 
//boolean done = false;
//String json = "[[512,5,1],[1024,5,2]]";
//"[[1024,5,2],[1536,1,3]]"


char thingsboardServer[] = "demo.thingsboard.io";
char telemetryTopic[] = "v1/devices/me/telemetry";
char requestTopic[] = "v1/devices/me/rpc/request/+";
char attributesTopic[] = "v1/devices/me/attributes";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

//STATES
int RECIVING_STATE = 0;
int GO_TO_ING_STATE = 1;
int SERVING_STATE = 2;
int CALIBRATING_STATE = 3;
int state = CALIBRATING_STATE;

//INGREDIENT DATA
JsonArray ingredientArray;
int ingredientNo = 0;

void setup() {
  Serial.begin(9600);
      
  stepper.setMaxSpeed(1000);
  stepper.setSpeed(512);
  stepper.setAcceleration(128); 

  pinMode(D0, INPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  
  analogWriteFreq(20);
  analogWrite(pump1Pin, 0);
  analogWrite(pump2Pin, 0);
  analogWrite(pump3Pin, 0);
   
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

      /*
      // ------------------EXTRA
      const size_t ARRAY_CAPACITY = JSON_ARRAY_SIZE(50);
      StaticJsonDocument<ARRAY_CAPACITY> docArray;
      deserializeJson(docArray, json, DeserializationOption::NestingLimit(4));
      // extract the values
      JsonArray jArray = docArray.as<JsonArray>();
      ingredientArray = jArray;
      // -------------------END
      */
      
      int ingredientPos = ingredientArray[ingredientNo][0];
      Serial.print("ingpos:_");
      Serial.print(ingredientPos);
      moveDiskToPosition(ingredientPos);
    
    }else if(state == SERVING_STATE){
      serve();
      
    }else if(state == CALIBRATING_STATE){
      int avoidVal = digitalRead(avoidPin);
      if(avoidVal == LOW){
        Serial.println("Detectado");
        stepper.setCurrentPosition(0); 
        ingredientNo = 0; 
        state = RECIVING_STATE;  
        sendStateToThingsBoard();
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
    sendStateToThingsBoard();
  }else{
    Serial.print("Going to ing:");
    Serial.print(positionToMoveDisk);
    stepper.setMaxSpeed(1000);
    stepper.setSpeed(512);
    stepper.setAcceleration(512); 
    stepper.moveTo(positionToMoveDisk);
    stepper.run();   
  }
}

void serve(){
  /*
  // ------------------EXTRA
      const size_t ARRAY_CAPACITY = JSON_ARRAY_SIZE(50);
      StaticJsonDocument<ARRAY_CAPACITY> docArray;
      deserializeJson(docArray, json, DeserializationOption::NestingLimit(4));
      // extract the values
      JsonArray jArray = docArray.as<JsonArray>();
      ingredientArray = jArray;
  // -------------------END
    */
      
  int serveTime = ingredientArray[ingredientNo][1];
  int pumpPin = getPumpPinForIng(ingredientArray[ingredientNo][2]);
  openPumpForTime(pumpPin, serveTime);
  
  ingredientNo = ingredientNo + 1;
  Serial.print("newingNumber:");
  Serial.println(ingredientNo);

  int arraySize = ingredientArray.size();
  bool noMoreIng = arraySize == ingredientNo;
  
  if(noMoreIng){
    Serial.println("No more ing");
    state = CALIBRATING_STATE;
    sendStateToThingsBoard();
    ingredientNo = 0;
  }else{
    state = GO_TO_ING_STATE;
    sendStateToThingsBoard();
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
  if(pumpPin == 1){
      analogWrite(pump1Pin, 400);
      delay(seconds * 1000);
      analogWrite(pump1Pin, 0);
  }else if(pumpPin == 2){
      analogWrite(pump2Pin, 400);
      delay(seconds * 1000);
      analogWrite(pump2Pin, 0);
  }else if(pumpPin == 3){
      analogWrite(pump3Pin, 400);
      delay(seconds * 1000);
      analogWrite(pump3Pin, 0);
  }
  
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
  sendStateToThingsBoard();
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

void sendStateToThingsBoard()
{
   const int capacity = JSON_OBJECT_SIZE(4);
   StaticJsonDocument<capacity> doc;
   
   // serverDispenseButtonState = response;
      
    //Update card
    doc["state"] = getStateName(state);

    
    String output = "";
    serializeJson(doc, output);
    
    char attributes[100];
    output.toCharArray( attributes, 100 );
/* 
    Serial.print("respuesta aributos: ");
    Serial.println(attributes);
*/
    // se envia la repsuesta la cual se despliegan en las tarjetas creadas para el atrubito 
    client.publish(attributesTopic, attributes);
}

String getStateName(){
  if(state==0){
    return "RECIVING_STATE";
  }else if (state ==1 ){
    return "GO_TO_ING_STATE";
  }else if (state ==2){
    return "SERVING_STATE";
  }else if (state ==3){
    return "CALIBRATING_STATE";
  }
}


