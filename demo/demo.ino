/*
 * IKEA-Samtid reading light upgrade firmware
 *
 * Modes of operation:
 * ===================
 *
 * Any valid button press will cause the on-board LED to flash
 *
 * M-button: 1) double-click --> change mode (works any time)
 *           2) press & hold --> decrease / cycle left
	     3) single-click --> decrease delay
 *
 * E-button: 1) press & hold --> increase / cycle right
 *           2) single-click --> increase delay
 *
 */

// Install: Adafruit NeoPixel library
// Install: ClickButton library
#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#include <ClickButton.h>
#include "demo.h"

#define PIN 1			// adapted to IKEA-Samtid_mood-light hardware
#define LEDS 64			// adjust to number of installed WS2812B [1-64]
#define M_BUTTON 12
#define E_BUTTON 11
#define PRESSED 0

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip(LEDS, PIN, NEO_GRB + NEO_KHZ800);
ClickButton button_m(M_BUTTON, LOW, CLICKBTN_PULLUP);
ClickButton button_e(E_BUTTON, LOW, CLICKBTN_PULLUP);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

uint8_t HSV_value_global;	// value of HSV triplet

void setup()
{
	button_m.debounceTime = 20;	// Debounce timer in ms
	button_m.multiclickTime = 250;	// Time limit for multi clicks
	button_m.longClickTime = 1000;
	button_e.debounceTime = 20;	// Debounce timer in ms
	button_e.multiclickTime = 250;	// Time limit for multi clicks
	button_e.longClickTime = 1000;
	strip.begin();
	strip.show();		// Initialize all pixels to 'off'
	pinMode(13, OUTPUT);
}

void loop()
{
	static uint8_t mode = 0;
	static int8_t clicks = 0;

	//testing();

	button_m.Update();
	button_e.Update();

	if (button_m.clicks != 0) {
		clicks = button_m.clicks;
		//flash_LED();
	}

	if (clicks == 2) {
		mode = (mode + 1) % 8;
		clicks = 0;
		flash_LED();
	}

	switch (mode) {
	case 0:
		white_NB(20, ALL);	// delay (0-255), ALL, TOP, BOTTOM
		break;
	case 1:
		white_NB(20, TOP);
		break;
	case 2:
		white_NB(20, BOTTOM);
		break;
	case 3:
		ring_hv_NB(45);	// initial hue (0-360)
		break;
	case 4:
		ring_split_hv_NB(45);	// initial hue (0-360)
		break;
	case 5:
		ring_cycle_hv_NB();
		break;
	case 6:
		rainbow_NB(10);	// delay (0-255)
		break;
	case 7:
		rainbowCycle_NB(10);	// delay (0-255)
		break;
	default:
		break;
	}
}

void flash_LED(void)
{
	digitalWrite(13, HIGH);
	delay(10);
	digitalWrite(13, LOW);
}

void testing(void)
{
	int8_t clicks = 0;
	static uint8_t mode = 0;
	while (1) {

		button_m.Update();
		button_e.Update();

		if (button_m.clicks != 0) {
			clicks = button_m.clicks;
			//flash_LED();
		}

		if (button_m.clicks == -1) {
			mode = (mode + 1) % 5;
			digitalWrite(13, !digitalRead(13));
			clicks = 0;
			flash_LED();
		}

	}
}

void ring_hv_NB(uint16_t hue)
{
	uint16_t i;
	static uint16_t hue_local = 0;
	static uint32_t last_run = 0;
	static uint8_t button_e_long_press_detected = 0;
	uint32_t time_now = millis();
	uint8_t tmp_array[3];

	if (last_run == 0) {
		hue_local = hue;
	}

	if (button_e.clicks == -1) {
		button_e_long_press_detected = 1;
		flash_LED();
	}

	if ((time_now - last_run) > 20) {

		if (button_e.depressed && button_e_long_press_detected == 1) {
			hue_local++;
			flash_LED();
		} else {
			button_e_long_press_detected = 0;
		}

		hsv_to_rgb(hue_local, 255, HSV_value_global, tmp_array);

		for (i = 0; i < strip.numPixels(); i++) {
			strip.setPixelColor(i, tmp_array[0], tmp_array[1],
					    tmp_array[2]);
		}

		strip.show();
		last_run = millis();
	}
}

