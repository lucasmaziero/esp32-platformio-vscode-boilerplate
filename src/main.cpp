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
  Version: 0.0
  Date: 19/08/2022
  Modified: 15/05/2023
**************************************************************************
 Copyright(c) by: Fox IoT.
**************************************************************************/

/**************************************************************************
  SET FEATURES APP
**************************************************************************/
// Debugging FreeHeap
//#define USE_DEBUG_TASK_FREEHEAP

// Device name
#define DEVICE_NAME "ESP32-IDF-ARDUINO-BRLINK"

// Firmware version
#define FIRMWARE_VERSION "0.0"

// Set tag for debug
#define TAG_DEBUG_DEVICE "[DEVICE]"

/**************************************************************************
  AUXILIARY LIBRARIES
**************************************************************************/
#include <sdkconfig.h> // ESP32 IDF native library
#include <Arduino.h>   // ESP32 Arduino native library
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
#define TASK_STACK_SIZE_LED_BLINK (3 * configMINIMAL_STACK_SIZE)

// Tasks Priority (use ever priority >= 2)
#define TASK_PRIORITY_LED_BLINK (2 + tskIDLE_PRIORITY)

// Tasks Handle
TaskHandle_t pvTaskHandleLedBlink = NULL;

#if defined(USE_DEBUG_TASK_FREEHEAP) && !defined(CONFIG_LOG_DEFAULT_LEVEL_NONE)
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
#if defined(USE_DEBUG_TASK_FREEHEAP) && !defined(CONFIG_LOG_DEFAULT_LEVEL_NONE)
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

  // Task BaseType
  BaseType_t xReturned = pdFAIL;

  // Print model and version
  ESP_LOGI(TAG_DEBUG_DEVICE, "%s v%s,", DEVICE_NAME, FIRMWARE_VERSION);
  // Print core version
  ESP_LOGI(TAG_DEBUG_DEVICE, "Core Version: %s", CORE_VERSION.c_str());
  // Print core version
  ESP_LOGI(TAG_DEBUG_DEVICE, "Chip ID: %s", CHIP_ID_STA_MAC_HEX.c_str());

    // Init "esp_netif" and "default event loop"
  if (espNetifAndEventloopInit())
    ESP_LOGI(TAG_DEBUG_DEVICE, "Init Netif and EventLoop Ok!");
  else
  {
    ESP_LOGE(TAG_DEBUG_DEVICE, "Init Netif and EventLoop Fail!");
    systemRestart();
  }

  // Task LED status
  xReturned = xTaskCreatePinnedToCore(
      pvTaskLedBlink,
      "LedBlink",                // Task name (configMAX_TASK_NAME_LEN is 16)
      TASK_STACK_SIZE_LED_BLINK, // Stack size (size * configMINIMAL_STACK_SIZE) (ESP32 bytes)
      NULL,                      // Parameters (NULL | &parameters)
      TASK_PRIORITY_LED_BLINK,   // Task priority (configUSE_TIME_SLICING + tskIDLE_PRIORITY)
      &pvTaskHandleLedBlink,     // Task handle (NULL | &handle)
      tskNO_AFFINITY             // CPU id (PRO_CPU_NUM | APP_CPU_NUM | tskNO_AFFINITY)
  );
  if (!verifyTaskCreation(xReturned))
    ESP.restart();

#if defined(USE_DEBUG_TASK_FREEHEAP) && !defined(CONFIG_LOG_DEFAULT_LEVEL_NONE)
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
    ESP_LOGI(TAG_DEBUG_DEVICE, "LED: on");
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(PIN_LED_BLINK, 0);
    ESP_LOGI(TAG_DEBUG_DEVICE, "LED: off");
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  vTaskDelete(NULL); // Execution should never get here
}

/****************************************************************************************************************************************************
  FUNCTIONS FREE HEAP
****************************************************************************************************************************************************/
#if defined(USE_DEBUG_TASK_FREEHEAP) && !defined(CONFIG_LOG_DEFAULT_LEVEL_NONE)
void pvTaskFreeHeap(void *pvParameters)
{
  // Setup

  // Loop
  while (true)
  {
    // Inspect our own high water mark of task
    ESP_LOGI("[FREEHEAP]", "---");
    stackHighWaterMarkPrint(pvTaskHandleLedBlink, TASK_STACK_SIZE_LED_BLINK);
    ESP_LOGI("[FREEHEAP]", "---");

    // Inspect freeHeap ESP32
    ESP_LOGI("[FREEHEAP]", "---");
    freeHeapPrint();
    ESP_LOGI("[FREEHEAP]", "---");

    vTaskDelay(pdMS_TO_TICKS(30000)); // Avoid ESP32 panics, allow CPU :)
  }
  vTaskDelete(NULL); // Execution should never get here
}

void stackHighWaterMarkPrint(TaskHandle_t xTask, size_t stackSize)
{
  if (xTask != NULL) // Check if task was created
  {
    UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(xTask);
    float stackSizePercent = (((float)highWaterMark / (float)stackSize) * 100.0);
    ESP_LOGI("[FREEHEAP]", "\"%s\" High Water Mark: %d Bytes | %.2f Percent", pcTaskGetTaskName(xTask), highWaterMark, stackSizePercent);
  }
}

void freeHeapPrint()
{
  ESP_LOGI("[FREEHEAP]", "HeapSize: %d Bytes | 100.00 Percent", ESP.getHeapSize());
  ESP_LOGI("[FREEHEAP]", "FreeHeap: %d Bytes | %.2f Percent", ESP.getFreeHeap(), ((float)ESP.getFreeHeap() / (float)ESP.getHeapSize()) * 100.0);
}
#endif
