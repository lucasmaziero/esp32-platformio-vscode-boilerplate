#ifndef app_debug_h
#define app_debug_h

#include <Arduino.h> // ESP32 Arduino native library

#if defined(USE_DEBUG_UART)

#define DEBUG_UART Serial // UART_NUM_0 -> Serial
#define DebugBegin(...)                      \
  do                                         \
  {                                          \
    DEBUG_UART.begin(__VA_ARGS__);           \
  } while (0)

#define DebugPrint(...)            \
  do                               \
  {                                \
    DEBUG_UART.print(__VA_ARGS__); \
  } while (0)

#define DebugPrintln(...)            \
  do                                 \
  {                                  \
    DEBUG_UART.println(__VA_ARGS__); \
  } while (0)

#define DebugPrintf(...)            \
  do                                \
  {                                 \
    DEBUG_UART.printf(__VA_ARGS__); \
  } while (0)

#define DebugPrintHEX(data, length)       \
  do                                      \
  {                                       \
    uint8_t *dataPointer = (data);        \
    for (uint16_t i = 0; i < length; i++) \
    {                                     \
      if (dataPointer[i] < 0x10)          \
        DebugPrint(F("0"));               \
      DebugPrint(dataPointer[i], HEX);    \
    }                                     \
  } while (0)
#else
#define DebugBegin(...) esp_log_level_set("*", ESP_LOG_NONE)
#define DebugPrint(...)
#define DebugPrintln(...)
#define DebugPrintf(...)
#define DebugPrintHEX(data, length)
#endif

#endif /* app_debug_h */