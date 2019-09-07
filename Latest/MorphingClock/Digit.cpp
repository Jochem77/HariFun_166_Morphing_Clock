#include "Digit.h"
#include "Ticker.h"

const byte sA = 0;
const byte sB = 1;
const byte sC = 2;
const byte sD = 3;
const byte sE = 4;
const byte sF = 5;
const byte sG = 6;
const uint16_t height = 31;
const uint16_t width = 63;
//
//  A A
// F   B
// F   B
//  G G
// E   C
// E   C
//  D D  
//
byte digitBits[] = {
  B11111100, // 0 ABCDEF--
  B01100000, // 1 -BC-----
  B11011010, // 2 AB-DE-G-
  B11110010, // 3 ABCD--G-
  B01100110, // 4 -BC--FG-
  B10110110, // 5 A-CD-FG-
  B10111110, // 6 A-CDEFG-
  B11100000, // 7 ABC-----
  B11111110, // 8 ABCDEFG-
  B11110110, // 9 ABCD_FG-
};

//byte sunBitmap[] {
//  B100100100,
//  B010001000,
//  B001110000,
//  B101110100,
//  B001110000,
//  B010001000,
//  B100100100
//};

uint16_t black;

void Digit::MorphDigit(){
  switch (_value) {
    case 2: Morph2(); break;
    case 3: Morph3(); break;
    case 4: Morph4(); break;
    case 5: Morph5(); break;
    case 6: Morph6(); break;
    case 7: Morph7(); break;
    case 8: Morph8(); break;
    case 9: Morph9(); break;
    case 0: Morph0(); break;
    case 1: Morph1(); break;
  }
  _step++;
  if (_step>segWidth+1)
  {
    _step =0;
    _doMorph=false;
  }
  return;
}
bool Digit::doMorph() {
  return _doMorph;
}

void Digit::Morph1() {
  // Zero or two to One
  //for (int i = 0; i <= (segWidth + 1); i++)
  //int i = 0;//tMorph1.getRunCounter()-1;
  {
    // Move E left to right
    drawLine(0 + _step - 1, 1, 0 + _step - 1, segHeight, black);
    drawLine(0 + _step, 1, 0 + _step, segHeight, _color);

    // Move F left to right
    drawLine(0 + _step - 1, segHeight * 2 + 1, 0 + _step - 1, segHeight + 2, black);
    drawLine(0 + _step, segHeight * 2 + 1, 0 + _step, segHeight + 2, _color);

    // Gradually Erase A, G, D
    drawPixel(1 + _step, segHeight * 2 + 2, black); // A
    drawPixel(1 + _step, 0, black); // D
    drawPixel(1 + _step, segHeight + 1, black); // G

    //delay(animSpeed);
  }
}

Digit::Digit(PxMATRIX* d, byte value, uint16_t xo, uint16_t yo, uint16_t color, int height, int width) {
  _display = d;
  _value = value;
  xOffset = xo;
  yOffset = yo;
  _color = color;
  segHeight=height;
  segWidth=width;

}

byte Digit::Value() {
  return _value;
}
void Digit::drawPixel(uint16_t x, uint16_t y, uint16_t c)
{
  _display->drawPixel(xOffset + x, height - (y + yOffset), c);
}

void Digit::drawLine(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t c)
{
  _display->drawLine(xOffset + x, height - (y + yOffset), xOffset + x2, height - (y2 + yOffset), c);
}

void Digit::drawFillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c)
{
  _display->fillRect(xOffset + x, height - (y + yOffset), w,h, c);
}

void Digit::DrawColon(uint16_t c)
{
  // Colon is drawn to the left of this digit
  drawFillRect(-3, segHeight-1, 2,2, c);
  drawFillRect(-3, segHeight+1+3, 2,2, c);
}

void Digit::drawSeg(byte seg)
{
  switch (seg) {
    case sA: drawLine(1, segHeight * 2 + 2, segWidth, segHeight * 2 + 2, _color); break;
    case sB: drawLine(segWidth + 1, segHeight * 2 + 1, segWidth + 1, segHeight + 2, _color); break;
    case sC: drawLine(segWidth + 1, 1, segWidth + 1, segHeight, _color); break;
    case sD: drawLine(1, 0, segWidth, 0, _color); break;
    case sE: drawLine(0, 1, 0, segHeight, _color); break;
    case sF: drawLine(0, segHeight * 2 + 1, 0, segHeight + 2, _color); break;
    case sG: drawLine(1, segHeight + 1, segWidth, segHeight + 1, _color); break;
  }
}


