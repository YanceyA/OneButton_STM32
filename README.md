## Note: This library is a current WIP.
- Most OneButton functionality is working. 
- Currently there is no ability for lamda or parameters in the callback functions as provided by OneButton.

# STM32 OneButton Library

This STM32 library improves the usage of a single button for input.
It shows how to use a digital input pin with a single pushbutton attached
for detecting some of the typical button press events like single clicks, double clicks, and long-time pressing.
This enables you to reuse the same button for multiple functions and lowers the hardware investments.

This is also a sample for implementing simple finite-state machines by using the simple pattern above.

This code is a C port of the [OneButton](https://github.com/mathertel/OneButton) C++ Arduino library written by Matthias Hertel.

You can find more details on the OneButton library at his personal [Webpage](http://www.mathertel.de/Arduino/OneButtonLibrary.aspx).

## Basic STM32 Usage Example

```c
OneButton_t button1;  // Declare global instance
uint16_t press_cnt = 0;

OB_Init(&button1); // Init the button with default params
OB_Setup(&button1, BTN_1_GPIO_Port, BTN_1_Pin, true); // Configure the button

void B1_press() { // Basic function to count button presses
    press_cnt++;
}

// Setup callback function
OB_AttachCallback(&button1, OB_EV_PRESS, B1_press)

while (1) {
    OB_Tick(&button1); // Call ticks in main loop
}
```

## Getting Started

Clone this repository into your STM32 project or copy the source files into your project directory. Include the header file in your code:

```c
#include "OneButtonC.h"
```

Each physical button requires its own `OneButton_t` instance.

### Initialize Button Instance

You must first declare a global instance and then initialize it:

```c
OneButton_t button1; // Declare global instance

// In your initialization code:
OB_Init(&button1); // Init with default parameters
OB_Setup(&button1, BTN_GPIO_Port, BTN_Pin, true); // Configure pin and active state
```

The `OB_Setup` function takes the following parameters:
- Button instance pointer
- GPIO Port or Port Alias
- GPIO Pin or Pin Alias
- Active state (true for active LOW, false for active HIGH)

### Attach State Events

Once you have your button initialized, you can handle events by attaching them to the button instance.

```c
void B1_press() { // Basic function to count button presses
    press_cnt++;
}

OB_AttachCallback(&button1, OB_EV_PRESS, B1_press); // Attach the B1_press function to the OB_EV_PRESS event for button1.
```

### Don't forget to `OB_Tick()`

In order for `STM32_OneButton` to work correctly, you must call `OB_Tick()` on __each button instance__ within your main loop. If you're not getting any button events, this is probably why.

```c
while (1) {
    OB_Tick(&button1); // Call ticks in main loop
    // Do other things...
}
```

### HAL Function

Note that the 'OB_Tick()' function uses the `HAL_GPIO_ReadPin( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)` call to get the logic level of the pin and returns RESET(0) or SET(1) for the low and high states respectively. 

`HAL_GetTick()` is also called which requires that the SysTick timer be enabled and configured.

## State Events

Here's a full list of events handled by this library:

| One Button Events         | Description                                                   |
| ----------------------- | ------------------------------------------------------------- |
| `OB_EV_PRESS`        | Fires as soon as a press is detected.                         |
| `OB_EV_CLICK`        | Fires as soon as a single click press and release is detected.|
| `OB_EV_DOUBLE_CLICK`  | Fires as soon as a double click is detected.                  |
| `OB_EV_MULTI_CLICK`   | Fires as soon as multiple clicks have been detected.          |
| `OB_EV_LONG_PRESS_START` | Fires as soon as the button is held down for the defined (ms).|
| `OB_EV_LONG_PRESS_STOP` | Fires periodically as long as the button is held down.        |
| `OB_EV_DURING_LONG_PRESS` | Fires when the button is released after a long hold.          |
| `OB_EV_IDLE` | Fires when the button is determined to be idle.          |

These event ares attached to the callback function using the generic attach function:
`OB_AttachCallback(OneButton_t* btn, OneButtonEvent event, OneButtonCallback cb);`

### Event Timing

Valid events occur when `OB_Tick()` is called after a specified number of (ms). You can use the following functions to change the timing.

__Note:__ Attaching a double click will increase the delay for detecting a single click. If a double click event is not attached, the library will assume a valid single click after one click duration, otherwise it must wait for the double click timeout to pass. This is because a single click callback must not be triggered in case of a double click event.

| Function                | Default    | Description                                                   |
| ----------------------- | ---------- | ------------------------------------------------------------- |
| `OB_SetDebounceMs(int)` | `50 ms`  | Period of time in which to ignore additional level changes.   |
| `OB_SetClickMs(int)`    | `400 ms` | Timeout used to distinguish single clicks from double clicks. |
| `OB_SetPressMs(int)`    | `800 ms` | Duration to hold a button to trigger a long press.            |
| `OB_SetIdleMs(int)`    | `1000 ms` | Duration of inactivity to consider the button idle.            |
| `OB_SetLongPressMs(int)` | `0 ms` | Duration to hold a button to trigger a long press.            |

You may change these default values but be aware that when you specify too short times it is hard to click twice or you will create a long press instead of a click.

Set debounce (ms) to a negative value to only debounce on release. `OB_SetDebounceMs(-25);` will immediately update to a pressed state, and will debounce for 25ms going into the released state. This will expedite the `OB_EV_PRESS` event callback function to run instantly.

Note that long press is not activated by default as it will mask other button functions.

### Additional Functions

`STM32_OneButton` also provides a couple additional functions to use for querying button status:

| Function                | Description                                                                    |
| ----------------------- | ------------------------------------------------------------------------------ |
| `OB_IsLongPressed(const OneButton_t* btn)` | Detect whether or not the button is currently inside a long press.             |
| `OB_GetPressedMs(const OneButton_t* btn)` | Get the current number of (ms) that the button has been held down for. |
| `OB_GetPin(const OneButton_t* btn)` | Get the button pin                                                          |
| `OB_GetState(const OneButton_t* btn)` | Get the button state                                                        |
| `OB_GetDebouncedValue(const OneButton_t* btn)` | Get the debounced button value                                              |

### `OB_Reset()`

If you wish to reset the internal state of your buttons, call `OB_Reset()`.

## Troubleshooting

If your buttons aren't acting the way they should, check these items:

1. Check your wiring and pin numbers.
2. Did you call `OB_Tick()` on each button instance in your loop?
3. Did you alter your clock timers in any way without adjusting ticks?
4. Are the pins or pin aliases defined correctly?
5. Is the active level configured correctly?
```