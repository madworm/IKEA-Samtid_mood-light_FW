#define ESP_SSID (F("<your stuff>"))
#define ESP_PASS (F("<your stuff>"))
#define ESP_STATIC_IP (F("192.168.1.199"))
#define ESP_SERVER_PORT (F("80"))
#define ESP_LINE_TERM (F("\r\n"))
#define ESP_SLOW_CIOBAUD 9600UL
#define ESP_FAST_CIOBAUD 115200UL

#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#include "minimal_test.h"

Adafruit_NeoPixel strip(LEDS, LED_CHAIN_PIN, NEO_GRB + NEO_KHZ800);

void setup(void)
{
	strip.begin();
	strip.setPixelColor(0, 32, 32, 32);
	strip.show();
	delay(1000);
	strip.setPixelColor(0, 0, 0, 0);
	strip.show();
	delay(1000);

	pinMode(RESET, OUTPUT);
	digitalWrite(RESET, HIGH);	// RESET is "active-low"

	pinMode(ENABLE, OUTPUT);
	digitalWrite(ENABLE, HIGH);

	delay(2000);		// wait for ESP8266 to boot up

	//increase_ESP8266_baud_rate(); // this may only be needed once or not at all
	//decrease_ESP8266_baud_rate();

	Serial.begin(ESP_SLOW_CIOBAUD);
	while (Serial.available()) {
		uint8_t dummy = Serial.read();	// make sure input butter is empty
	}

	// set mode
	Serial.print(F("AT+CWMODE=3"));
	Serial.print(ESP_LINE_TERM);
	delay(50);

	// join access point
	Serial.print(F("AT+CWJAP=\""));
	Serial.print(ESP_SSID);
	Serial.print(F("\",\""));
	Serial.print(ESP_PASS);
	Serial.print(F("\""));
	Serial.print(ESP_LINE_TERM);
	delay(5000);

	// set static IP address
	Serial.print(F("AT+CIPSTA=\""));
	Serial.print(ESP_STATIC_IP);
	Serial.print(F("\""));
	Serial.print(ESP_LINE_TERM);
	delay(50);

	// now you should be able to PING the board

	// start SERVER
	Serial.print(F("AT+CIPMODE=0"));
	Serial.print(ESP_LINE_TERM);
	delay(50);

	Serial.print(F("AT+CIPMUX=1"));
	Serial.print(ESP_LINE_TERM);
	delay(50);

	Serial.print(F("AT+CIPSERVER=1,"));
	Serial.print(ESP_SERVER_PORT);
	Serial.print(ESP_LINE_TERM);
	delay(50);

	strip.setPixelColor(0, 0, 32, 0);
	strip.show();
}

void loop(void)
{
	static uint8_t state = 0;

	if (Serial.available()) {
		uint8_t dummy = Serial.read();
		if (dummy == ':') {
			switch (state) {
			case 0:
				strip.setPixelColor(1, 64, 0, 0);
				state = 1;
				break;
			case 1:
				strip.setPixelColor(1, 0, 64, 0);
				state = 0;
				break;
			default:
				break;
			}
			strip.show();
		}
	}
}

void increase_ESP8266_baud_rate(void)
{
	Serial.begin(ESP_SLOW_CIOBAUD);	// start with low speed
	Serial.print(F("AT+CIOBAUD="));
	Serial.print(ESP_FAST_CIOBAUD);
	Serial.print(ESP_LINE_TERM);
	delay(50);		// wait a bit so stuff has been sent out
}

void decrease_ESP8266_baud_rate(void)
{
	Serial.begin(ESP_FAST_CIOBAUD);	// start with high speed
	Serial.print(F("AT+CIOBAUD="));
	Serial.print(ESP_SLOW_CIOBAUD);
	Serial.print(ESP_LINE_TERM);
	delay(50);		// wait a bit so stuff has been sent out
}
