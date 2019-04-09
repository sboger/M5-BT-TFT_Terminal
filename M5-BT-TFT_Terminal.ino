#include <M5Stack.h>
#include "BluetoothSerial.h"
#include "WiFi.h"

BluetoothSerial SerialBT;

// The scrolling area setup. Must be a integral multiple of TEXT_HEIGHT
#define TEXT_HEIGHT 16 // Height of text to be printed and scrolled
#define BOT_FIXED_AREA 0 // Number of lines in bottom fixed area (lines counted from bottom of screen)
#define TOP_FIXED_AREA 16 // Number of lines in top fixed area (lines counted from top of screen)
#define YMAX 240 // Bottom of screen area
// end scrolling


#define SECS_TO_SLEEP 30  // Set auto sleep time to 30 seconds
                          // Note: Will auto-awaken on input
boolean sleepMode = 0;    // Start with sleep mode on (1) or off (0).


int LcdBrightness = 80; // 50%


// The initial y coordinate of the top of the scrolling area
uint16_t yStart = TOP_FIXED_AREA;
// yArea must be a integral multiple of TEXT_HEIGHT
uint16_t yArea = YMAX-TOP_FIXED_AREA-BOT_FIXED_AREA;
// The initial y coordinate of the top of the bottom text line
uint16_t yDraw = YMAX - BOT_FIXED_AREA - TEXT_HEIGHT;

// Keep track of the drawing x coordinate
uint16_t xPos = 0;

// For the byte we read from the BT port
byte data = 0;

// Yes, we want to change color on each input line.
boolean changeColor = 1;
boolean changeTripped = 1;

// timer
long timeSinceLastUpdate = 0;

int blank[19];

void setup() {
  M5.begin();

  WiFi.mode(WIFI_OFF);
  
  M5.Power.begin();
  
  // Setup the TFT display
  M5.Lcd.init();
  M5.Lcd.setBrightness(LcdBrightness);
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(TFT_BLACK);
  
  SerialBT.begin("BLUETERM5"); //Bluetooth device name
  
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLUE);
  M5.Lcd.fillRect(0,0,320,16, TFT_BLUE);
  M5.Lcd.drawCentreString(" BLUETERM5 ",320/2,0,2);

  // Change color for scrolling zone text
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);

  // Setup scroll area
  setupScrollArea(TOP_FIXED_AREA, BOT_FIXED_AREA);

  // Zero the array
  for (byte i = 0; i<18; i++) blank[i]=0;
}


void loop(void) {

  M5.update();
  displayBatt();
  displaySleepMode(sleepMode);
  

  if (M5.BtnA.wasReleased()) {
    sleepMode = 0;
  } else if (M5.BtnA.wasReleasefor(400)) {
    sleepMode = 1;
  }

  // Y U BROKEN???
  if (M5.BtnB.wasReleased()) {
    M5.Lcd.setBrightness(++LcdBrightness);
  } else if (M5.BtnB.wasReleasefor(400)) {
    M5.Lcd.setBrightness(--LcdBrightness);
  }

  if (M5.BtnC.wasReleased()) {
    M5.Lcd.setBrightness(LcdBrightness);
    M5.Lcd.wakeup();
  } else if (M5.BtnC.wasReleasefor(400)) {
    M5.Lcd.setBrightness(0);
    M5.Lcd.sleep();    
  }

  
  while (SerialBT.available()) {
    
    M5.Lcd.setBrightness(LcdBrightness);
    M5.Lcd.wakeup();
    colorRotator();
  
    data = SerialBT.read();
    
    // If it is a CR or we are near end of line then scroll one line
    if (data == '\r' || xPos > 311) {
      xPos = 0;
      yDraw = scroll_line(); // It can take 13ms to scroll and blank 16 pixel lines
    }
    
    if (data > 31 && data < 128) {
      xPos += M5.Lcd.drawChar(data,xPos,yDraw,2);
      blank[(18+(yStart-TOP_FIXED_AREA)/TEXT_HEIGHT)%19]=xPos; // Keep a record of line lengths
    }  
  }  
  changeTripped = 1;
  if (sleepMode) {
    if (millis() - timeSinceLastUpdate > (1000L*SECS_TO_SLEEP)) {
      M5.Lcd.setBrightness(0);
      M5.Lcd.sleep();
      timeSinceLastUpdate = millis();
     }
  }
}

// ##############################################################################################
// Call this function to get get battery status
// ##############################################################################################
void displayBatt() {

  String battstat;

  if (M5.Power.isChargeFull()) {
    battstat = "FULL";
  } else if (M5.Power.isCharging()) {
    battstat = "CHRG";
  } else {
    battstat = String(M5.Power.getBatteryLevel()) + "%";
  }

  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLUE);
  M5.Lcd.drawString("["+battstat.substring(0,4)+"] ",270,1,1);
}

// ##############################################################################################
// Call this function to get get sleep mode
// ##############################################################################################
void displaySleepMode(boolean mode) {

  String sMode = (String)(mode ? "Zzz " : "NoZ ");
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLUE);
  M5.Lcd.drawString(sMode,10,1,1);
}

// ##############################################################################################
// Call this function to scroll the display one text line
// ##############################################################################################
int scroll_line() {
  int yTemp = yStart; // Store the old yStart, this is where we draw the next line
  // Use the record of line lengths to optimise the rectangle size we need to erase the top line
  M5.Lcd.fillRect(0,yStart,blank[(yStart-TOP_FIXED_AREA)/TEXT_HEIGHT],TEXT_HEIGHT, TFT_BLACK);

  // Change the top of the scroll area
  yStart+=TEXT_HEIGHT;
  // The value must wrap around as the screen memory is a circular buffer
  if (yStart >= YMAX - BOT_FIXED_AREA) yStart = TOP_FIXED_AREA + (yStart - YMAX + BOT_FIXED_AREA);
  // Now we can scroll the display
  scrollAddress(yStart);
  return  yTemp;
}

// ##############################################################################################
// Setup a portion of the screen for vertical scrolling
// ##############################################################################################
// We are using a hardware feature of the display, so we can only scroll in portrait orientation
void setupScrollArea(uint16_t tfa, uint16_t bfa) {
  M5.Lcd.writecommand(ILI9341_VSCRDEF); // Vertical scroll definition
  M5.Lcd.writedata(tfa >> 8);           // Top Fixed Area line count
  M5.Lcd.writedata(tfa);
  M5.Lcd.writedata((YMAX-tfa-bfa)>>8);  // Vertical Scrolling Area line count
  M5.Lcd.writedata(YMAX-tfa-bfa);
  M5.Lcd.writedata(bfa >> 8);           // Bottom Fixed Area line count
  M5.Lcd.writedata(bfa);
}

// ##############################################################################################
// Setup the vertical scrolling start address pointer
// ##############################################################################################
void scrollAddress(uint16_t vsp) {
  M5.Lcd.writecommand(ILI9341_VSCRSADD); // Vertical scrolling pointer
  M5.Lcd.writedata(vsp>>8);
  M5.Lcd.writedata(vsp);
}


// ##############################################################################################
// flip the color for output
// ##############################################################################################
void colorRotator() {
  if (changeTripped) {
    if (changeColor) { 
      M5.Lcd.setTextColor(TFT_CYAN, TFT_BLACK);
      changeColor=0;
    } else {
      M5.Lcd.setTextColor(TFT_MAGENTA, TFT_BLACK);
      changeColor=1;
    }
    changeTripped=0;
  }
}