void ring_split_hv_NB(uint16_t hue)
{
	uint16_t i;
	static uint16_t hue_top_local = 0;
	static uint16_t hue_bottom_local = 0;
	static uint32_t last_run = 0;
	static uint8_t button_e_long_press_detected = 0;
	static uint8_t button_m_long_press_detected = 0;
	uint32_t time_now = millis();
	uint8_t tmp_array[3];

	if (last_run == 0) {
		hue_top_local = hue;
		hue_bottom_local = hue + 180;
	}

	if (button_e.clicks == -1) {
		button_e_long_press_detected = 1;
		flash_LED();
	}

	if (button_m.clicks == -1) {
		button_m_long_press_detected = 1;
		flash_LED();
	}

	if ((time_now - last_run) > 20) {

		if (button_e.depressed && button_e_long_press_detected == 1) {
			hue_top_local++;
			flash_LED();
		} else {
			button_e_long_press_detected = 0;
		}

		if (button_m.depressed && button_m_long_press_detected == 1) {
			hue_bottom_local++;
			flash_LED();
		} else {
			button_m_long_press_detected = 0;
		}

		hsv_to_rgb(hue_bottom_local, 255, HSV_value_global, tmp_array);

		for (i = 0; i < (strip.numPixels() / 2); i++) {
			strip.setPixelColor(i, tmp_array[0], tmp_array[1],
					    tmp_array[2]);
		}

		hsv_to_rgb(hue_top_local, 255, HSV_value_global, tmp_array);

		for (i = (strip.numPixels() / 2); i < strip.numPixels(); i++) {
			strip.setPixelColor(i, tmp_array[0], tmp_array[1],
					    tmp_array[2]);
		}

		strip.show();
		last_run = millis();
	}
}

void ring_cycle_hv_NB(void)
{
	uint16_t i;
	static uint16_t hue_local = 0;
	static uint32_t last_run = 0;
	static uint8_t wait_local = 0;
	uint32_t time_now = millis();
	uint8_t tmp_array[3];

	if (last_run == 0) {
		hue_local = 0;	// start with RED
		wait_local = 20;
	}

	if (button_e.clicks == 1) {
		if ((255 - wait_local) >= 5) {
			wait_local += 5;
		} else {
			wait_local = 255;
		}
		flash_LED();
	}

	if (button_m.clicks == 1) {
		if ((wait_local - 0) >= 5) {
			wait_local -= 5;
		} else {
			wait_local = 0;
		}
		flash_LED();
	}

	if ((time_now - last_run) > wait_local) {

		hue_local++;
		hsv_to_rgb(hue_local, 255, HSV_value_global, tmp_array);

		for (i = 0; i < strip.numPixels(); i++) {
			strip.setPixelColor(i, tmp_array[0], tmp_array[1],
					    tmp_array[2]);
		}

		strip.show();
		last_run = millis();
	}
}

void set_ring_hsv(uint16_t hue, uint8_t sat, uint8_t val)
{
	uint8_t i;
	uint8_t tmp_array[3];
	hsv_to_rgb(hue, sat, val, tmp_array);

	uint8_t red = tmp_array[0];
	uint8_t green = tmp_array[1];
	uint8_t blue = tmp_array[2];

	for (i = 0; i < strip.numPixels(); i++) {
		strip.setPixelColor(i, red, green, blue);
	}

	strip.show();
}

void white_NB(uint8_t wait, which_side_t side)
{
	uint16_t i;
	static uint8_t run_once = 0;
	static uint8_t c = 0;
	static uint32_t last_run = 0;
	static uint8_t button_e_long_press_detected = 0;
	static uint8_t button_m_long_press_detected = 0;
	uint32_t time_now = millis();

	if (button_e.clicks == -1) {
		button_e_long_press_detected = 1;
		flash_LED();
	}

	if (button_m.clicks == -1) {
		button_m_long_press_detected = 1;
		flash_LED();
	}

	if (run_once == 0) {

		if ((c < 255) && ((time_now - last_run) > wait)) {
			c++;
			HSV_value_global = c;

			if (c == 255) {
				run_once = 1;
			}

			for (i = 0; i < strip.numPixels(); i++) {
				strip.setPixelColor(i, c, c, c);
			}

			strip.show();
			last_run = millis();
		}
	} else if ((time_now - last_run) > wait) {

		if (button_e.depressed && button_e_long_press_detected
		    && (c < 255)) {
			c++;
			HSV_value_global = c;
			flash_LED();
		} else {
			button_e_long_press_detected = 0;
		}

		if (button_m.depressed && button_m_long_press_detected
		    && (c > 0)) {
			c--;
			HSV_value_global = c;
			flash_LED();
		} else {
			button_m_long_press_detected = 0;
		}

		switch (side) {
		case ALL:
			for (i = 0; i < strip.numPixels(); i++) {
				strip.setPixelColor(i, c, c, c);
			}
			break;
		case TOP:
			for (i = 0; i < (strip.numPixels() / 2); i++) {
				strip.setPixelColor(i, 0, 0, 0);
			}
			for (i = (strip.numPixels() / 2); i < strip.numPixels();
			     i++) {
				strip.setPixelColor(i, c, c, c);
			}
			break;
		case BOTTOM:
			for (i = 0; i < (strip.numPixels() / 2); i++) {
				strip.setPixelColor(i, c, c, c);
			}
			for (i = (strip.numPixels() / 2); i < strip.numPixels();
			     i++) {
				strip.setPixelColor(i, 0, 0, 0);
			}
			break;
		default:
			break;
		}

		strip.show();
		last_run = millis();
	}
}

