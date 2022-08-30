/**************************************************************************
  PROJETO: ESP32 IDF ARDUINO BLINK
  FEATURES: --
  MCU: ESP32-WROOM-32E 4MB
  OS: FREERTOS
  PROTOCOLO: --
***************************************************************************
  AUTHOR: LUCAS MAZIERO - Electrical Engineer
  EMAIL: lucas.mazie.ro@hotmail.com or lucasmaziero@foxiot.com.br
  CITY: Santa Maria - Rio Grande do Sul - Brasil
  FOUNDATION: Fox IoT
**************************************************************************
  Version: 1.0
  Date: 19/08/2022
  Modified: XX/XX/20XX
**************************************************************************
 Copyright(c) by: Fox IoT.
**************************************************************************/

/**************************************************************************
  SET FEATURES APP
**************************************************************************/
// Debugging
#define USE_DEBUG_UART

// Debugging HeapFree
// #define USE_DEBUG_HEAP_FREE

// Device model
#define DEVICE_MODEL "ESP32-IDF-ARDUINO-BRLINK"

// Firmware version
#define FIRMWARE_VERSION "1.0"

/**************************************************************************
  AUXILIARY LIBRARIES
**************************************************************************/
#include <sdkconfig.h> // ESP32 IDF native library
#include <Arduino.h>   // ESP32 Arduino native library
#include "app_debug.h"
#include "app_utility.h"

/**************************************************************************
  VARIABLES, DEFINITIONS AND STRUCTS
**************************************************************************/
// Pin mapping LED
#define PIN_LED_BLINK GPIO_NUM_2

/**************************************************************************
  FREERTOS ELEMENTS
**************************************************************************/
// Tasks Stack Size
#define TASK_STACK_SIZE_LED_BLINK (2 * configMINIMAL_STACK_SIZE)

// Tasks Priority (use ever priority >= 2)
#define TASK_PRIORITY_LED_BLINK (2 + tskIDLE_PRIORITY)

// Task BaseType
BaseType_t xReturned;

// Tasks Handle
TaskHandle_t pvTaskHandleLedBlink = NULL;

#if defined(USE_DEBUG_HEAP_FREE) && defined(USE_DEBUG_UART)
TaskHandle_t pvTaskHandleFreeHeap = NULL;
#endif

/**************************************************************************
  INSTANCE OF OBJECTS
**************************************************************************/

/**************************************************************************
  PROTOTYPE OF FUNCTIONS
**************************************************************************/
// Tasks
void pvTaskLedBlink(void *pvParameters);

// Free Heap Statistic
#if defined(USE_DEBUG_HEAP_FREE) && defined(USE_DEBUG_UART)
void pvTaskFreeHeap(void *pvParameters);
void stackHighWaterMarkPrint(TaskHandle_t xTask, size_t stackSize);
void freeHeapPrint();
#endif

/****************************************************************************************************************************************************
  FUNCTION IDF APP MAIN
****************************************************************************************************************************************************/
extern "C" void app_main()
{
  // Initialize arduino library before we start the tasks
  initArduino();

  // Init UART DEBUG
  DebugBegin(115200);
  DebugPrintln();

// Wait for open Serial Monitor
#if defined(USE_DEBUG_UART)
  vTaskDelay(pdMS_TO_TICKS(2000));
#endif

  // Print model and version
  DebugPrintln(DEVICE_MODEL " v" FIRMWARE_VERSION);

  // Print core version
  DebugPrintln("Core Version: " + CORE_VERSION);

  // Print core version
  DebugPrintln("Chip ID: " + CHIP_ID_MAC_HEX);

  // Task LED status
  xReturned = xTaskCreatePinnedToCore(
      pvTaskLedBlink,
      "Blink",                      // Task name (configMAX_TASK_NAME_LEN is 16)
      TASK_STACK_SIZE_LED_BLINK, // Stack size (size * configMINIMAL_STACK_SIZE) (ESP32 bytes)
      NULL,                       // Parameters (NULL | &parameters)
      TASK_PRIORITY_LED_BLINK,    // Task priority (configUSE_TIME_SLICING + tskIDLE_PRIORITY)
      &pvTaskHandleLedBlink,      // Task handle (NULL | &handle)
      tskNO_AFFINITY              // CPU id (PRO_CPU_NUM | APP_CPU_NUM | tskNO_AFFINITY)
  );
  if (!verifyTaskCreation(xReturned))
    ESP.restart();

#if defined(USE_DEBUG_HEAP_FREE) && defined(USE_DEBUG_UART)
  xReturned = xTaskCreatePinnedToCore(
      pvTaskFreeHeap,
      "FreeHeap",                     // Task name (configMAX_TASK_NAME_LEN is 16)
      (3 * configMINIMAL_STACK_SIZE), // Stack size (size * configMINIMAL_STACK_SIZE) (ESP32 bytes)
      NULL,                           // Parameters (NULL | &parameters)
      (2 + tskIDLE_PRIORITY),         // Task priority (configUSE_TIME_SLICING + tskIDLE_PRIORITY)
      &pvTaskHandleFreeHeap,          // Task handle (NULL | &handle)
      tskNO_AFFINITY                  // CPU id (PRO_CPU_NUM | APP_CPU_NUM | tskNO_AFFINITY)
  );
  if (!verifyTaskCreation(xReturned))
    ESP.restart();
#endif
}

