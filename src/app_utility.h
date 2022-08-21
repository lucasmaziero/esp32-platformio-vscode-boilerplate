#ifndef app_utility_h
#define app_utility_h

#include <Arduino.h>   // ESP32 Arduino native library
#include <WiFi.h>      // ESP32 Arduino native library
#include <Ticker.h>    // ESP32 Arduino native library
#include "app_debug.h" // Debug local library

#ifndef ESP_ARDUINO_VERSION // If version arduino < 2.x
#define ESP_ARDUINO_VERSION_MAJOR 1
#define ESP_ARDUINO_VERSION_MINOR 0
#define ESP_ARDUINO_VERSION_PATCH 6
#endif

// Ticker Object
Ticker reset;

#define FREERTOS_MILLIS() (xTaskGetTickCount() * portTICK_PERIOD_MS) // Equivalent to "millis()" function
#define MAC_CHIP_HEX getChipId()
#define CORE_VERSION getCoreVersion()

// Verify task creation FreeRTOS (Tasks)
bool verifyTaskCreation(BaseType_t taskBaseType_t)
{
    if (taskBaseType_t != pdPASS)
        return false;
    return true;
}

// Verify component creation FreeRTOS (Semaphores | Event Groups | Queues | Timers)
template <typename T>
bool verifyComponentCreation(T componentHandle_t)
{
    if (componentHandle_t == NULL)
        return false;
    return true;
}

TimerHandle_t createTimer(const char *name, TickType_t periodInMillis, TimerCallbackFunction_t callbackFunction)
{
    return xTimerCreate(name, pdMS_TO_TICKS(periodInMillis), pdTRUE, 0, callbackFunction);
}

void startTimer(TimerHandle_t handle, TickType_t periodInMillisToWait = 0)
{
    if ((handle != NULL) && (xTimerGetPeriod(handle)) > 0)
        xTimerStart(handle, pdMS_TO_TICKS(periodInMillisToWait));
}

void stopTimer(TimerHandle_t handle, TickType_t periodInMillisToWait = 0)
{
    if (handle != NULL)
        xTimerStop(handle, pdMS_TO_TICKS(periodInMillisToWait));
}

String getChipId()
{
    String mac = WiFi.macAddress();
    mac.replace(":", ""); // Remove ":"
    mac.toLowerCase();    // Convert to lower case
    return mac;
}

String getCoreVersion()
{
    // Get SDK version
    String sdkVersion = String(ESP_IDF_VERSION_MAJOR) + "." + String(ESP_IDF_VERSION_MINOR) + "." + String(ESP_IDF_VERSION_PATCH);

    // Get Core Release
    String coreVersion = String(ESP_ARDUINO_VERSION_MAJOR) + "." + String(ESP_ARDUINO_VERSION_MINOR) + "." + String(ESP_ARDUINO_VERSION_PATCH);

    // Get FreeRTOS version
    String freeRTOSVersion = String(tskKERNEL_VERSION_MAJOR) + "." + String(tskKERNEL_VERSION_MINOR) + "." + String(tskKERNEL_VERSION_BUILD);

    return "FWK(" + coreVersion + ")-SDK(" + sdkVersion + ")-OS(" + freeRTOSVersion + ")";
}

void reboot(const String msg, uint32_t ms)
{
    DebugPrintln(msg);
    reset.once_ms(ms, esp_restart); // Reset Non-blocking
}

#endif /* app_utility_h */