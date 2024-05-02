# Arduino-RTOS

This project is to help deepen the understanding of the RTOS using the Arduino. First let us try to understand what is the differnece between Regular or General Purpise OS vs RTOS (Real Time Operating System). In the case of General Purpose OS like Windoes, macOS etcetera., these Operating systems are designed mainly for user interaction which can be wither CLI (Command Line Interface) or GUI (Graphical User Interface). Another point to note is that since the main focus of these OS is user inetraction, some amount of latency is tolerable here. However, When we talk about the RTOS, they are mainly designed to be run on microcontrollers like Arduino, ESP etcetera. The RTOS has no complex user interface. Additionally, the programs are written in s way that the tasks should meet their deadlines with almost no latency.

Now, we know that we can write the code that needs to be executed on Arudino in Arduino IDE where we declare the variables in the void setup() function and the actually fuunctuionality of the program is written in void loop(). This type of implementation is referred to as Super-Loop where all the tasks happen one after the other. This can be easier to debug and implement.

<img width="472" alt="Screenshot 2024-05-02 at 10 53 09 AM" src="https://github.com/inder-saini/Arduino-RTOS/assets/61368266/e3674552-9168-401a-bc85-aa2c934d3c03">

On the other hand, when we utilize RTOS, we can divide the work of the prgram into different tasks which can run concurrently or serially as per the user's need. So, similar to the Superloop functionality we set the variables in the setup() function but for the tasks we can write them as seperate functions altogether where we do not use the loop() function.

<img width="472" alt="Screenshot 2024-05-02 at 10 53 33 AM" src="https://github.com/inder-saini/Arduino-RTOS/assets/61368266/caab863a-d4ef-48d4-bb12-ba2c1617523f">

 This project was done in two different implementations:
 1.) In first Implementation we use a Global Boolean Variable to keep a check on status of which LED should be lit.
 2.) In Second Implementation we use a Semaphore to synchronise the LED's Lighting (but in this case both the tasks are of same priority).

 Let's talk about the general Idea of the Project. So, The initial idea was to utilize Multiple Tasks or threads, Events,and Interrupt Handling.

 Note: Tasks and Threads are used interchangeably in RTOS(or FreeRTOS).

 So, Blinking RED LED was a regular task that would just go on continously untill the Potentiometer is rotated which enables the Interrupt and causes an Event turning on the BLUE LED while turning off RED LED and then the control goes back to RED LED. Now, achieving the Potentiometer's Rotation Update was the task that I wanted to make efficient because we could have simply used a infinte loop which would keep checking the status of the rotation or we can refer to it as polling. However, I wanted to make it efficient. So, I utilized the concept of Event and something that I learned during the Project, Analog Comparator. Let's Devlve into these concepts:

 1.) Event - 
 As the name is self-explanantory by itself. It helps us determine if an event has occurred. In the FreeRTOS implementation, we actually first initiate a Event Group which in itself has like a particular number of Event Bits. This is very helpful because we do not have to define an Event Group for each Task rather we can set a bit in the Event Group for a particular task. In the context, to our Project we set BIT_0 in the EventGroup meaning when the potentiometer woulbe be rotated, the Interrput would set the BIT_0 to 1 which in turn would run the function corresponding to the BLUE LED and then the BIT_0 goes back to 0 where the RED LED keeps on blinking.
 Let's take a look at an example of the Bits inside of Event Group form the FreeRTOS website:

<img width="614" alt="Screenshot 2024-05-02 at 11 26 44 AM" src="https://github.com/inder-saini/Arduino-RTOS/assets/61368266/a227aa0e-57fb-46d2-9fe5-f5941c299ef5">

2.) Analog Comparator - 
This is the part which helps us determine if the Potentiometer was rotated or not. The name was first confusing in the start as it mentions analog which made me think that this might compare two different analog inputs or an analog and power (3.3V or 5V) of the Arduino Pins. However, I was wrong. The comparison does take place between two pins of the Arduino but they are between the Pins D6 and D7 which correspond to AIN0 and AIN1 as per the ATMEGA328P Pin Assignment. 

 <img width="512" alt="Screenshot 2024-05-02 at 11 31 23 AM" src="https://github.com/inder-saini/Arduino-RTOS/assets/61368266/e314c2c2-5c2d-4ee5-9373-40d89faf9b34">

The image above is the Block Diagram of the Analog Comparator. Here, the Voltage on positive pin AIN0 is compared with the negative pin AIN1. So, when the volatge at Pin AIN0 or D6 is higher than AIN1 or D7 that is when we know that the output is triggered. Since, we utilize one of the libraries that was already available: analogComp.h. An interesting aspect of this library was that once we set the Analog Comparator and when we get the trigger, we can call a function which will only work when this trigger occurs. This function goes by the name of enableInterrupt(). So, in the context of our project, when we rotate the potentiometer we call the enableInterrupt() function which in turn calls aFunction() where we set the BIT_0 of the Event group.

3.) Semaphore - 
Semaphore or Binary Semaphore in particular are used for synchronisation purpose and mutual exclusion. They are similar to Mutex but Binary Semaphores do not have Priority Inheritance as Mutexes have (due to which I presume in the Semaphore implementation both the tasks had to be of same priority). We used Semaphore in this project to synchronize the blinking of LEDs. Now, the thing is in real-time the context switiching happens very fast like in microseconds but it would not be visible to the naked eye. Hence, to make it visible I utilized Semaphore so that we can visually confirm the working of the Project.

Now, Let's Talk about the two implemenations:

1.) In the first implemenation, we have two tasks ehre the TaskAnalogRead() function (which turns on BLUE LED) is of priority 2 and the TaskBlink() (which turns on the RED LED) is of Priority 1. Here, when the RED LED starts blinking once the prgram is loaded to Arduino. Once, the Potentiometer is rotated, it sets a trigger calling enableInterrupt() function and setting the BIT_0 of the Event Group. Along with which, we also have a global boolean variable called 'status' which keeps track of which LED to blink. So, when the BLUE LED turns on due to the interrupt, the RED LED is turned off for that small period of time and then the control goes back to the lower priority task which is TaskBlink() untill the potentiometer is roated again.

2.) In the second implementataion, we use the same methodlogy as above but the differences are that both tasks: TaskBlink() and TaskAnalogRead() are of same priority, we use a Semaphore instead of a Global Boolean variable. Here, as well the RED LED starts blinking once the program is loaded onto Arduino as that is the only task which can take the access to the Semaphore. Once, the Potentiometer is rotated whcih enables the Interrupt and set the BIT_0 of the Event Group, we see the BLUE LED turn on meaning the Semaphore is accessed by TaskAnalogRead() function and then the RED LED starts blinking untill the potentiometer is rotated again.

Resources:
1.) https://www.freertos.org/FreeRTOS-Event-Groups.html
2.) https://www.freertos.org/taskandcr.html
3.) https://www.freertos.org/Embedded-RTOS-Binary-Semaphores.html
4.) https://github.com/leomil72/analogComp
5.) https://www.hackster.io/yeshvanth_muniraj/analog-comparator-module-in-atmega328p-migrating-to-c-7502ea
6.) https://www.digikey.ca/en/maker/projects/what-is-a-realtime-operating-system-rtos/28d8087f53844decafa5000d89608016

