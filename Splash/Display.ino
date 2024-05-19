// The display is layed out as follows:
// <wallet balance> sat(s)                            QRCODE
// -------------------------------------------------- QRCODE
// <amount> sat(s): comment1                          QRCODE
// <amount> sat(s): comment2 comment2 comment2        status
// comment2                                           status
// <amount> sat(s): comment3                          status
// <fiatbalance> <currency> (<fiatprice> <currency)
//

// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>

#define MAX_DISPLAY_BUFFER_SIZE 65536ul
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
// Both display drivers are compiled in, and the right one is detected and used at runtime:
//GxEPD2_BW<GxEPD2_213_B74, MAX_HEIGHT(GxEPD2_213_B74)> display1(GxEPD2_213_B74(/*CS=*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));
GxEPD2_BW<GxEPD2_213_BN, MAX_HEIGHT(GxEPD2_213_BN)> display1(GxEPD2_213_BN(/*CS=*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));
GxEPD2_BW<GxEPD2_266_BN, MAX_HEIGHT(GxEPD2_266_BN)> display2(GxEPD2_266_BN(/*CS=*/ 5, /*DC=*/ 19, /*RST=*/ 4, /*BUSY=*/ 34));

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

int smallestFontHeight = 14;

int statusAreaVoltageHeight = -1; // this value is cached after it's calculated so it can be reused later to updated only the voltage

int blackBackgroundVerticalMargin=2;
int blackBackgroundHorizontalMargin=2;

String lines[10];
int nroflines = 0;

int displayToUse = -NOT_SPECIFIED;

/* Detecting the display works by timing the clearScreen operation.
09:08:03.074 -> init operation took 23ms
09:08:06.103 -> rotation operation took 0ms
09:08:11.426 -> _Update_Full : 2290999
09:08:11.426 -> clearScreen operation took 2339ms => right display
09:08:14.452 -> init operation took 23ms
09:08:17.446 -> rotation operation took 0ms
09:08:20.507 -> _PowerOn : 3
09:08:20.507 -> _Update_Full : 1
09:08:20.507 -> clearScreen operation took 60ms => wrong display
 */
void setup_display() {
  display1.init(115200, true, 2, false);
  long beforeTime = millis();
  display1.clearScreen();
  Serial.println("clearScreen operation took " + String(millis() - beforeTime) + "ms");
  if ((millis() - beforeTime) > 1500) {
    Serial.println("clearScreen took a long time so found the right display: 1!");
    displayToUse = DISPLAY_TYPE_213DEPG;
    display1.setRotation(1); // display is used in landscape mode
    u8g2Fonts.begin(display1); // connect u8g2 procedures to Adafruit GFX
  } else {
    display2.init(115200, true, 2, false);
    displayToUse = DISPLAY_TYPE_266DEPG;
    display2.setRotation(1); // display is used in landscape mode
    u8g2Fonts.begin(display2); // connect u8g2 procedures to Adafruit GFX
  }

  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
}

int getDisplayToUse() {
  return displayToUse;
}

void setPartialWindow(int x, int y, int h, int w) {
  if (displayToUse == DISPLAY_TYPE_213DEPG) {
    display1.setPartialWindow(x, y, h, w);
  } else if (displayToUse == DISPLAY_TYPE_266DEPG) {
    display2.setPartialWindow(x, y, h, w);
  } else {
    Serial.println("ERROR: there's no display to use detected!");
  }
}

void displayFirstPage() {
  if (displayToUse == DISPLAY_TYPE_213DEPG) {
    display1.firstPage();
  } else if (displayToUse == DISPLAY_TYPE_266DEPG) {
    display2.firstPage();
  } else {
    Serial.println("ERROR: there's no display to use detected!");
  }
}