void Digit::Draw(byte value) {
  byte pattern = digitBits[value];
  if (bitRead(pattern, 7)) drawSeg(sA);
  if (bitRead(pattern, 6)) drawSeg(sB);
  if (bitRead(pattern, 5)) drawSeg(sC);
  if (bitRead(pattern, 4)) drawSeg(sD);
  if (bitRead(pattern, 3)) drawSeg(sE);
  if (bitRead(pattern, 2)) drawSeg(sF);
  if (bitRead(pattern, 1)) drawSeg(sG);
  _value = value;
}

void Digit::Morph2() {
  // TWO
  //for (int i = 0; i <= segWidth; i++)
  {
    if (_step < segWidth) {
      drawPixel(segWidth - _step, segHeight * 2 + 2, _color);
      drawPixel(segWidth - _step, segHeight + 1    , _color);
      drawPixel(segWidth - _step, 0                , _color);
    }
    if (_step <= segWidth) {
      drawLine(segWidth + 1 - _step, 1, segWidth + 1 - _step, segHeight, black);
      drawLine(segWidth - _step    , 1, segWidth - _step    , segHeight, _color);
      //delay(animSpeed);
    }
  }
}

void Digit::Morph3() {
  // THREE
  //for (int i = 0; i <= segWidth; i++)
  {
    if (_step <= segWidth) {
      drawLine(0 + _step, 1, 0 + _step, segHeight, black); //E Erase
      drawLine(1 + _step, 1, 1 + _step, segHeight, _color);// C draw
      //delay(animSpeed);
    }
  }
}

void Digit::Morph4() {
  // FOUR
  //for (int i = 0; i < segWidth; i++)
  {
    if (_step < segWidth) {
      drawPixel(segWidth - _step, segHeight * 2 + 2        , black); // Erase A
      drawPixel(0               , segHeight * 2 + 1 - _step, _color); // Draw as F
      drawPixel(1 + _step       , 0                        , black); // Erase D
      //delay(animSpeed);
    }
  }
}

void Digit::Morph5() {
  // FIVE
  //for (int i = 0; i < segWidth; i++)
  {
    if (_step < segWidth) {
      drawPixel(segWidth + 1, segHeight + 2 + _step, black); // Erase B
      drawPixel(segWidth - _step, segHeight * 2 + 2, _color); // Draw as A
      drawPixel(segWidth - _step, 0, _color); // Draw D
      //delay(animSpeed);
    }
  }
}

void Digit::Morph6() {
  // SIX
  //for (int i = 0; i <= segWidth; i++)
  {
    if (_step <= segWidth) {
      // Move C right to left
      drawLine(segWidth - _step, 1, segWidth - _step, segHeight, _color);
      if (_step > 0) drawLine(segWidth - _step + 1, 1, segWidth - _step + 1, segHeight, black);
      //delay(animSpeed);
    }
  }
}

void Digit::Morph7() {
  // SEVEN
  //for (int i = 0; i <= (segWidth + 1); i++)
  {
    // Move E left to right
    drawLine(0 + _step - 1, 1, 0 + _step - 1, segHeight, black);
    drawLine(0 + _step, 1, 0 + _step, segHeight, _color);

    // Move F left to right
    drawLine(0 + _step - 1, segHeight * 2 + 1, 0 + _step - 1, segHeight + 2, black);
    drawLine(0 + _step, segHeight * 2 + 1, 0 + _step, segHeight + 2, _color);

    // Erase D and G gradually
    drawPixel(1 + _step, 0, black); // D
    drawPixel(1 + _step, segHeight + 1, black); // G
    //delay(animSpeed);
  }
}

