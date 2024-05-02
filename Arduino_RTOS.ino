#include <Arduino_FreeRTOS.h>
#include <event_groups.h>
#include <semphr.h>
#include <analogComp.h>

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );

//define function call for setting bits for Event group
void aFunction(EventGroupHandle_t  xEventGroup);

//Declaring a variable to hold the event group which will be created
EventGroupHandle_t xCreatedEventGroup;

//Decalring a variable of type Semaphore (Binary)
//SemaphoreHandle_t xBinarySema;

//Decalring a variable of type boolean
volatile bool status = false;

//define bit for event
#define BIT_0 (1<<0)

// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);


  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
  //We create a event group
  xCreatedEventGroup = xEventGroupCreate();

  //We create the binary semaphore
  //xBinarySema = xSemaphoreCreateBinary();
  
  //If the event group was not created successfully. 
  if(xCreatedEventGroup == NULL){
  Serial.println("The event group did not get created.");
  }
  //if the event group was created succesfully.
  else {
    //aFunction(xCreatedEventGroup);
    analogComparator.setOn(AIN0,AIN1);
    analogComparator.enableInterrupt(aFunction);
    }

    //If the Semaphore was not created successfully.
    /*if(xBinarySema == NULL){
      Serial.println("Failed to create semaphore.");
      } else {
       Serial.println("Semaphore got created"); 
         xSemaphoreGive(xBinarySema);
        }*/

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskBlink
    ,  "Blink"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 1 (configMAX_PRIORITIES - 1) 3 being the highest, and 0 being the lowest.
    ,  NULL );
    
      xTaskCreate(
    TaskAnalogRead
    ,  "AnalogRead"
    ,  128  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(12,OUTPUT);
  for (;;) // A Task shall never return or exit.
  {
    if(status == false){
    digitalWrite(12, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay(pdMS_TO_TICKS(1000) ); // wait for one second
    digitalWrite(12, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay( pdMS_TO_TICKS(100) ); // wait for one second
    }
  }

}

void TaskAnalogRead(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  
  EventBits_t uxBit;
  
  for (;;)
  {
    uxBit = xEventGroupWaitBits(xCreatedEventGroup, BIT_0, pdTRUE, pdFALSE, portMAX_DELAY);
    Serial.println(uxBit);
    //delay(1000);
    //vTaskDelay(50);
    // read the input on analog pin 0:
    //int sensorValue = digitalRead(pin6);
    // print out the value you read:
      if(((uxBit & BIT_0) != 0) && (status == true)){
        //vTaskDelay(50);
       digitalWrite(12,LOW);
       digitalWrite(11,HIGH);
       vTaskDelay(pdMS_TO_TICKS(500));
       digitalWrite(11,LOW);
       status = false;
      }
    
  }
  
}

void aFunction(){
     xEventGroupSetBitsFromISR(xCreatedEventGroup,BIT_0,pdFALSE);
     status = true;
  }