bool displayNextPage() {
  if (displayToUse == DISPLAY_TYPE_213DEPG) {
    return display1.nextPage();
  } else if (displayToUse == DISPLAY_TYPE_266DEPG) {
    return display2.nextPage();
  } else {
    Serial.println("ERROR: there's no display to use detected!");
  }
  return false;
}

void displayFillRect(int x, int y, int w, int h, int color) {
  if (displayToUse == DISPLAY_TYPE_213DEPG) {
    display1.fillRect(x,y,w,h,color);
  } else if (displayToUse == DISPLAY_TYPE_266DEPG) {
    display2.fillRect(x,y,w,h,color);
  } else {
    Serial.println("ERROR: there's no display to use detected!");
  }
}

void displayDrawImage(const unsigned char logo [], int posX, int posY, int sizeX, int sizeY, bool toggle) {
  if (displayToUse == DISPLAY_TYPE_213DEPG) {
    display1.drawImage(logo, posX, posY, sizeX, sizeY, toggle);
  } else if (displayToUse == DISPLAY_TYPE_266DEPG) {
    display2.drawImage(logo, posX, posY, sizeX, sizeY, toggle);
  } else {
    Serial.println("ERROR: there's no display to use detected!");
  }
}

int displayHeight() {
  if (displayToUse == DISPLAY_TYPE_213DEPG) {
    return 122;
  } else if (displayToUse == DISPLAY_TYPE_266DEPG) {
    return 152;
  } else {
    Serial.println("ERROR: there's no display to use detected!");
  }
  return 0;
}

int displayWidth() {
  if (displayToUse == DISPLAY_TYPE_213DEPG) {
    return 250;
  } else if (displayToUse == DISPLAY_TYPE_266DEPG) {
    return 296;
  } else {
    Serial.println("ERROR: there's no display to use detected!");
  }
  return 0;
}

// size 0 = smallest font (8pt)
// size 1 = 10pt
// size 2 = 12pt
// size 3 = 14pt
// size 4 = 18pt
// size 5 = 24pt
void setFont(int fontSize) {
  if (fontSize < 0) {
    Serial.println("ERROR: font size " + String(fontSize) + " is not supported, setting min size");
    u8g2Fonts.setFont(u8g2_font_helvR08_te);
  } else if (fontSize == 0) {
    u8g2Fonts.setFont(u8g2_font_helvR08_te);
  } else if (fontSize == 1) {
    u8g2Fonts.setFont(u8g2_font_helvR10_te);
  } else if (fontSize == 2) {
    u8g2Fonts.setFont(u8g2_font_helvR12_te);
  } else if (fontSize == 3) {
    u8g2Fonts.setFont(u8g2_font_helvR14_te);
  } else if (fontSize == 4) {
    u8g2Fonts.setFont(u8g2_font_helvR18_te);
  } else if (fontSize == 5) {
    u8g2Fonts.setFont(u8g2_font_helvR24_tf);
  } else {
    Serial.println("ERROR: font size " + String(fontSize) + " is not supported, setting max size");
    u8g2Fonts.setFont(u8g2_font_helvR24_tf);
  }
}

// find the max length that fits the width
int fitMaxText(String text, int maxWidth) {
  //long startTime = millis();
  int maxLength = 0;
  int16_t x1, y1;
  uint16_t w, h;

  // first get height of one big character
  //display.getTextBounds("$", 0, 0, &x1, &y1, &w, &h);
  w = u8g2Fonts.getUTF8Width("$");
  h = u8g2Fonts.getFontAscent()-u8g2Fonts.getFontDescent();
  Serial.println("Got big character bounds: " + String(x1) + "," + String(y1) + ","+ String(w) + "," + String(h) + " for text: $");
  uint16_t maxHeight = h * 1.5; // ensure it's really big, but smaller than 2 lines
  //Serial.println("maxHeight = " + String(maxHeight));
  h = 0;

  while (maxLength < text.length() && h < maxHeight && w < maxWidth) {
    String textToFit = text.substring(0, maxLength+2); // end is exclusive
    w = u8g2Fonts.getUTF8Width(textToFit.c_str());
    h = u8g2Fonts.getFontAscent()-u8g2Fonts.getFontDescent();
    //Serial.println("Got text bounds: " + String(x1) + "," + String(y1) + ","+ String(w) + "," + String(h) + " for text: " + textToFit);
    maxLength++;
  }

  //Serial.println("Max text length that fits: " + String(maxLength) + " calculated in " + String(millis()-startTime) + "ms.");
  return maxLength;
}

