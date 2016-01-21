#include <user_config.h>
#include "application.h"
#include "application.h"
#include <SmingCore/SmingCore.h>
#include <Libraries/Adafruit_SSD1306/Adafruit_SSD1306.h>
#include <Libraries/JPEGDecoder/JPEGDecoder.h>

#define SDA_PIN 13
#define SCL_PIN 12
#define WIFI "wifi-2-32.jpg"
#define DISCONNECTED_WIFI "wifi-3-32.jpg"
#define CONNECTED_WIFI "wifi-32.jpg"
#define SUN "sun-2-256.jpg"
Adafruit_SSD1306 display(16);

void drawPixelDelegate(int x, int y, int color) {
	if(display.height()>=y and display.width() >= x ) {
		// crude way of converting gray scale to black and white.
		// jpgs were chosen to still appear nicely
		display.drawPixel(x,y,color>128?WHITE:BLACK);
	}

}
void init() {
	spiffs_mount(); // Mount file system, in order to work with files

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial
	WifiStation.enable(false);
	System.onReady(startWork);
}

void startWork() {
	Wire.pins(SCL_PIN, SDA_PIN);
	display.begin(SSD1306_SWITCHCAPVCC);
	display.clearDisplay();
	display.setTextColor(WHITE);
	display.setTextSize(1);
	display.print("Drawing jpg!");
	display.display();

	JpegDec.init(&drawPixelDelegate);

	//display.showHeader("Starting");
	JpegDec.display(DISCONNECTED_WIFI,0,32);
	JpegDec.display(CONNECTED_WIFI,34,32);
	JpegDec.display(WIFI,68,32);
	// Reducing the file isn't supported for now...
	// JpegDec.setReduce(true);
	//JpegDec.display(SUN,0,38);
	display.display();

}