void Digit::Morph8() {
  // EIGHT
  //for (int i = 0; i <= segWidth; i++)
  {
    if (_step <= segWidth) {
      // Move B right to left
      drawLine(segWidth - _step, segHeight * 2 + 1, segWidth - _step, segHeight + 2, _color);
      if (_step > 0) drawLine(segWidth - _step + 1, segHeight * 2 + 1, segWidth - _step + 1, segHeight + 2, black);

      // Move C right to left
      drawLine(segWidth - _step, 1, segWidth - _step, segHeight, _color);
      if (_step > 0) drawLine(segWidth - _step + 1, 1, segWidth - _step + 1, segHeight, black);

      // Gradually draw D and G
      if (_step < segWidth) {
        drawPixel(segWidth - _step, 0, _color); // D
        drawPixel(segWidth - _step, segHeight + 1, _color); // G
      }
    }
    //delay(animSpeed);
    
  }
}

void Digit::Morph9() {
  // NINE
  //for (int i = 0; i <= (segWidth + 1); i++)
  {
    // Move E left to right
    drawLine(0 + _step - 1, 1, 0 + _step - 1, segHeight, black);
    drawLine(0 + _step, 1, 0 + _step, segHeight, _color);
    //delay(animSpeed);
  }
}

void Digit::Morph0() {
  // ZERO
  //for (int i = 0; i <= segWidth; i++)
  {
    if (_step <= segWidth) {
      if (_prevvalue==1) { // If 1 to 0, slide B to F and E to C  
        // slide B to F 
        drawLine(segWidth - _step, segHeight * 2+1 , segWidth - _step, segHeight + 2, _color);
        if (_step > 0) drawLine(segWidth - _step + 1, segHeight * 2+1, segWidth - _step + 1, segHeight + 2, black);
  
        // slide E to C
        drawLine(segWidth - _step, 1, segWidth - _step, segHeight, _color);
        if (_step > 0) drawLine(segWidth - _step + 1, 1, segWidth - _step + 1, segHeight, black);
  
        if (_step<segWidth) drawPixel(segWidth - _step, segHeight * 2 + 2 , _color); // Draw A
        if (_step<segWidth) drawPixel(segWidth - _step, 0, _color); // Draw D
      }
      
      if (_prevvalue==2) { // If 2 to " ", slide B to F and Flow G to C
        drawPixel(segWidth - _step, segHeight * 2 + 2, black); // Erase A
        drawLine(segWidth - _step + 1, segHeight * 2+1, segWidth - _step + 1, segHeight + 2, black); // erase B
        drawPixel(1 + _step, 0, black); // erase D
        drawLine(segWidth - _step + 1, 1, segWidth - _step + 1, segHeight, black); //Erase E
        drawPixel(1+_step, segHeight + 1, black); // Erase G left to right
  
      }
  
      if (_prevvalue==3) { // B to F, C to E
        // slide B to F 
        drawLine(segWidth - _step, segHeight * 2+1 , segWidth - _step, segHeight + 2, _color);
        if (_step > 0) drawLine(segWidth - _step + 1, segHeight * 2+1, segWidth - _step + 1, segHeight + 2, black);
        
        // Move C to E
        drawLine(segWidth - _step, 1, segWidth - _step, segHeight, _color);
        if (_step > 0) drawLine(segWidth - _step + 1, 1, segWidth - _step + 1, segHeight, black);
  
        // Erase G from right to left
        drawPixel(segWidth - _step, segHeight + 1, black); // G
      }
      
      if (_prevvalue==5) { // If 5 to 0, we also need to slide F to B
        if (_step<segWidth) {
          if (_step>0) drawLine(1 + _step, segHeight * 2 + 1, 1 + _step, segHeight + 2, black);
          drawLine(2 + _step, segHeight * 2 + 1, 2 + _step, segHeight + 2, _color);
        }
      }
      
      if (_prevvalue==5 || _prevvalue==9) { // If 9 or 5 to 0, Flow G into E
        if (_step<segWidth) drawPixel(segWidth - _step, segHeight + 1, black);
        if (_step<segWidth) drawPixel(0, segHeight - _step, _color);
      }
      //delay(animSpeed);
    }
  }
}

void Digit::setColorDigit(uint16_t newColor) {
    if(newColor!=_color){
      _color = newColor;
      _doMorph = false;
    }

}
void Digit::setValue(byte newValue) {
    _prevvalue = _value;
    _value = newValue;
    _doMorph = true;
}
