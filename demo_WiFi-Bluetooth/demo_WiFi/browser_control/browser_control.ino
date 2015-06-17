//
// This code will:
// 
// * join an access point (settings below)
// * start a server using a static IP + PORT
// 
// Use a webbrowser to connect to it:
//       http://192.168.1.199:80
// 

#define ESP_SSID (F("---"))
#define ESP_PASS (F("---"))
#define ESP_STATIC_IP (F("192.168.1.199"))
#define ESP_SERVER_PORT (F("80"))
#define ESP_LINE_TERM (F("\r\n"))
#define ESP_SLOW_CIOBAUD 9600UL
#define ESP_FAST_CIOBAUD 115200UL
#define ESP_TIMEOUT 15000UL

#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#include "browser_control.h"

Adafruit_NeoPixel strip(LEDS, LED_CHAIN_PIN, NEO_GRB + NEO_KHZ800);

String command_str;

void setup(void)
{
	strip.begin();
	lamp_test();
	init_ESP8266();
}

void loop(void)
{
	if (Serial.available()) {
		char c = Serial.read();
		if (c == '\n') {
			parseCommand(command_str);
			command_str = "";
			clear_serial_buffer();
		} else if (c != '\r')
			command_str += c;
	}
}

void parseCommand(String com_str)
{
	String maincmd_str;

	int index_of_semicolon = com_str.indexOf(":");

	if (index_of_semicolon != -1) {
		// found the ":" in "+IPD,0,297:GET..."

		int index_of_1st_comma = com_str.indexOf(",");
		int index_of_2nd_comma = com_str.indexOf(",", index_of_1st_comma + 1);

		String incoming_connection_str = com_str.substring(index_of_1st_comma + 1, index_of_2nd_comma);

		uint8_t incoming_connection_number = (uint8_t) (incoming_connection_str.toInt());

		maincmd_str = com_str.substring(index_of_semicolon + 1);

		if (maincmd_str.length() > 0) {

			Serial.print(F("AT+CIPSEND="));
			Serial.print(incoming_connection_number);
			Serial.print(F(","));
			Serial.print(12 + maincmd_str.length() + 1 + 15 + number_of_digits(maincmd_str.length()) + 1);
			Serial.print(ESP_LINE_TERM);
			if (wait_for("OK") != true) {
				init_ESP8266();
				return;
			}
			Serial.print("\n\nYou sent: ");	// 12
			Serial.print(maincmd_str);
			Serial.print(F("\n"));	// 1
			Serial.print(F("Reqest length: "));	// 15
			Serial.print(maincmd_str.length());
			Serial.print(F("\n"));	// 1
			if (wait_for("OK") != true) {
				init_ESP8266();
				return;
			}

		} else {
			Serial.print(F("AT+CIPSEND="));
			Serial.print(incoming_connection_number);
			Serial.print(F(","));
			Serial.print(F("31"));
			Serial.print(ESP_LINE_TERM);
			if (wait_for("OK") != true) {
				init_ESP8266();
				return;
			}
			Serial.print(F("commands:\n\n* on\n* off\n* blink\n\n"));
			if (wait_for("OK") != true) {
				init_ESP8266();
				return;
			}
		}

		// bye bye
		Serial.print(F("AT+CIPSEND="));
		Serial.print(incoming_connection_number);
		Serial.print(F(","));
		Serial.print(F("9"));
		Serial.print(ESP_LINE_TERM);
		if (wait_for("OK") != true) {
			init_ESP8266();
			return;
		}
		Serial.print(F("Bye Bye!\n"));
		if (wait_for("OK") != true) {
			init_ESP8266();
			return;
		}
		// close connection
		Serial.print(F("AT+CIPCLOSE="));
		Serial.print(5);	// close ALL connections. Just closing the current one leads to "busy p..." crap
		Serial.print(ESP_LINE_TERM);
		if (wait_for("OK") != true) {
			init_ESP8266();
			return;
		}
	}
}

