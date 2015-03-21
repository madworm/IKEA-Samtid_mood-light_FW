#define ESP_SSID (F("test-ssid"))
#define ESP_PASS (F("test-pass"))
#define ESP_STATIC_IP (F("192.168.1.8"))
#define ESP_LINE_TERM (F("\r\n"))
#define ESP_INIT_CIBAUD 9600UL
#define ESP_FAST_CIBAUD 115200UL

void setup(void) {
	increase_ESP8266_baud_rate(); // this may only be needed once or not at all

        Serial.begin(ESP_FAST_CIBAUD); 
	
	// join access point
        Serial.print(F("AT+CWJAP=\""));
	Serial.print(ESP_SSID); 
        Serial.print(F("\",\""));
        Serial.print(ESP_PASS);
        Serial.print(F("\""));
        Serial.print(ESP_LINE_TERM);
        delay(2000);
        
	// set static IP address
        Serial.print(F("AT+CISTAP=\""));
        Serial.print(ESP_STATIC_IP);
        Serial.print(F("\""));
        Serial.print(ESP_LINE_TERM);
}

void loop(void) {

}

void increase_ESP8266_baud_rate(void) {
        Serial.begin(ESP_INIT_CIBAUD); // start with low speed
        Serial.print(F("AT+CIBAUD=\""));
        Serial.print(ESP_FAST_CIBAUD);
        Serial.print(F("\""));
        Serial.print(ESP_LINE_TERM);
        delay(50); // wait a bit so stuff has been sent out
}

