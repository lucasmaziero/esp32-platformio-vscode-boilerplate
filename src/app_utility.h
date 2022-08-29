#ifndef app_utility_h
#define app_utility_h

#include <Arduino.h>  // ESP32 Arduino native library
#include <esp_wifi.h> // ESP32 IDF native library

#define MILLIS() (xTaskGetTickCount() * portTICK_PERIOD_MS) // Equivalent to "millis()" function
#define CHIP_ID_MAC_HEX getChipIdMAC()
#define CORE_VERSION getCoreVersion()

/**************************************************************************
  FreeRTOS Elements Check Creation
**************************************************************************/
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

/**************************************************************************
  Timers Software
**************************************************************************/
TimerHandle_t timerCreate(const char *name, TickType_t periodInMillis, TimerCallbackFunction_t callbackFunction)
{
    return xTimerCreate(name, pdMS_TO_TICKS(periodInMillis), pdTRUE, 0, callbackFunction);
}

void timerStart(TimerHandle_t handle, TickType_t periodInMillisToWait = 0)
{
    if ((handle != NULL) && (xTimerGetPeriod(handle)) > 0)
        xTimerStart(handle, pdMS_TO_TICKS(periodInMillisToWait));
}

void timerStop(TimerHandle_t handle, TickType_t periodInMillisToWait = 0)
{
    if (handle != NULL)
        xTimerStop(handle, pdMS_TO_TICKS(periodInMillisToWait));
}

void timerDelete(TimerHandle_t handle, TickType_t periodInMillisToWait = 0)
{
    if (handle != NULL)
        xTimerDelete(handle, pdMS_TO_TICKS(periodInMillisToWait));
}

/**************************************************************************
  ChipID Base on MAC Default
**************************************************************************/
String getChipIdMAC()
{
    char macStr[13] = {0};
    uint8_t id[6] = {0};
    uint64_t chipid = ESP.getEfuseMac();
    id[0] = chipid;
    id[1] = chipid >> 8;
    id[2] = chipid >> 16;
    id[3] = chipid >> 24;
    id[4] = chipid >> 32;
    id[5] = chipid >> 40;
    sprintf(macStr, "%02x%02x%02x%02x%02x%02x", id[0], id[1], id[2], id[3], id[4], id[5]);
    return String(macStr);

    /*
    wifi_mode_t mode;
    uint8_t mac[6];
    char macStr[13] = {0};
    if (esp_wifi_get_mode(&mode) != ESP_OK)
        esp_read_mac(mac, ESP_MAC_WIFI_STA);
    else
        esp_wifi_get_mac((wifi_interface_t)ESP_IF_WIFI_STA, mac);
    sprintf(macStr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
   */
}

/**************************************************************************
  Core version (Arduino | IDF | FreeRTOS)
**************************************************************************/
String getCoreVersion()
{
    // Get Core version (Arduino | IDF | FreeRTOS)
    char coreVersionStr[42] = {0};
    sprintf(coreVersionStr, "FWK(%d.%d.%d)-IDF(%d.%d.%d)-OS(%d.%d.%d)",
            ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH,
            ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH,
            tskKERNEL_VERSION_MAJOR, tskKERNEL_VERSION_MINOR, tskKERNEL_VERSION_BUILD);
    return String(coreVersionStr);
}

#endif /* app_utility_h */