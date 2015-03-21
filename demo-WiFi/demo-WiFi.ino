#define SSID test-ssid
#define PASS test-pass
#define STATIC_IP 192.168.1.8
#define LINE_TERM \r\n

void setup(void) {
	Serial.begin(115200);
	// setup ESP8266
	// 
	// some stuff missing
	Serial.print("AT+CWJAP=\"SSID\",\"PASS\"LINE_TERM"); 
	Serial.print("AT+CISTAP=\"STATIC_IP\"LINE_TERM");
}

void loop(void) {

}
