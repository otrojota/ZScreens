#ifndef ZSPIFFSJPEG_h
#define ZSPIFFSJPEG_h

#include <JPEGDecoder.h>
#include "FS.h"
#include "SPIFFS.h"
#include "ZScreens.h"

void drawSPIFFSJpeg(TFT_eSPI &tft, const char *filename, int xpos, int ypos);

class ZSPIFFSJPEG: public ZControl {
  public:
    String path;
    ZSPIFFSJPEG(const char *_id, JsonObject def);
    virtual void draw();
};

//// ******* Implementation in header. Only when #included

ZSPIFFSJPEG::ZSPIFFSJPEG(const char *_id, JsonObject def): ZControl(_id, def) {
  if (def.containsKey("path")) path = String((const char *)def["path"]);
  else path = "";
}
void ZSPIFFSJPEG::draw() {
  clear();
  if (!isVisible() || !path.length()) {
    ZControl::draw();
    return;  
  }
  if (path.length()) {
    drawSPIFFSJpeg(*(app->tft), path.c_str(), zx(), zy());
  }
  
  ZControl::draw();
}


ZControl *ZSPIFFSJPEGFactory(const char *id, JsonObject def) {
  return new ZSPIFFSJPEG(id, def);
}
static_block {
  ZControl::registerFactory("ZSPIFFSJPEG", ZSPIFFSJPEGFactory);  
}

void jpegSPIFFSRender(TFT_eSPI &tft, int xpos, int ypos);

//####################################################################################################
// Draw a JPEG on the TFT pulled from SPIFFS Flash memory
//####################################################################################################
// xpos, ypos is top left corner of plotted image
void drawSPIFFSJpeg(TFT_eSPI &tft, const char *filename, int xpos, int ypos) {
  fs::File jpegFile = SPIFFS.open( filename, "r");
  if ( !jpegFile ) {
    Serial.print("ERROR: SPIFFS File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }
  boolean decoded = JpegDec.decodeFsFile(jpegFile);  // Pass the SD file handle to the decoder,
  if (decoded) {
    jpegSPIFFSRender(tft, xpos, ypos);
  }
  else {
    Serial.println("Jpeg file format not supported!");
  }
}

//####################################################################################################
// Draw a JPEG on the TFT, images will be cropped on the right/bottom sides if they do not fit
//####################################################################################################
// This function assumes xpos,ypos is a valid screen coordinate. For convenience images that do not
// fit totally on the screen are cropped to the nearest MCU size and may leave right/bottom borders.
void jpegSPIFFSRender(TFT_eSPI &tft, int xpos, int ypos) {
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  bool swapBytes = tft.getSwapBytes();
  tft.setSwapBytes(true);
  
  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = jpg_min(mcu_w, max_x % mcu_w);
  uint32_t min_h = jpg_min(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // Fetch data from the file, decode and display
  while (JpegDec.read()) {    // While there is more data in the file
    pImg = JpegDec.pImage ;   // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)

    // Calculate coordinates of top left corner of current MCU
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++)
      {
        p += mcu_w;
        for (int w = 0; w < win_w; w++)
        {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }

    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;

    // draw image MCU block only if it will fit on the screen
    if (( mcu_x + win_w ) <= tft.width() && ( mcu_y + win_h ) <= tft.height())
      tft.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
    else if ( (mcu_y + win_h) >= tft.height())
      JpegDec.abort(); // Image has run off bottom of screen so abort decoding
  }

  tft.setSwapBytes(swapBytes);

}


#endif