void rainbow_NB(uint8_t wait)
{
	uint16_t i;
	static uint16_t j = 0;
	static uint32_t last_run = 0;
	uint32_t time_now = millis();
	static uint8_t wait_local = 0;

	if (last_run == 0) {
		wait_local = wait;
	}

	if ((button_e.clicks == 1)) {
		if ((255 - wait_local) >= 5) {
			wait_local += 5;
		} else {
			wait_local = 255;
		}
		flash_LED();
	}

	if ((button_m.clicks == 1)) {
		if ((wait_local - 0) >= 5) {
			wait_local -= 5;
		} else {
			wait_local = 0;
		}
		flash_LED();
	}

	if ((time_now - last_run) > wait_local) {

		if (j < 256) {
			for (i = 0; i < strip.numPixels(); i++) {
				strip.setPixelColor(i, Wheel((i + j) & 255));
			}
			strip.show();
			j++;
			if (j == 256) {
				j = 0;
			}
			last_run = millis();
		}
	}
}

void rainbowCycle_NB(uint8_t wait)
{
	uint16_t i;
	static uint16_t j = 0;
	static uint32_t last_run = 0;
	uint32_t time_now = millis();
	static uint8_t wait_local = 0;

	if (last_run == 0) {
		wait_local = wait;
	}

	if ((button_e.clicks == 1)) {
		if ((255 - wait_local) >= 5) {
			wait_local += 5;
		} else {
			wait_local = 255;
		}
		flash_LED();
	}

	if ((button_m.clicks == 1)) {
		if ((wait_local - 0) >= 5) {
			wait_local -= 5;
		} else {
			wait_local = 0;
		}
		flash_LED();
	}

	if ((time_now - last_run) > wait_local) {

		if (j < 256) {
			for (i = 0; i < strip.numPixels(); i++) {

				strip.setPixelColor(i,
						    Wheel(((i * 256 /
							    strip.numPixels()) +
							   j) & 255));
			}
			strip.show();
			j++;
			if (j == 256) {
				j = 0;
			}
			last_run = millis();
		}
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
	uint16_t b =
	    bottom + (uint16_t) (val) * (uint16_t) (sat) / 2 + slope * rel_pos;
	uint16_t c = top - slope * rel_pos;
	uint16_t d =
	    top - (uint16_t) (val) * (uint16_t) (sat) / 2 - slope * rel_pos;

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

//
// These still need to be rewritten to non-blocking mode
//

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait)
{
	for (uint16_t i = 0; i < strip.numPixels(); i++) {
		strip.setPixelColor(i, c);
		strip.show();
		button_m.Update();
		button_e.Update();
		if (button_m.depressed) {
			break;
		}
		delay(wait);
	}
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait)
{
	for (int j = 0; j < 10; j++) {	//do 10 cycles of chasing
		for (int q = 0; q < 3; q++) {
			for (int i = 0; i < strip.numPixels(); i = i + 3) {
				strip.setPixelColor(i + q, c);	//turn every third pixel on
			}
			strip.show();

			button_m.Update();
			button_e.Update();

			if (button_m.depressed) {
				break;
			}

			delay(wait);

			for (int i = 0; i < strip.numPixels(); i = i + 3) {
				strip.setPixelColor(i + q, 0);	//turn every third pixel off
			}
		}
	}
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait)
{
	for (int j = 0; j < 256; j++) {	// cycle all 256 colors in the wheel
		for (int q = 0; q < 3; q++) {
			for (int i = 0; i < strip.numPixels(); i = i + 3) {
				strip.setPixelColor(i + q, Wheel((i + j) % 255));	//turn every third pixel on
			}
			strip.show();

			button_m.Update();
			button_e.Update();

			if (button_m.depressed) {
				break;
			}

			delay(wait);

			for (int i = 0; i < strip.numPixels(); i = i + 3) {
				strip.setPixelColor(i + q, 0);	//turn every third pixel off
			}
		}
	}
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
	if (WheelPos < 85) {
		return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	} else if (WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	} else {
		WheelPos -= 170;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
}

	/*

	   switch (mode) {
	   case 0:
	   colorWipe(strip.Color(255, 0, 0), time_delay);       // Red
	   colorWipe(strip.Color(0, 255, 0), time_delay);       // Green
	   colorWipe(strip.Color(0, 0, 255), time_delay);       // Blue
	   break;
	   case 1:
	   theaterChase(strip.Color(127, 127, 127), time_delay);        // White
	   break;
	   case 2:
	   theaterChase(strip.Color(127, 0, 0), time_delay);    // Red
	   break;
	   case 3:
	   theaterChase(strip.Color(0, 0, 127), time_delay);    // Blue
	   break;
	   case 6:
	   theaterChaseRainbow(time_delay);
	   break;
	   default:
	   break;
	   }

	 */