int drawLines(String stringArray[], int nrOfItems, int startX, int endX, int startY, bool invert, bool alignRight) {
  int yPos = startY;
  for (int linenr=0;linenr<nroflines;linenr++) {
    if (!alignRight) {
      yPos += drawLine(lines[linenr],startX,yPos,invert,alignRight);
    } else {
      yPos += drawLine(lines[linenr],endX,yPos,invert,alignRight);
    }
  }
  return yPos;
}

// xPos,yPos is the top left of the line (in case no alignRight) or top right of the line (in case of alignRight)
// returns line height
int drawLine(String line, int xPos, int yPos, bool invert, bool alignRight) {
  int w = u8g2Fonts.getUTF8Width(line.c_str());
  int h = u8g2Fonts.getFontAscent()-u8g2Fonts.getFontDescent();
  Serial.println("Drawing text " + String(line) + " at (" + String(xPos) + "," + String(yPos) + ") with size "+ String(w) + "x"+ String(h));
  if (!alignRight) {
    Serial.println("u8g2Fonts.setCursor(" + String(xPos) + "," + String(yPos + h) + ")");
    u8g2Fonts.setCursor(xPos, yPos + h); // bottom of the line
  } else {
    Serial.println("u8g2Fonts.setCursor(" + String(xPos-w) + "," + String(yPos + h) + ")");
    u8g2Fonts.setCursor(xPos-w, yPos + h); // bottom of the line
  }
  if (!invert) {
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
  } else {
    Serial.println("Filling rectangle for inverted text from (" + String(xPos) + "," + String(yPos) + ") of size " + String(w) + "x" + String(h));
    displayFillRect(xPos-blackBackgroundHorizontalMargin, yPos, w+blackBackgroundHorizontalMargin*2, h+blackBackgroundVerticalMargin*2, GxEPD_BLACK);
    u8g2Fonts.setForegroundColor(GxEPD_WHITE);
    u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
  }
  Serial.println("Displaying line: " + line);
  u8g2Fonts.println(line);
  return h;
}


int displayFit(String text, int startX, int startY, int endX, int endY, int fontSize) {
    return displayFit(text, startX, startY, endX, endY, fontSize, false);
}

int displayFit(String text, int startX, int startY, int endX, int endY, int fontSize, bool invert) {
    return displayFit(text, startX, startY, endX, endY, fontSize, invert, false);
}

int displayFit(String text, int startX, int startY, int endX, int endY, int fontSize, bool invert, bool alignRight) {
    return displayFit(text, startX, startY, endX, endY, fontSize, invert, alignRight, true);
}

