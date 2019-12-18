#ifndef PSN_CONFIG_H
#define PSN_CONFIG_H

// ============================================================================
//                                 Settings
// ============================================================================
// Comment option to disable it.
#define ENABLE_BREAK_SENSOR
#define ENABLE_CRUISE
#define ENABLE_PEDAL_SENSOR 

#define PRINT_INTERVAL 500
#define VCC_MEASURE_INTERVAL 10 * 1000
#define CRUISE_MAX_JITTER 100
#define CRUISE_IS_HOLD_BUTTON true 
#define SERIAL_PRINT true
#define PEDAL_SCALE 0.8

// ============================================================================
//                                 Sensors
// ============================================================================
// Throttle needs an analog pin.
#define PIN_THROTTLE_ADC A2
// #define THROTTLE_MIN 172
// #define THROTTLE_MAX 820
#define THROTTLE_MIN 250
#define THROTTLE_MAX 885
// Padal sensor output a rising edge every 30 degree. It needs a digital pin.
#define PIN_PEDAL 2 // Pin2, INT0
// Break sensor is a hall sensor attached to a break crank. It need a digital pin.
#define PIN_BUTTON_BREAK 12
// When pressing the cruise button, the bike will go into cruising state.
#define PIN_BUTTON_CRUISE 11
#define PIN_BATTERY_VOLTAGE_ADC A1
#define PIN_THRTTLE_OUTPUT_PWM 3
#define PIN_LED 13

// ============================================================================
//                                 OLED SSD1306
// ============================================================================
// #define PIN_SSD1306_RESET  4
// #define PIN_SSD1306_GND    4
// #define PIN_SSD1306_VCC    3
#define PIN_SSD1306_RESET  -1
#define PIN_SSD1306_GND    -1
#define PIN_SSD1306_VCC    -1

// ============================================================================
//                                Software Serial
// ============================================================================
// #define PIN_SOFTWARE_SERIAL_RX 10
// #define PIN_SOFTWARE_SERIAL_TX 11
#define PIN_SOFTWARE_SERIAL_RX -1
#define PIN_SOFTWARE_SERIAL_TX -1

#endif
