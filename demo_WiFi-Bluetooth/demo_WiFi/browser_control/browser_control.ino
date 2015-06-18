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
#define ESP_TIMEOUT 10000UL

#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#include "browser_control.h"

Adafruit_NeoPixel strip(LEDS, LED_CHAIN_PIN, NEO_GRB + NEO_KHZ800);

String command_str;
unsigned int request_ctr = 0;

void setup(void)
{
	strip.begin();
	lamp_test();

	bool ESP8266_OK = false;

	// loop until we get the ESP8266 properly initialized & joined a network
	while (ESP8266_OK == false) {
		ESP8266_OK = init_ESP8266();
	}
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

	// set default values
	unsigned int led_mode = 0;
	unsigned int led_start = 0;
	unsigned int led_stop = (LEDS - 1);
	unsigned int led_hue = 0;
	unsigned int led_sat = 255;
	unsigned int led_val = 255;

	if (index_of_semicolon != -1) {
		// found the ":" in "+IPD,0,297:GET..."
		request_ctr++;

		int index_of_1st_comma = com_str.indexOf(",");
		int index_of_2nd_comma = com_str.indexOf(",", index_of_1st_comma + 1);

		String incoming_connection_str = com_str.substring(index_of_1st_comma + 1, index_of_2nd_comma);

		uint8_t incoming_connection_number = (uint8_t) (incoming_connection_str.toInt());

		maincmd_str = com_str.substring(index_of_semicolon + 1);

		Serial.print(F("AT+CIPSEND="));
		Serial.print(incoming_connection_number);
		Serial.print(F(","));
		Serial.print(11 + maincmd_str.length() + 1 + 16 + NoD(maincmd_str.length()) + 1 + 15 + NoD(request_ctr) + 1);
		Serial.print(ESP_LINE_TERM);
		if (wait_for("OK") != true) {
			init_ESP8266();
			return;
		}
		Serial.print("\nYou sent: ");	// 11
		Serial.print(maincmd_str);
		Serial.print(F("\n"));	// 1
		Serial.print(F("Request length: "));	// 16
		Serial.print(maincmd_str.length());
		Serial.print(F("\n"));	// 1
		Serial.print(F("Request count: "));	// 15
		Serial.print(request_ctr);
		Serial.print(F("\n"));	// 1
		if (wait_for("OK") != true) {
			init_ESP8266();
			return;
		}

		if (maincmd_str.length() <= 14) {

			Serial.print(F("AT+CIPSEND="));
			Serial.print(incoming_connection_number);
			Serial.print(F(","));
			Serial.print(F("14"));
			Serial.print(ESP_LINE_TERM);
			if (wait_for("OK") != true) {
				init_ESP8266();
				return;
			}
			Serial.print(F("commands: tba\n"));	// 14
			if (wait_for("OK") != true) {
				init_ESP8266();
				return;
			}
		} else {
			// parsing maincmd_str goes here
			// proposed format: mode=<...>&start=<...>&stop=<...>&hue=<...>&sat=<...>&val=<...>&
			// default values: mode = 0, start = 0, stop = (LEDS-1), (hue,sat,val) = current values

			// look for mode=<...> etc.

			int index_of_mode = maincmd_str.indexOf("mode=");	// -1 if nothing found
			if (index_of_mode != -1) {
				int index_of_mode_term = maincmd_str.indexOf("&", index_of_mode);	// search '&' after 'mode'
				if (index_of_mode_term != -1) {
					String tmp_str = maincmd_str.substring(index_of_mode + 5, index_of_mode_term);	// "mode=123&..." take "123"
					led_mode = (unsigned int)(tmp_str.toInt());
				}
			}

			int index_of_start = maincmd_str.indexOf("start=");
			if (index_of_start != -1) {
				int index_of_start_term = maincmd_str.indexOf("&", index_of_start);
				if (index_of_start_term != -1) {
					String tmp_str = maincmd_str.substring(index_of_start + 6, index_of_start_term);	// "start=234&..." take "234"
					led_start = (unsigned int)(tmp_str.toInt());
				}
			}

			int index_of_stop = maincmd_str.indexOf("stop=");
			if (index_of_stop != -1) {
				int index_of_stop_term = maincmd_str.indexOf("&", index_of_stop);
				if (index_of_stop_term != -1) {
					String tmp_str = maincmd_str.substring(index_of_stop + 5, index_of_stop_term);	// "stop=456&..." take "456"
					led_stop = (unsigned int)(tmp_str.toInt());
				}
			}

			int index_of_hue = maincmd_str.indexOf("hue=");
			if (index_of_hue != -1) {
				int index_of_hue_term = maincmd_str.indexOf("&", index_of_hue);
				if (index_of_hue_term != -1) {
					String tmp_str = maincmd_str.substring(index_of_hue + 4, index_of_hue_term);	// "hue=123&..." take "123"
					led_hue = (unsigned int)(tmp_str.toInt());
				}
			}

			int index_of_sat = maincmd_str.indexOf("sat=");
			if (index_of_sat != -1) {
				int index_of_sat_term = maincmd_str.indexOf("&", index_of_sat);
				if (index_of_sat_term != -1) {
					String tmp_str = maincmd_str.substring(index_of_sat + 4, index_of_sat_term);	// "sat=456&..." take "456"
					led_sat = (unsigned int)(tmp_str.toInt());
				}
			}

			int index_of_val = maincmd_str.indexOf("val=");
			if (index_of_val != -1) {
				int index_of_val_term = maincmd_str.indexOf("&", index_of_val);
				if (index_of_val_term != -1) {
					String tmp_str = maincmd_str.substring(index_of_val + 4, index_of_val_term);	// "val=789&..." take "789"
					led_val = (unsigned int)(tmp_str.toInt());
				}
			}

			Serial.print(F("AT+CIPSEND="));
			Serial.print(incoming_connection_number);
			Serial.print(F(","));
			Serial.print(NoD(led_mode) + NoD(led_start) + NoD(led_stop) + NoD(led_hue) + NoD(led_sat) + NoD(led_val) + 40);
			Serial.print(ESP_LINE_TERM);
			if (wait_for("OK") != true) {
				init_ESP8266();
				return;
			}
			Serial.print(F("mode: "));	// 6
			Serial.print(led_mode);
			Serial.print(F("\n"));	// 1                        
			Serial.print(F("start: "));	// 7
			Serial.print(led_start);
			Serial.print(F("\n"));	// 1                                                
			Serial.print(F("stop: "));	// 6
			Serial.print(led_stop);
			Serial.print(F("\n"));	// 1                                                
			Serial.print(F("hue: "));	// 5
			Serial.print(led_hue);
			Serial.print(F("\n"));	// 1                                                
			Serial.print(F("sat: "));	// 5
			Serial.print(led_sat);
			Serial.print(F("\n"));	// 1                                                
			Serial.print(F("val: "));	// 5
			Serial.print(led_val);
			Serial.print(F("\n"));	// 1                                                
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
		Serial.print(F("Bye Bye!\n"));	// 9
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
			} else if (c != '\r') {
				temp_str += c;
			}

			if (temp_str.length() > 32) {
				// throw away everything except the last 32 characters
				// the ESP8266 spits out a lot of garbage after booting
				// before anything useful is sent (e.g. "ready").
				temp_str = "";
			}
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

unsigned int NoD(unsigned int number)
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

bool init_ESP8266(void)
{
	bool retval = true;

	Serial.begin(ESP_FAST_CIOBAUD);
	while (Serial.available()) {
		uint8_t dummy = Serial.read();	// make sure input butter is empty
	}

	strip.setPixelColor(0, 32, 0, 0);
	strip.show();
	delay(250);

	// reset ESP8266 & enable
	pinMode(RESET, OUTPUT);
	digitalWrite(RESET, LOW);
	delay(100);
	digitalWrite(RESET, HIGH);	// RESET is "active-low"

	pinMode(ENABLE, OUTPUT);
	digitalWrite(ENABLE, HIGH);

	if (wait_for("ready") != true) {	// wait for ESP8266 to boot up
		retval = false;
	}
	// set mode
	Serial.print(F("AT+CWMODE=1"));
	Serial.print(ESP_LINE_TERM);
	if (wait_for("OK") != true) {
		retval = false;	// this really should be replaced with "wait_for_OK()" !
	}
	// join access point
	Serial.print(F("AT+CWJAP=\""));
	Serial.print(ESP_SSID);
	Serial.print(F("\",\""));
	Serial.print(ESP_PASS);
	Serial.print(F("\""));
	Serial.print(ESP_LINE_TERM);
	if (wait_for("OK") != true) {
		retval = false;
	}
	// set static IP address
	Serial.print(F("AT+CIPSTA=\""));
	Serial.print(ESP_STATIC_IP);
	Serial.print(F("\""));
	Serial.print(ESP_LINE_TERM);
	if (wait_for("OK") != true) {
		retval = false;
	}
	// get IP address
	Serial.print(F("AT+CIFSR"));
	Serial.print(ESP_LINE_TERM);
	if (wait_for("OK") != true) {
		retval = false;
	}
	// now you should be able to PING the board

	// start SERVER
	Serial.print(F("AT+CIPMODE=0"));
	Serial.print(ESP_LINE_TERM);
	if (wait_for("OK") != true) {
		retval = false;
	}

	Serial.print(F("AT+CIPMUX=1"));
	Serial.print(ESP_LINE_TERM);
	if (wait_for("OK") != true) {
		retval = false;
	}

	Serial.print(F("AT+CIPSERVER=1,"));
	Serial.print(ESP_SERVER_PORT);
	Serial.print(ESP_LINE_TERM);
	if (wait_for("OK") != true) {
		retval = false;
	}

	Serial.print(F("AT+CIPSTO=5000"));
	Serial.print(ESP_LINE_TERM);
	if (wait_for("OK") != true) {
		retval = false;
	}

	if (retval == true) {
		strip.setPixelColor(0, 0, 32, 0);
		strip.show();
		delay(250);
		strip.setPixelColor(0, 0, 0, 0);
		strip.show();
	}
	clear_serial_buffer();

	return retval;
}
