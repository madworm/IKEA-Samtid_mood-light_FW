#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#include "simple_serial.h"

Adafruit_NeoPixel strip(LEDS, LED_CHAIN_PIN, NEO_GRB + NEO_KHZ800);

uint8_t rgb_values[3] = { 0, 0, 0 };	// store RGB values after conversion from HSV

String command_str;

unsigned int request_ctr = 0;

void setup(void)
{
	strip.begin();
	lamp_test();
	Serial.begin(115200);
	clear_serial_buffer();
}

void loop(void)
{
	if (Serial.available()) {
		char c = Serial.read();
		if (c == '\n') {
			parseCommand(&command_str);
			command_str = "";
			clear_serial_buffer();
		} else if (c != '\r')
			command_str += c;
	}
}

void parseCommand(String * com_str)
{
	int index_of_SET = com_str->indexOf("SET:");

	// set default values

	REQ_VAR req_vars[9] =
	    { {"mode=", 0, 16, 5}, {"start=", 0, (LEDS - 1), 6}, {"stop=", (LEDS - 1), (LEDS - 1), 5}, {"hue=", 0, 360, 4}, {"sat=", 255, 255, 4}, {"val=", 32, 255, 4}, {"red=", 32, 255, 4}, {"green=", 32, 255, 6}, {"blue=", 32, 255, 5} };

	if (index_of_SET != -1) {
		// found the ":" in "SET:mode=0&start=0&..."

		*com_str = com_str->substring(index_of_SET + 3);

		// parsing com_str goes here
		// proposed format: mode=<...>&start=<...>&stop=<...>&hue=<...>&sat=<...>&val=<...>&
		// default values: mode = 0, start = 0, stop = (LEDS-1), (hue,sat,val) = current values

		// look for mode=<...> etc.

		uint8_t ctr;
		int index_a;
		int index_b;

		for (ctr = 0; ctr <= 8; ctr++) {
			index_a = com_str->indexOf(req_vars[ctr].name);
			if (index_a != -1) {
				index_b = com_str->indexOf("&", index_a);
				if (index_b != -1) {
					String tmp_str = com_str->substring(index_a + req_vars[ctr].offset, index_b);
					req_vars[ctr].value = constrain((unsigned int)(tmp_str.toInt()), 0, req_vars[ctr].max);
				}
			}
		}

		uint8_t which_led;

		switch (req_vars[0].value) {	// switch mode
		case 0:	// HSV
			hsv_to_rgb(req_vars[3].value, req_vars[4].value, req_vars[5].value, rgb_values);

			for (which_led = req_vars[1].value; which_led <= req_vars[2].value; which_led++) {
				strip.setPixelColor(which_led, rgb_values[0], rgb_values[1], rgb_values[2]);	// H S V
			}
			strip.show();
			break;

		case 1:	// RGB
			for (which_led = req_vars[1].value; which_led <= req_vars[2].value; which_led++) {
				strip.setPixelColor(which_led, req_vars[6].value, req_vars[7].value, req_vars[8].value);	// R G B
			}
			strip.show();
			break;

		default:
			break;
		}

	}

}

void clear_serial_buffer(void)
{
	uint8_t dummy;
	while (Serial.available()) {
		dummy = Serial.read();
	}
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

void hsv_to_rgb(uint16_t hue, uint8_t sat, uint8_t val, uint8_t * tmp_array)
{
	/* BETA */

	/* finally thrown out all of the float stuff and replaced with uint16_t
	 *
	 * hue: 0-->360 (hue, color)
	 * sat: 0-->255 (saturation)
	 * val: 0-->255 (value, brightness)
	 *
	 */

	hue = hue % 360;
	uint8_t sector = hue / 60;
	uint8_t rel_pos = hue - (sector * 60);
	uint16_t const mmd = 65025;	// 255 * 255 /* maximum modulation depth */
	uint16_t top = val * 255;
	uint16_t bottom = val * (255 - sat);	/* (val*255) - (val*255)*(sat/255) */
	uint16_t slope = (uint16_t) (val) * (uint16_t) (sat) / 120;	/* dy/dx = (top-bottom)/(2*60) -- val*sat: modulation_depth dy */
	uint16_t a = bottom + slope * rel_pos;
	uint16_t b = bottom + (uint16_t) (val) * (uint16_t) (sat) / 2 + slope * rel_pos;
	uint16_t c = top - slope * rel_pos;
	uint16_t d = top - (uint16_t) (val) * (uint16_t) (sat) / 2 - slope * rel_pos;

	uint16_t R, G, B;

	if (sector == 0) {
		R = c;
		G = a;
		B = bottom;
	} else if (sector == 1) {
		R = d;
		G = b;
		B = bottom;
	} else if (sector == 2) {
		R = bottom;
		G = c;
		B = a;
	} else if (sector == 3) {
		R = bottom;
		G = d;
		B = b;
	} else if (sector == 4) {
		R = a;
		G = bottom;
		B = c;
	} else {
		R = b;
		G = bottom;
		B = d;
	}

	uint16_t scale_factor = mmd / 255;

	R = (uint8_t) (R / scale_factor);
	G = (uint8_t) (G / scale_factor);
	B = (uint8_t) (B / scale_factor);

	tmp_array[0] = R;
	tmp_array[1] = G;
	tmp_array[2] = B;
}
