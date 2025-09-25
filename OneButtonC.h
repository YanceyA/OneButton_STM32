
// -----
// Original code License and information given below. This code is released under
// the same license.:
// OneButton.h - Library for detecting button clicks, doubleclicks and long
// press pattern on a single button. This class is implemented for use with the
// Arduino environment. Copyright (c) by Matthias Hertel,
// http://www.mathertel.de This work is licensed under a BSD style license. See
// http://www.mathertel.de/License.aspx More information on:
// http://www.mathertel.de/Arduino
// -----
// Feb 20 2025 : V0.1 Port from C++ Arduino library to C with most functionality tested.
// -----

#ifndef ONEBUTTONC_H
#define ONEBUTTONC_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h" // Include the HAL library appropriate for your MCU

// Define the invalid pin
#define INVALID_PIN UINT16_MAX

//Define default values for INIT function
#define DEFAULT_DEBOUNCE_MS            50
#define DEFAULT_CLICK_MS               400
#define DEFAULT_PRESS_MS               800
#define DEFAULT_IDLE_MS                1000
#define DEFAULT_LONG_PRESS_INTERVAL_MS 0
#define DEFAULT_MAX_CLICKS_MULTI       100

// Callback type
typedef void (*OneButtonCallback)(void);

// Finite states
typedef enum {
  OCS_INIT,
  OCS_DOWN,
  OCS_UP,
  OCS_COUNT,
  OCS_PRESS,
  OCS_PRESSEND
} OneButtonState;

// Main struct for OneButton
typedef struct {
  GPIO_TypeDef* port;
  uint16_t pin;
 
  //Intervals
  uint16_t debounce_ms;
  uint16_t click_ms;
  uint16_t press_ms;
  uint16_t idle_ms;
  
  // State machine
  OneButtonState state;
  bool idleState;
  bool debounceEnabled;
  bool inTick; // Reentrancy guard

  //Levels
  uint8_t buttonPressedLevel;
  bool debouncedLevel;
  bool lastDebounceLevel;

  //Timers
  uint32_t lastDebounceTime;
  uint32_t startTime;
  uint32_t now;

  // Click counter
  uint16_t nClicks;
  uint16_t maxClicks;

  // Long press interval
  uint16_t longPressIntervalMs;
  uint32_t lastDuringLongPressTime;

  // Event callbacks
  OneButtonCallback pressFunc;
  OneButtonCallback clickFunc;
  OneButtonCallback doubleClickFunc;
  OneButtonCallback multiClickFunc;
  OneButtonCallback longPressStartFunc;
  OneButtonCallback longPressStopFunc;
  OneButtonCallback duringLongPressFunc;
  OneButtonCallback idleFunc;

  //Button enabled flag
  bool enabled;

} OneButton_t;

typedef enum {
  OB_EV_PRESS,
  OB_EV_CLICK,
  OB_EV_DOUBLE_CLICK,
  OB_EV_MULTI_CLICK,
  OB_EV_LONG_PRESS_START,
  OB_EV_LONG_PRESS_STOP,
  OB_EV_DURING_LONG_PRESS,
  OB_EV_IDLE
} OneButtonEvent;

// ----- Function prototypes -----

void OB_Init(OneButton_t* btn);
void OB_Setup(OneButton_t* btn, GPIO_TypeDef* port, uint16_t pin, bool activeLow);

bool OB_Debounce(OneButton_t* btn, bool newLevel);
void OB_Tick(OneButton_t* btn /*, bool rawLevel*/);
void OB_Reset(OneButton_t* btn);
uint16_t OB_GetNumberClicks(const OneButton_t* btn);
bool OB_IsIdle(const OneButton_t* btn);
bool OB_IsLongPressed(const OneButton_t* btn);

// Setter functions for intervals
void OB_SetDebounceMs(OneButton_t* btn, uint16_t ms);
void OB_SetDebounceEnabled(OneButton_t* btn, bool enabled);
void OB_SetClickMs(OneButton_t* btn, uint16_t ms);
void OB_SetPressMs(OneButton_t* btn, uint16_t ms);
void OB_SetIdleMs(OneButton_t* btn, uint16_t ms);
void OB_SetLongPressIntervalMs(OneButton_t* btn, uint16_t ms);
void OB_SetMaxClicks(OneButton_t* btn, uint16_t maxClicks);

// Replace all individual attach function prototypes with this single function
bool OB_AttachCallback(OneButton_t* btn, OneButtonEvent event, OneButtonCallback cb);

// Getter functions
uint32_t OB_GetPressedMs(const OneButton_t* btn);
uint16_t OB_GetPin(const OneButton_t* btn);
OneButtonState OB_GetState(const OneButton_t* btn);
bool OB_GetDebouncedValue(const OneButton_t* btn);

// Internal state machine functions
void OB_NewState(OneButton_t* btn, OneButtonState nextState);
void OB_FSM(OneButton_t* btn, bool activeLevel);

// Enable/Disable button
void OB_Enable(OneButton_t *btn);
void OB_Disable(OneButton_t *btn);

#endif // ONEBUTTONC_H