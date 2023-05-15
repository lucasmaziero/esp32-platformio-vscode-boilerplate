#ifndef app_utility_h
#define app_utility_h

#include <Arduino.h>  // ESP32 Arduino native library
#include <WiFi.h>     // ESP32 Arduino native library
#include <esp_wifi.h> // ESP32 IDF native library
#include <Ticker.h>   // ESP32 Arduino native library

#define TAG_DEBUG_APP_UTILITY_TIMER_RESET "[TIMER RESET]"

// Used check time system is synced
#define TIME_YEARS_DIFFERENCE_CHECK (2020 - 1900) // Is time set? If not, tm_year will be (1970 - 1900).

// Defines time base
#define APP_TIMER_SECOND 1000UL
#define APP_TIMER_MINUTE 60000UL
#define APP_TIMER_HOUR 3600000UL

// Defines most usad
#define MILLIS() (xTaskGetTickCount() * portTICK_PERIOD_MS) // Equivalent to "millis()" function
#define CHIP_ID_STA_MAC_HEX getChipIdMAC(WIFI_IF_STA)
#define CHIP_ID_AP_MAC_HEX getChipIdMAC(WIFI_IF_AP)

#define STA_MAC_HEX getMacAddress()
#define CORE_VERSION getCoreVersion()

// Ticker Object
Ticker _timerRestart;

/**************************************************************************
  ESP init "esp_netif" and "default event loop" used in (WiFi | PPP | MQTT | HTTP ...)
**************************************************************************/
bool espNetifAndEventloopInit()
{
    if (esp_netif_init() != ESP_OK)
        return false;
    if (esp_event_loop_create_default() != ESP_OK)
        return false;
    return true;
}

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
TimerHandle_t timerCreate(const char *name, TickType_t periodInMillis, TimerCallbackFunction_t callbackFunction, UBaseType_t autoReload = pdTRUE)
{
    return xTimerCreate(name, pdMS_TO_TICKS(periodInMillis), autoReload, 0, callbackFunction);
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

void timerChangePeriod(TimerHandle_t handle, TickType_t periodInMillis, TickType_t periodInMillisToWait = 0)
{
    if (handle != NULL)
    {
        if (periodInMillis > 0)
            xTimerChangePeriod(handle, periodInMillis, periodInMillisToWait);
        else
            xTimerStop(handle, pdMS_TO_TICKS(periodInMillisToWait));
    }
}

void timerDelete(TimerHandle_t handle, TickType_t periodInMillisToWait = 0)
{
    if (handle != NULL)
        xTimerDelete(handle, pdMS_TO_TICKS(periodInMillisToWait));
}

/**************************************************************************
  ChipID Base on MAC Default
**************************************************************************/
String getChipIdMAC(wifi_interface_t ifx)
{
    uint8_t mac[6];
    char macStr[18] = {0};
    if ((WiFiGenericClass::getMode() == WIFI_MODE_NULL) || (ifx == WIFI_IF_STA))
        esp_efuse_mac_get_default(mac);
    else
        esp_wifi_get_mac(ifx, mac);

    sprintf(macStr, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}

/**************************************************************************
  Get STA MAC Default
**************************************************************************/
String getMacAddress()
{
    wifi_mode_t mode;
    uint8_t mac[6];
    char macStr[18] = {0};
    if (esp_wifi_get_mode(&mode) != ESP_OK)
        esp_read_mac(mac, ESP_MAC_WIFI_STA);
    else
        esp_wifi_get_mac((wifi_interface_t)ESP_IF_WIFI_STA, mac);
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}

/**************************************************************************
  Core version (Arduino | IDF | FreeRTOS)
**************************************************************************/
String getCoreVersion()
{
    // Get Core version (Arduino | IDF | FreeRTOS)
    char coreVersionStr[40] = {0};
    sprintf(coreVersionStr, "FWK(%d.%d.%d)-IDF(%d.%d.%d)-OS(%d.%d.%d)",
            ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH,
            ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH,
            tskKERNEL_VERSION_MAJOR, tskKERNEL_VERSION_MINOR, tskKERNEL_VERSION_BUILD);
    return String(coreVersionStr);
}

/**************************************************************************
  Convert to format (days hour:minute:second)
**************************************************************************/
void systemRestart(uint32_t ms = 0, const char *msg = NULL)
{
    if (msg != NULL)
        ESP_LOGI(TAG_DEBUG_APP_UTILITY_TIMER_RESET, "%s", msg);
    if (ms > 0)
        _timerRestart.once_ms(ms, esp_restart); // Reset Non-blocking
    else
        esp_restart();
}

#endif /* app_utility_h */