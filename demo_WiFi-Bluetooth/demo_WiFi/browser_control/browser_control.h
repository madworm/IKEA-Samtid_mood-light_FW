#ifndef minimal_test_h
#define minimal_test_h

#define E_BUTTON 11
#define M_BUTTON 12
#define LED_CHAIN_PIN 5		// WS2812B LEDs
#define LEDS 64
#define STATUS_LED 13

#define KEY 19			// for Bluetooth HC-05 module
#define RESET 19		// for ESP8266-1 module + adapter

#define STATE 6			// for Bluetooth HC-05 module
#define ENABLE 6		// for ESP8266-1 module + adapter

#endif

typedef struct REQ_VAR {
    String name;
    unsigned int value;
    uint8_t offset;    
} REQ_VAR;
