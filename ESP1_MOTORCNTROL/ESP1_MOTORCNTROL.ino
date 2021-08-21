// Codigo para dividir las tareas de sensado de nivel y
// control del motor, así mismo enviar una señal por digitalWrite

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
static byte TheState = 0;

// TASKS

// Print level sensor value to give a condition
void printValues(void *parameters) {

  // Loop forever, wait for semaphore, and print value
  while (1) {
   val = analogRead(adc_pin);
   if (val <= 100){
   Serial.println("La medida es \t" + String(val));
   digitalWrite(CON, HIGH); //SOLO PARA PROBAR
   vTaskDelay(2000 / portTICK_PERIOD_MS);
   TheState = 1;
  }if (val >= 101){
    Serial.println("La medida es \t" + String(val));
    digitalWrite(CON, LOW); // RECUERDA QUITARLO DESPUES PARA INTEGRAR EL RESTO DEL CODIGO
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TheState = 2;
  }

       //Print out remaining stack memory (words)
    Serial.print("High water mark (words): ");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));

    //Print out number of free heap memory bytes before malloc
    Serial.print("Heap before malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());
    int *ptr = (int*)pvPortMalloc(1024 * sizeof(int));

    //One way to prevent heap overflow is to check the malloc output
    if (*ptr == NULL){
      Serial.println("Not enough heap.");
    }else{

    // Do something with the memory so it's not optimized out by the compiler
    for (int i = 0; i < 1024; i++){
      ptr[i] = 3;
     }
    }

    //Print out number of free heap memory bytes after malloc
    Serial.print("Heap after malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());

    //Free up our allocated memory
    vPortFree(ptr);

    //Wait for a while
    vTaskDelay(2000 / portTICK_PERIOD_MS);
 }
}

// Control de motor
void MotorCtrl(void *parameters){
  while(1){
    switch(TheState){

    case 0:
    for(int w = 0; w<5; w++){
      
    Serial.println("\tIniciando motor...");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    Serial.println("\tEl motor está listo");
    break;
    
    case 1:
    //Serial.println("La medida es \t" + String(val));
    //digitalWrite(LED1, HIGH);
    //delay(3000);
    //digitalWrite(LED1, LOW);
    for (int motorValue = 0; motorValue <= 60; motorValue += 60){
        ledcWrite(canalPWM, motorValue);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }

    for (int motorValue = 60; motorValue >= 0; motorValue -= 60){
        ledcWrite(canalPWM, motorValue);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    
    Serial.println("\tEl sensor está dentro!");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  break;

  case 2:
       for(int r = 0; r<300; r++){

      Serial.println("\tEl sensor está tomando las medidas");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    Serial.println("\tEl sensor está listo para subir!");
    TheState = 3;
  break;
  case 3:
  //Serial.println("La medida es \t" + String(val));
    //digitalWrite(LED2, HIGH);
    //delay(3000);
    //digitalWrite(LED2, LOW);
    for (int motorValue = 0; motorValue <= 100; motorValue += 50){
        ledcWrite(canalPWM1, motorValue);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }

    for (int motorValue = 100; motorValue >=0; motorValue -= 50){
        ledcWrite(canalPWM1, motorValue);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    for(int y = 0; y<10; y++){

      Serial.println("\tEl sensor está saliendo");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    Serial.println("\tEl sensor está afuera!");
    TheState = 4;
  break;
    
  case 4:
    for(int z = 0; z<900; z++){

      Serial.println("\tEl sensor está en pausa");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    Serial.println("\tEl sensor está listo para moverse!");
  break;
  }
       //Print out remaining stack memory (words)
    Serial.print("\tHigh water mark (words): ");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));

    //Print out number of free heap memory bytes before malloc
    Serial.print("\tHeap before malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());
    int *ptr = (int*)pvPortMalloc(1024 * sizeof(int));

    //One way to prevent heap overflow is to check the malloc output
    if (*ptr == NULL){
      Serial.println("\tNot enough heap.");
    }else{

    // Do something with the memory so it's not optimized out by the compiler
    for (int i = 0; i < 1024; i++){
      ptr[i] = 3;
     }
    }

    //Print out number of free heap memory bytes after malloc
    Serial.print("\tHeap after malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());

    //Free up our allocated memory
    vPortFree(ptr);

    //Wait for a while
    vTaskDelay(2000 / portTICK_PERIOD_MS);
 }
 
}

void setup() {
  // Initialize Serial
  Serial.begin(9600);

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

  // Start task to print out results (Higher priority)
  xTaskCreatePinnedToCore(printValues,
                          "Print Values",
                          1024,
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

  // Delete Loop and Setup tasks
  vTaskDelete(NULL);

}

void loop() {
  // put your main code here, to run repeatedly:

}
