#define EP_SSID (F("test-ssid"))
#define EP_PASS (F("test-pass"))
#define STATIC_IP (F("192.168.1.8"))
#define LINE_TERM (F("\r\n"))
#define INIT_CIBAUD 9600UL
#define FAST_CIBAUD 115200UL

void setup(void) {
	Serial.begin(INIT_CIBAUD); // start with low speed
	// setup ESP8266
	// 
        // crank up baud rate
        Serial.print(F("AT+CIBAUD=\""));
        Serial.print(FAST_CIBAUD);
        Serial.print(F("\""));
        Serial.print(LINE_TERM);
        delay(50); // wait a bit so stuff has been sent out
        Serial.begin(FAST_CIBAUD); // switch to high speed
	// join access point
        Serial.print(F("AT+CWJAP=\""));
	Serial.print(EP_SSID); 
        Serial.print(F("\",\""));
        Serial.print(EP_PASS);
        Serial.print(F("\""));
        Serial.print(LINE_TERM);
        delay(2000);
        // set static IP address
        Serial.print(F("AT+CISTAP=\""));
        Serial.print(STATIC_IP);
        Serial.print(F("\""));
        Serial.print(LINE_TERM);
}

void loop(void) {

}
