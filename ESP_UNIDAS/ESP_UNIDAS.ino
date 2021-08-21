// Codigo que une el ESP1 y la ESP2

// Comunicacion librerias
#include "ThingSpeak.h"
#include "WiFi.h"

// OX
#include <SoftwareSerial.h>
#define rx 33                                          //define what pin rx is going to be
#define tx 32                                          //define what pin tx is going to be
SoftwareSerial myserial(rx, tx);                      //define how the soft serial port is going to work
#define CON 2

// Global Variables
String inputstring = "";                              //a string to hold incoming data from the PC
String sensorstring = "";                             //a string to hold the data from the Atlas Scientific product
boolean input_string_complete = false;                //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
float DO;                                             //used to hold a floating point number that is the DO
int ox;
int OxCopia = 0;

// Pins
static const int adc_pin = 26;
int val = 0;
#define CON 2

// Motor Settings
int LED1 = 13;
int LED2 = 12;
const int canalPWM = 0;
const int canalPWM1 = 1;
const int freq = 2000;
const int resolucion = 8;

// Global variable
static byte TheState = 1;
static byte TheUdstate = 1;
int ud = 0;

// Comunicacion definiciones

// WiFi Credentials
const char* ssid = "INFINITUMC47D";
const char* password = "2705483869";

// ThingSpeak credentials
unsigned long channelID = 1229566;
const char* WriteAPIKey = "O07LIAVZQU2APO95";

WiFiClient cliente;

// TASKS

// Prepare OD EMBEDDED
void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}

// Get DO data and send it by HTTP
 void GetOx(void *parameters) {
  while(1){
  // GetOx
  if (TheState == 1){
        ud = 0;
    if (input_string_complete == true) {                //if a string from the PC has been received in its entirety
    myserial.print(inputstring);                      //send that string to the Atlas Scientific product
    myserial.print('\r');                             //add a <CR> to the end of the string
    inputstring = "";                                 //clear the string
    input_string_complete = false;                    //reset the flag used to tell if we have received a completed string from the PC
  }

  if (myserial.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)myserial.read();              //get the char we just received
    sensorstring += inchar;                           //add the char to the var called sensorstring
    if (inchar == '\r') {                             //if the incoming character is a <CR>
      sensor_string_complete = true;                  //set the flag
    }
  }


  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    //Serial.println(sensorstring);                   // Print sensor string
    ox = sensorstring.toInt(); 
    OxCopia = ox;
       if(OxCopia > 2){
       Serial.println("El oxígeno es: " + String(OxCopia));
       ThingSpeak.setField (5,ud);
       ThingSpeak.setField (4,OxCopia);
        //sensor_string_complete = false;+
       vTaskDelay(1000 / portTICK_PERIOD_MS);
       ThingSpeak.writeFields(channelID,WriteAPIKey);
       Serial.println("Datos enviados a ThingSpeak!");
       }

    //listo_para_enviar = true;
    sensorstring = "";                                //clear the string
    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
    
  }
 }
  else{
    ud = 1;
   ThingSpeak.setField (5,ud);
   vTaskDelay(1000 / portTICK_PERIOD_MS);
   ThingSpeak.writeFields(channelID,WriteAPIKey);
   Serial.println("Datos enviados a ThingSpeak!");
  }
 }
}

// Control de motor
void MotorCtrl(void *parameters){
  while(1){
    switch(TheUdstate){
    case 1:
    //Serial.println("La medida es \t" + String(val));
    //digitalWrite(LED1, HIGH);
    //delay(3000);
    //digitalWrite(LED1, LOW);
    for (int motorValue = 0; motorValue <= 100; motorValue += 10){
        ledcWrite(canalPWM, motorValue);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }

    for (int motorValue = 100; motorValue >= 0; motorValue -= 10){
        ledcWrite(canalPWM, motorValue);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    for(int x = 0; x<5; x++){

      Serial.println("\tEl sensor está entrando");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    Serial.println("\tEl sensor está dentro!");
  break;

  case 2:
  //Serial.println("La medida es \t" + String(val));
    //digitalWrite(LED2, HIGH);
    //delay(3000);
    //digitalWrite(LED2, LOW);
    for (int motorValue = 0; motorValue <= 100; motorValue += 10){
        ledcWrite(canalPWM1, motorValue);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }

    for (int motorValue = 100; motorValue >=0; motorValue -= 10){
        ledcWrite(canalPWM1, motorValue);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    for(int y = 0; y<10; y++){

      Serial.println("\tEl sensor está saliendo");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    Serial.println("\tEl sensor está afuera!");
  break;
  }
    //Wait for a while
    vTaskDelay(2000 / portTICK_PERIOD_MS);
 }
 
}

void setup() {

  // Initialize Serial and settings
  Serial.begin(9600);
  myserial.begin(9600);                               //set baud rate for the software serial port to 9600
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product

  // Motor begin
  ledcSetup(canalPWM, freq, resolucion);
  ledcSetup(canalPWM1, freq, resolucion);
  ledcAttachPin(LED1, canalPWM);
  ledcAttachPin(LED2, canalPWM1);
  pinMode(CON, OUTPUT);
  digitalWrite(CON, LOW);

  // Wait a moment to start
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---CONTROL DEL MOTOR TEST---");
  
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("Conectando...");
  }
  Serial.println("WiFi conectado!");
  digitalWrite(CON, HIGH);

  ThingSpeak.begin(cliente);

  // Start task to print out results (Higher priority)
  xTaskCreatePinnedToCore(GetOx,
                          "Get Oxigen",
                          1500,
                          NULL,
                          1,
                          NULL,
                          0);

  xTaskCreatePinnedToCore(MotorCtrl,
                          "Motor Control",
                          1024,
                          NULL,
                          2,
                          NULL,
                          1);

}

void loop() {
   // Mide el valor del sensor de nivel y modifica el TheState
   val = analogRead(adc_pin);
   if (val <= 1500){
   Serial.println("\t\t\tLa medida es" + String(val));
   vTaskDelay(1000 / portTICK_PERIOD_MS);
   TheState = 1;
   vTaskDelay(1000 / portTICK_PERIOD_MS);
   TheUdstate = 1;
   vTaskDelay(1000 / portTICK_PERIOD_MS);
  }if(val >= 1501){
    Serial.println("\t\t\tLa medida es" + String(val));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TheState = 2;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TheUdstate = 2;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
