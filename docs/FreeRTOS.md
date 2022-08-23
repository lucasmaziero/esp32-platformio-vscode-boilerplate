# [FreeRTOS](https://github.com/platformio/platform-espressif32) — API Reference
FreeRTOS™ Real-time operating system for microcontrollers
#
## `Tasks` 
#

- Task Defines
```c++
// Tasks Stack Size
#define TASK_STACK_SIZE_XXX (2 * configMINIMAL_STACK_SIZE)

// Tasks Priority (use ever priority >= 2)
#define TASK_PRIORITY_XXX (2 + tskIDLE_PRIORITY)
```

- Task Handle
```c++
// Task BaseType
BaseType_t xReturned;

// Tasks Handle
TaskHandle_t pvTaskHandleXXX = NULL;
```
- Task Funtion Prototype
```c++
// Tasks
void pvTaskXXX(void *pvParameters);
```
- Task Create
```c++
xReturned = xTaskCreatePinnedToCore(
    pvTaskXXX,
    "XXX",               // Task name (configMAX_TASK_NAME_LEN is 16)
    TASK_STACK_SIZE_XXX, // Stack size (size * configMINIMAL_STACK_SIZE) (ESP32 bytes)
    NULL,                // Parameters (NULL | &parameters)
    TASK_PRIORITY_XXX,   // Task priority (configUSE_TIME_SLICING + tskIDLE_PRIORITY)
    &pvTaskHandleXXX,    // Task handle (NULL | &handle)
    tskNO_AFFINITY       // CPU id (PRO_CPU_NUM | APP_CPU_NUM | tskNO_AFFINITY)
    );
    if (!verifyTaskCreation(xReturned))
        ESP.restart();
```
- Task Funtion
```c++
void pvTaskXXX(void *pvParameters)
{
    // Setup
    // Receiver parameters (Copy parameters mode safe)
    // XXX_t XXX;
    // memcpy(&XXX, pvParameters, sizeof(XXX));

    // Loop
    while (true)
    {
      // Task code goes here.

      vTaskDelay(pdMS_TO_TICKS(10)); // Avoid ESP32 panics, allow CPU :)
    }
    vTaskDelete(NULL); // Execution should never get here
}
```

# 
## `Event Group` 
#
- Event Group Defines
```c++
// Event group BITS
#define NETWORK_CONNECTED_BIT BIT0
#define MQTT_CONNECTED_BIT BIT1
```
- Event Group Handle
```c++
// Event Group Handle
EventGroupHandle_t xEventGroup = NULL;
```
- Event Group Create
```c++
// Event group
xEventGroup = xEventGroupCreate();
if (!verifyComponentCreation(xEventGroup))
  ESP.restart();
```
- Event Group SetBits
```c++
xEventGroupSetBits(xEventGroup, NETWORK_CONNECTED_BIT);
```
- Event Group ClearBits
```c++
xEventGroupClearBits(xEventGroup, NETWORK_CONNECTED_BIT);
```
- Event Group WaitBits
```c++
// Wainting network and mqtt connection
xEventGroupWaitBits(xEventGroup, (NETWORK_CONNECTED_BIT | MQTT_CONNECTED_BIT), pdFALSE, pdTRUE, portMAX_DELAY);
```

# 
## `Queues` 
#
- Queue Struct
```c++
// Set struct of queue
typedef struct
{
    uint8_t var1;
    uint16_t var2;
    uint32_t var3;
} XXX_t;
```
- Queue Handle
```c++
// Queues Handle
QueueHandle_t xQueueXXX = NULL;
```
- Queue Create
```c++
// Queues
xQueueXXX = xQueueCreate(5, sizeof(XXX_t));
if (!verifyComponentCreation(xQueueXXX))
  ESP.restart();
```
- Queue Send
```c++
// Define struct
XXX_t xxx;

xxx.var1 = 10;
xxx.var1 = 100;
xxx.var1 = 100;

// Send data for buffer
xQueueSend(xQueueXXX, &xxx, pdMS_TO_TICKS(0));
```
- Queue Overwrite
```c++
// Define struct
XXX_t xxx;

// Set variables
xxx.var1 = 10;
xxx.var1 = 100;
xxx.var1 = 100;

// Overwrite last data
xQueueOverwrite(xQueueXXX, &xxx);
```
- Queue Receiver
```c++
// Define struct
XXX_t xxx;

// Receiver data and remove of buffer
if(xQueueReceive(xQueueXXX, &xxx, portMAX_DELAY))
{
  // Queue code goes here.  
}
```
- Queue Peek
```c++
// Define struct
XXX_t xxx;

// Receiver data not remove of buffer
if(xQueuePeek(xQueueXXX, &xxx, pdMS_TO_TICKS(0)))
{
  // Queue code goes here.
}
```

#
## `Timers` 
#
- Timers Handle
```c++
TimerHandle_t xTimerXXX = NULL;
```
- Timers Funtion Prototype
```c++
void onTimerXXX(TimerHandle_t pxTimer);
```
- Timers Create
```c++
xTimerXXX = xTimerCreate("TimerXXX", pdMS_TO_TICKS(5 * 1000UL /*SECOND*/), pdTRUE, 0, onTimerXXX);
if (!verifyComponentCreation(xTimerXXX))
  ESP.restart();
```
- Timers Funtion
```c++
void onTimerXXX(TimerHandle_t pxTimer)
{
  // Timer code goes here.
}
```

#
## `Semaphore` 
#
- Semaphore Handle
```c++
SemaphoreHandle_t xMutexXXX = NULL;
```
- Semaphore Create
```c++
xMutexXXX = xSemaphoreCreateMutex();
if (!verifyComponentCreation(xMutexXXX))
  ESP.restart();
```
- Semaphore Take | Give
```c++
// If the semaphore is not available wait portMAX_DELAY
if(xSemaphoreTake(xMutexXXX, portMAX_DELAY))
{
  // Semaphore code goes here.

  // Release the semaphore.
  xSemaphoreGive(xMutexXXX);
}
```