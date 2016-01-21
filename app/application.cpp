#include <user_config.h>
#include "application.h"
#include "application.h"
#include <SmingCore/SmingCore.h>
#include <Libraries/Adafruit_SSD1306/Adafruit_SSD1306.h>
#include <Libraries/JPEGDecoder/JPEGDecoder.h>
#include <Libraries/image/image.h>
#define SDA_PIN 13
#define SCL_PIN 12
#define WIFI "wifi-2-32.jpg"
#define DISCONNECTED_WIFI "wifi-3-32.jpg"
#define CONNECTED_WIFI "wifi-32.jpg"

#define YELLOW_WIFI_GIF "yellow-wifi-32.gif"
#define YELLOW_WIFI_JPG "yellow-wifi-32.jpg"
#define YELLOW_WIFI_PNG "yellow-wifi-32.png"
Adafruit_SSD1306 display(16);
Timer refresh_timer;
int page = 0;

unsigned long elapse_start = micros();
//#define START_ELAPSED(operation) { elapse_start = micros(); Serial.printf("Begin operation %s",operation); Serial.printf("Free Heap: %d\r\n", system_get_free_heap_size()); 	system_print_meminfo();}
//#define END_ELAPSED(operation,detail) { unsigned long end = micros(); unsigned long delta = end - elapse_start;  Serial.printf("%s/%s rendering time: %i\n",operation,detail ,delta); Serial.printf("Free Heap: %d\r\n", system_get_free_heap_size()); 	system_print_meminfo();}
#define START_ELAPSED(operation) ;
#define END_ELAPSED(operation,detail) ;

void drawPixelDelegate(int x, int y, int color) {
	if(display.height()>=y and display.width() >= x ) {
		// crude way of converting gray scale to black and white.
		// jpgs were chosen to still appear nicely
		display.drawPixel(x,y,color>254?WHITE:BLACK);
	}

}
bool image_draw(String filename, int xPos, int yPos){
	int x,y,n;
	unsigned char *pCurdata;
//	Serial.printf("image_draw called - Filename [%s]\n",filename.c_str());

	unsigned char maxColor=0;
	unsigned char minColor=255;
	unsigned char *data = image_load(filename, &x, &y, &n, 1);
	if(data){
		pCurdata = data;
		//    // ... process data if not NULL ...
//		String message = String("Width is ")+x+" and height is "+y+" and there were "+n+" components per pixel";
//		Serial.printf(message.c_str());

			for(int curY=0;curY<y;curY++){
				bool bNewline=true;
				for(int curX=0;curX<x;curX++){
//					if(bNewline)
//						Serial.printf(" \n(%i,%i)",curX,curY);
//					bNewline = false;
				// ... x = width, y = height, n = # 8-bit components per pixel ...
				//    // ... replace '0' with '1'..'4' to force that many components per pixel
				//    // ... but 'n' will always be the number that it would have been if you said 0

//				Serial.printf("%#02x",data[curX*curY]);

				drawPixelDelegate(curX+xPos,curY+yPos,*pCurdata);
				if( *pCurdata>maxColor) maxColor  = *pCurdata;
				if( *pCurdata<minColor) minColor = * pCurdata;
				pCurdata+=1;
			}
		}
//		Serial.printf("Before free - Free Heap: %d\r\n", system_get_free_heap_size());
//		Serial.printf("freeing up memory. Max color val was %i and min color was %i\n",maxColor,minColor);
//		Serial.printf("After free - Free Heap: %d\r\n", system_get_free_heap_size());
		stbi_image_free(data);
	}
	else
	{
		return false;
	}

	return true;

}
void DrawImages(){

	display.clearDisplay();
	display.setTextColor(BLACK);
	display.fillRect(0,0,132,16,WHITE);
	display.setCursor(0,3);
	display.display();
	switch(page){
	case 0:
		display.println("JPEGDecoder");
		JpegDec.display(YELLOW_WIFI_JPG,0,32);
		JpegDec.display(YELLOW_WIFI_JPG,34,32);
		JpegDec.display(YELLOW_WIFI_JPG,68,32);
		break;
	default:
		display.println("stbi_image-multiformat");
		image_draw(YELLOW_WIFI_JPG,0,32);
//		image_draw(YELLOW_WIFI_JPG,34,32);
//		image_draw(YELLOW_WIFI_JPG,68,32);
		//page=0;
		break;
	}

	display.display();
	refresh_timer.startOnce();

}
void startWork() {
	Wire.pins(SCL_PIN, SDA_PIN);
	display.begin(SSD1306_SWITCHCAPVCC);
	display.setTextColor(WHITE);
	display.fillScreen(BLACK);
	display.display();
	JpegDec.init(&drawPixelDelegate);
	display.setTextSize(1);
	page=1;
	DrawImages();
	refresh_timer.initializeMs(3000, DrawImages).startOnce();
}
void init() {
	spiffs_mount(); // Mount file system, in order to work with files
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	system_print_meminfo();
	Serial.systemDebugOutput(true); // Enable debug output to serial
	WifiStation.enable(false);
	WifiAccessPoint.enable(true);
	System.onReady(startWork);
}