// Try to fit a String into a rectangle, including the borders.
// bool bold == true means black background, white text
// returns: the y position after fitting the text
int displayFit(String text, int startXbig, int startYbig, int endXbig, int endYbig, int fontSize, bool invert, bool alignRight, bool drawIt) {
  long startTime = millis();
  bool debugDisplayFit = false;

  Serial.println("displayFit " + text + " of length: " + String(text.length()) + " from (" + String(startXbig) + "," + String(startYbig) + ") to (" + String(endXbig) + "," + String(endYbig) + ") with max fontSize " + String(fontSize));

  if (text.length() == 0) {
    Serial.println("Aborting displayFit due to zero length text.");
    return startYbig;
  }

  int16_t x1, y1;
  uint16_t w, h;
  int startX = startXbig;
  int startY = startYbig;
  int endX = endXbig;
  int endY = endYbig;
  int invertOffsetXbefore = blackBackgroundHorizontalMargin;
  int invertOffsetYbefore = 0;
  int invertOffsetXafter = blackBackgroundHorizontalMargin;
  int invertOffsetYafter = blackBackgroundVerticalMargin;
  if (invert) {
    // black rectangle is slightly bigger than the text; from (-2,-1) inclusive until (+2,+2) inclusive
    startX = startXbig + invertOffsetXbefore;
    startY = startYbig + invertOffsetYbefore;
    endX = endXbig - invertOffsetXafter;
    endY = endYbig - invertOffsetYafter;
  }

  // Don't go past the end of the display and remember pixels start from zero, so [0,max-1]
  endX = min(displayWidth()-1,endX);
  endY = min(displayHeight()-1,endY);

  int yPos;

  if (drawIt) {
    Serial.println("Setting partial window: (" + String(startXbig) + "," + String(startYbig) + " with size " + String(endXbig-startXbig+1) + "x" + String(endYbig-startYbig+1));
    setPartialWindow(startXbig, startYbig, endXbig-startXbig+1, endYbig-startYbig+1);
  }
  while (fontSize > 0) {
    nroflines = 0;
    setFont(fontSize);

    yPos = startY;
    int textPos = 0;
    while (textPos < text.length()) {
      // Try to fit everything that still needs displaying:
      String textWithoutAlreadyPrintedPart = text.substring(textPos);
      int chars = fitMaxText(textWithoutAlreadyPrintedPart, endX);

      String textLine = text.substring(textPos, textPos+chars);
      if (debugDisplayFit) Serial.println("first line that fits: " + textLine);
      lines[nroflines] = textLine;
      nroflines++;

      h = u8g2Fonts.getFontAscent()-u8g2Fonts.getFontDescent();

      textPos += chars;
      yPos += h;
    }
    if (debugDisplayFit) Serial.println("After simulating the text, yPos = " + String(yPos) + " while endY = " + String(endY));

    // Check if the entire text fit:
    if (yPos <= endY) {
      if (debugDisplayFit) Serial.println("yPos (" + String(yPos) + ") <= endY (" + String(endY) + ") so fontSize " + String(fontSize) + " fits!");
      break; // exit the fontSize loop because it fits
    } else {
      if (debugDisplayFit) Serial.println("fontSize " + String(fontSize) + " did not fit so trying smaller...");
      fontSize--;
    }
  }

  if (drawIt) {
    // finally print the array
    displayFirstPage();
    do {
      yPos = drawLines(lines, nroflines, startX, endX, startY, invert, alignRight);
    } while (displayNextPage());
  } else {
    yPos = drawLines(lines, nroflines, startX, endX, startY, invert, alignRight);
  }
  if (debugDisplayFit) Serial.println("After writing the text, yPos = " + String(yPos) + " while endY = " + String(endY));

  if (debugDisplayFit) Serial.println("displayFit returning yPos = " + String(yPos) + " after runtime of " + String(millis() - startTime) + "ms."); // takes around 1700ms
  return yPos;
}

void fastClearScreen() {
  // display.clearScreen(); // slow
  setPartialWindow(0, 0, displayWidth(), displayHeight()); // this clear the display
  displayFirstPage();
  do {
    displayFillRect(0, 0, displayWidth(), displayHeight(), GxEPD_WHITE);
  } while (displayNextPage());
}

void showLogo(const unsigned char logo [], int sizeX, int sizeY, int posX, int posY) {
  fastClearScreen();
  displayDrawImage(logo, posX, posY, sizeX, sizeY, false);
}

void displaySetup() {
  displayFit("Setup at www.lightningpiggy.com", 0, displayHeight()-smallestFontHeight-2, displayWidth(), displayHeight(), 1);
}
