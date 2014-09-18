#include <stdint.h>
#include <stdio.h>

//

int main(void);
void hsv_to_rgb(uint16_t heu, uint8_t sat, uint8_t val, uint8_t * tmp_array);

//

int main(void)
{

	uint8_t tmp[3];
	uint16_t hue;

	for (hue = 0; hue <= 360; hue++) {

		hsv_to_rgb(hue, 255, 255, tmp);
		printf("HUE: %d -- ", hue);
		printf("RED: %d - ", tmp[0]);
		printf("GREEN: %d - ", tmp[1]);
		printf("BLUE: %d - ", tmp[2]);
		printf("SUM: %d\n", tmp[0] + tmp[1] + tmp[2]);

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
