// Install: Adafruit NeoPixel library
// Install: ClickButton library
#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#include <ClickButton.h>

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
	int8_t clicks;

	button_m.Update();
	button_e.Update();

	if (button_m.clicks != 0) {
		clicks = button_m.clicks;
	}

	if (clicks == 2) {
		mode = (mode + 1) % 3;
		digitalWrite(13, !digitalRead(13));
		clicks = 0;
	}

	switch (mode) {
	case 0:
		white_NB(20);
		break;
	case 1:
		rainbow_NB(10);
		break;
	case 2:
		rainbowCycle_NB(10);
		break;
	default:
		break;
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
}

void white_NB(uint8_t wait)
{
	uint16_t i;
	static uint8_t run_once = 0;
	static uint16_t c = 0;
	static uint32_t last_run = 0;
	uint32_t time_now = millis();

	if (run_once == 0) {

		if ((c < 255) && ((time_now - last_run) > wait)) {
			c++;
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

		if (button_e.depressed && (c < 255)) {
			c++;
		}

		if (button_m.depressed && (c > 0)) {
			c--;
		}

		for (i = 0; i < strip.numPixels(); i++) {
			strip.setPixelColor(i, c, c, c);
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

	if ((button_e.clicks == 1)) {
		if ((255 - wait_local) >= 5) {
			wait_local += 5;
		} else {
			wait_local = 255;
		}
	}

	if ((button_m.clicks == 1)) {
		if ((wait_local - 0) >= 5) {
			wait_local -= 5;
		} else {
			wait_local = 0;
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
	if ((button_e.clicks == 1)) {
		if ((255 - wait_local) >= 5) {
			wait_local += 5;
		} else {
			wait_local = 255;
		}
	}

	if ((button_m.clicks == 1)) {
		if ((wait_local - 0) >= 5) {
			wait_local -= 5;
		} else {
			wait_local = 0;
		}
	}
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
