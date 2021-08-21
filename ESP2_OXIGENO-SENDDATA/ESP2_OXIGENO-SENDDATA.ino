// Código destinado al sensado del oxígeno en base a la señal digital
// del sensor de nivel, así como para el envío de datos vía HTTP

// Comunicacion librerias
#include "ThingSpeak.h"
#include "WiFi.h"

// OX
#include <SoftwareSerial.h>
#define rx 33                                          //define what pin rx is going to be yellow
#define tx 32                                          //define what pin tx is going to be orange
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

// ud
static const int dc_pin = 26;
int val = 0;
static byte TheState = 1;
int ud = 0;

// Comunicacion definiciones

// WiFi Credentials
const char* ssid = "Monitor";
const char* password = "12345678";

// ThingSpeak credentials
unsigned long channelID = 1229566;
const char* WriteAPIKey = "O07LIAVZQU2APO95";

WiFiClient cliente;


// Prepare OD EMBEDDED
void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}

// Task GETOX
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


void setup() {
  // Initialize Serial and settings
  Serial.begin(9600);
  myserial.begin(9600);                               //set baud rate for the software serial port to 9600
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
  pinMode(dc_pin, INPUT);
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

  // TASKS
 // Task Send data low priority
 xTaskCreatePinnedToCore(GetOx,
                         "Send Data",
                         2048,
                         NULL,
                         1,
                         NULL,
                         1);




}

void loop(){
   val = digitalRead(dc_pin);
   if(val == 0){
     TheState = 1;
     Serial.println("\t\t\tEl valor es: " + String(val));
     Serial.println("\t\t\tEl sensor está dentro!");
      vTaskDelay(2000 / portTICK_PERIOD_MS);
   }if(val == 1){
    TheState = 2;
    Serial.println("\t\t\tEl valor es: " + String(val));
    Serial.println("\t\t\tEl sensor está afuera!");
     vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
 }