/****************************************************************************************************************************************************
  FUNCTIONS FREERTOS TASKS
****************************************************************************************************************************************************/
void pvTaskLedBlink(void *pvParameters)
{
  // Setup
  // Set pin LED status
  gpio_pad_select_gpio(PIN_LED_BLINK);
  gpio_set_direction(PIN_LED_BLINK, GPIO_MODE_OUTPUT);
  gpio_set_level(PIN_LED_BLINK, 0);

  // Loop
  while (true)
  {
    gpio_set_level(PIN_LED_BLINK, 1);
    DebugPrintln("LED: on");
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(PIN_LED_BLINK, 0);
    DebugPrintln("LED: off");
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  vTaskDelete(NULL); // Execution should never get here
}

/****************************************************************************************************************************************************
  FUNCTIONS FREE HEAP
****************************************************************************************************************************************************/
#if defined(USE_DEBUG_HEAP_FREE) && defined(USE_DEBUG_UART)
void pvTaskFreeHeap(void *pvParameters)
{
  // Setup

  // Loop
  while (true)
  {
    // Inspect our own high water mark of task
    DebugPrintln(F("---"));
    stackHighWaterMarkPrint(pvTaskHandleLedBlink, TASK_STACK_SIZE_LED_BLINK);
    DebugPrintln(F("---"));

    // Inspect freeHeap ESP32
    freeHeapPrint();
    vTaskDelay(pdMS_TO_TICKS(60000));

    // vTaskDelay(pdMS_TO_TICKS(1)); // Avoid ESP32 panics, allow CPU :)
  }
  vTaskDelete(NULL); // Execution should never get here
}

void stackHighWaterMarkPrint(TaskHandle_t xTask, size_t stackSize)
{
  if (xTask != NULL) // Check if task was created
  {
    UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(xTask);
    float stackSizePercent = (((float)highWaterMark / (float)stackSize) * 100.0);

    DebugPrint(pcTaskGetTaskName(xTask));
    DebugPrint(F(" high water mark: "));
    DebugPrint(highWaterMark);
    DebugPrint(F(" Bytes | "));
    DebugPrint(stackSizePercent, 2);
    DebugPrint(F(" %"));
    DebugPrintln();
    // DebugPrint(" | Core: ");
    // DebugPrintln(xPortGetCoreID());
  }
}

void freeHeapPrint()
{
  DebugPrintln(F("---"));
  DebugPrint(F("HeapSize: "));
  DebugPrint(ESP.getHeapSize());
  DebugPrintln(F(" Bytes | 100.00 %"));
  DebugPrint(F("FreeHeap: "));
  DebugPrint(ESP.getFreeHeap());
  DebugPrint(F(" Bytes | "));
  DebugPrint(((float)ESP.getFreeHeap() / (float)ESP.getHeapSize()) * 100.0, 2);
  DebugPrintln(F(" %"));
}
#endif