bool wait_for(const char *text)
{
	// first attempts failed miserably
	//
	// need to precicely check what the ESP8266 sends back
	// (including the \r\n stuff) --> logic analyzer
	// terminal + hexdump gives inconsistent results [useless]
	//
	// either this is inherently inconsistent / unreliable or I had
	// a pretty substantial "brain-AFK" yesterday

	String temp_str = "";

	bool retval = false;

	uint32_t start_time = millis();

	while ((millis() - start_time) < ESP_TIMEOUT) {

		if (Serial.available()) {
			char c = Serial.read();
			if (c == '\n') {
				if (temp_str.indexOf(text) != -1) {
					clear_serial_buffer();
					retval = true;
					delay(100);
					break;
				} else if (temp_str.indexOf("ERROR") != -1) {
					clear_serial_buffer();
					retval = false;
					delay(100);
					break;
				} else if (temp_str.indexOf("busy") != -1) {
					clear_serial_buffer();
					retval = false;
					delay(100);
					break;
				}
				temp_str = "";	// didn't find "OK" in this line, clear & keep on reading new data
			} else if (c != '\r')
				temp_str += c;
		}

	}

	return retval;
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

void clear_serial_buffer(void)
{
	uint8_t dummy;
	while (Serial.available()) {
		dummy = Serial.read();
	}
}

void toggle_2nd_WS(void)
{
	static uint8_t state = 0;
	switch (state) {
	case 0:
		strip.setPixelColor(1, 0, 32, 0);
		state = 1;
		break;
	case 1:
		strip.setPixelColor(1, 32, 0, 0);
		state = 0;
		break;
	default:
		break;
	}
	strip.show();

}

unsigned int number_of_digits(unsigned int number)
{
	unsigned int digits = 0;
	unsigned int temp;

	temp = number;

	if (temp == 0) {
		digits++;
	}

	while (temp > 0) {
		temp = temp / 10;
		digits++;
	}

	return digits;
}

void lamp_test(void)
{
	uint8_t LED_counter;
	int16_t brightness;

	for (brightness = 0; brightness <= 64; brightness++) {
		for (LED_counter = 0; LED_counter <= 64; LED_counter++) {
			strip.setPixelColor(LED_counter, brightness, brightness, brightness);
		}
		strip.show();
		delay(5);
	}

	for (brightness = 64; brightness >= 0; brightness--) {
		for (LED_counter = 0; LED_counter <= 64; LED_counter++) {
			strip.setPixelColor(LED_counter, brightness, brightness, brightness);
		}
		strip.show();
		delay(5);
	}

}

void init_ESP8266(void)
{
	Serial.begin(ESP_FAST_CIOBAUD);
	while (Serial.available()) {
		uint8_t dummy = Serial.read();	// make sure input butter is empty
	}

	pinMode(RESET, OUTPUT);
	digitalWrite(RESET, LOW);
	delay(100);
	digitalWrite(RESET, HIGH);	// RESET is "active-low"

	pinMode(ENABLE, OUTPUT);
	digitalWrite(ENABLE, HIGH);

	wait_for("ready");	// wait for ESP8266 to boot up

	// set mode
	Serial.print(F("AT+CWMODE=1"));
	Serial.print(ESP_LINE_TERM);
	wait_for("OK");		// this really should be replaced with "wait_for_OK()" !

	// join access point
	Serial.print(F("AT+CWJAP=\""));
	Serial.print(ESP_SSID);
	Serial.print(F("\",\""));
	Serial.print(ESP_PASS);
	Serial.print(F("\""));
	Serial.print(ESP_LINE_TERM);
	wait_for("OK");

	// set static IP address
	Serial.print(F("AT+CIPSTA=\""));
	Serial.print(ESP_STATIC_IP);
	Serial.print(F("\""));
	Serial.print(ESP_LINE_TERM);
	wait_for("OK");

	// get IP address
	Serial.print(F("AT+CIFSR"));
	Serial.print(ESP_LINE_TERM);
	wait_for("OK");

	// now you should be able to PING the board

	// start SERVER
	Serial.print(F("AT+CIPMODE=0"));
	Serial.print(ESP_LINE_TERM);
	wait_for("OK");

	Serial.print(F("AT+CIPMUX=1"));
	Serial.print(ESP_LINE_TERM);
	wait_for("OK");

	Serial.print(F("AT+CIPSERVER=1,"));
	Serial.print(ESP_SERVER_PORT);
	Serial.print(ESP_LINE_TERM);
	wait_for("OK");

	Serial.print(F("AT+CIPSTO=5000"));
	Serial.print(ESP_LINE_TERM);
	wait_for("OK");

	strip.setPixelColor(0, 32, 0, 0);
	strip.show();
	delay(250);
	strip.setPixelColor(0, 0, 32, 0);
	strip.show();
	delay(250);
	clear_serial_buffer();
}
