#include "Tlc5940.h"

int colorLinearCorrection[] = {
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
          0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
          0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05,
          0x05, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x09, 0x09, 0x0A, 0x0A, 0x0B, 0x0B,
          0x0C, 0x0C, 0x0D, 0x0D, 0x0E, 0x0F, 0x0F, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
          0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x26, 0x27, 0x29, 0x2B, 0x2C,
          0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, 0x40, 0x43, 0x45, 0x47, 0x4A, 0x4C, 0x4F,
          0x51, 0x54, 0x57, 0x59, 0x5C, 0x5F, 0x62, 0x64, 0x67, 0x6A, 0x6D, 0x70, 0x73, 0x76, 0x79, 0x7C,
          0x7F, 0x82, 0x85, 0x88, 0x8B, 0x8E, 0x91, 0x94, 0x97, 0x9A, 0x9C, 0x9F, 0xA2, 0xA5, 0xA7, 0xAA,
          0xAD, 0xAF, 0xB2, 0xB4, 0xB7, 0xB9, 0xBB, 0xBE, 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
          0xD0, 0xD2, 0xD3, 0xD5, 0xD7, 0xD8, 0xDA, 0xDB, 0xDD, 0xDE, 0xDF, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5,
          0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xED, 0xEE, 0xEF, 0xEF, 0xF0, 0xF1, 0xF1, 0xF2,
          0xF2, 0xF3, 0xF3, 0xF4, 0xF4, 0xF5, 0xF5, 0xF6, 0xF6, 0xF6, 0xF7, 0xF7, 0xF7, 0xF8, 0xF8, 0xF8,
          0xF9, 0xF9, 0xF9, 0xF9, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFC,
          0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD,
          0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF};

typedef struct 
{
  int red,green,blue;
}Color;

typedef struct _Strip Strip;

typedef void (*stripProcess)(Strip *s); 

struct _Strip
{
  int red,green,blue;
  Color color;
  stripProcess process;
  int stage;
  int length;
  unsigned long t;
};

Color firstColor = {0x45, 0x02, 0x79};
Color secondColor = {0xF8, 0xC2, 0x00};
Color off = {0, 0, 0};

Strip campStrip = {1,2,0,off, NULL, -1,0 , 0};
Strip iStrip = {3,4,5,off, NULL, -1, 0, 0};
Strip davidStrip = {6,7,8,off, NULL, -1, 0, 0};



void setup()
{
  Serial.begin(115200);
  Tlc.init(4095); // initialise TLC5940 and set all channels 
  randomSeed(analogRead(0));
  setOn();
}

void setAllOff()
{
  for (int i = 0; i < 9; i++)
    Tlc.set(i, 4095);   
      
  Tlc.update();
}

int to4095(int c)
{
  return 4095 - c*16;
}


void brightnessColor(Color basic, Color *newColor, int brightness)
{
  newColor->red = basic.red * brightness / 255;
  newColor->green = basic.green * brightness / 255;
  newColor->blue = basic.blue * brightness / 255;
}

void setStrip(Strip strip, Color color, int update = 1) //brightness should be between 0 to 255
{
  Tlc.set(strip.red,to4095(color.red));
  Tlc.set(strip.green,to4095(color.green));
  Tlc.set(strip.blue,to4095(color.blue));
  
  if(update == 1)
    Tlc.update();
  delay(1);
}

void fadeIn(Strip *strip)
{
    Color temp;
    if(strip->stage == 255)
    {
        setStrip(*strip, strip->color);
        strip->stage = -1;
        strip->process = NULL;
        strip->t = millis();
    }
    else if(strip->t < millis())
    {
        brightnessColor(strip->color, &temp, colorLinearCorrection[strip->stage]);
        setStrip(*strip, temp);
      
        strip->t = millis() + (strip->length/255);
        strip->stage++;
    }
}

void fadeOut(Strip *strip)
{
    Color temp;
    if(strip->stage == 255)
    {
        setStrip(*strip, off);
        
        strip->stage = -1;
        strip->process = NULL;
        strip->t = millis();
    }
    else if(strip->t < millis())
    {
        brightnessColor(strip->color, &temp, colorLinearCorrection[255 - strip->stage]);
        setStrip(*strip, temp);
     
        strip->t = millis() + (strip->length/255);
         strip->stage++;
    }
}

void flickering(Strip *strip)
{
  if(strip->stage%3 == 0)
  {
    setStrip(*strip,strip->color);
    
    strip->t = millis() + random(40,60);
    strip->stage++;
  }
  else if(strip->stage%3 == 1 &&  strip->t < millis()  )
  {
    setStrip(*strip,off);
    
    if(strip->stage > strip->length*3 || strip->length > 20)
    {
      strip->stage = -1;
      strip->process = NULL;
      strip->t = millis();
    }
    else
    {
       strip->t = millis() + random(80,250);
        strip->stage++;
    }
  }
  else if(strip->stage%3 == 2 && strip->t <  millis())
     strip->stage++;
 
}

void setFlickering(Strip *strip, int length)
{
  if(strip->stage == -1)
  {
    strip->process = flickering;
    strip->stage = 0;
    strip->length = length;
  }
}
void setFade(Strip *strip, stripProcess process, int length)
{
   if(strip->stage == -1)
   {
    strip->process = process;
    strip->stage = 0;
    strip->length = length;
   }
}

void waitTillProcessEnded(Strip *strip)
{
  while(strip->process != NULL && strip->stage != -1)
  {
    strip->process(strip);
    delay(2);
  }
}

void waitTillallEnded(int delayTiem = 0)
{
  unsigned long startTime = millis();
  while(campStrip.stage != -1 || iStrip.stage != -1 || davidStrip.stage!= -1)
  {
    if(campStrip.process != NULL && campStrip.stage != -1)
      campStrip.process(&campStrip);
    if(iStrip.process != NULL && iStrip.stage != -1)
      iStrip.process(&iStrip);
    if(davidStrip.process != NULL && davidStrip.stage!= -1)
      davidStrip.process(&davidStrip);
    delay(2);
    if(delayTiem!= 0 && millis() - startTime > delayTiem)
      break;
  }
}

void setOn()
{
  setRandomColor();
  setFlickering(&iStrip, random(2,3));
  waitTillProcessEnded(&iStrip);
  delay(random(500,1200));
   setFlickering(&iStrip, random(2,3));
  waitTillProcessEnded(&iStrip);
  delay(800);
  setFade(&campStrip, fadeIn, 3000);
  setFade(&davidStrip, fadeIn, 3000);
  waitTillallEnded(random(1500,2000));
  setFlickering(&iStrip, random(2,3));
  waitTillallEnded();
  delay(random(800,1200));
  setFlickering(&iStrip, random(2,3));
  waitTillProcessEnded(&iStrip);
  setFade(&iStrip, fadeIn, 3000);
  waitTillProcessEnded(&iStrip);
}

void setRandomColor()
{
   iStrip.color = (random(10) > 4)? firstColor: secondColor;
    davidStrip.color = iStrip.color;
   if(random(10) > 2)
      campStrip.color = (iStrip.color.red == secondColor.red && iStrip.color.green == secondColor.green && iStrip.color.blue == secondColor.blue)? firstColor: secondColor;
   else
      campStrip.color = iStrip.color;
}


void loop()
{
  waitTillallEnded();
  Serial.print("hi");
  delay(random(50000,80000));
  if(random(18) > 3) //just flickering
  {
    Serial.println("1");
    setStrip(iStrip, off); //turn off the strip
    
    setFlickering(&iStrip, random(3,5)); //flickring
    waitTillProcessEnded(&iStrip);
    setStrip(iStrip, iStrip.color);
    delay(random(300,500));
    setFlickering(&iStrip, random(2,3));
    waitTillProcessEnded(&iStrip);
    
    setStrip(iStrip, iStrip.color); //turn on the strip
  }
  else
  {
    Serial.println("2");
    
    setFade(&campStrip, fadeOut, 3000); //fade all out
    waitTillallEnded(random(600,800));
    
    setFade(&iStrip, fadeOut, 3000);
    setFade(&davidStrip, fadeOut, 3000);
    waitTillallEnded();
    delay(random(2000,5000));
    
    setRandomColor(); //get random color
    
    setFlickering(&iStrip, 3); //flickering
    waitTillProcessEnded(&iStrip);
    
    delay(random(600,1500));
    
    setFlickering(&iStrip, random(2,3));
    waitTillProcessEnded(&iStrip);

    delay(random(1200,3000));
    
     setFade(&campStrip, fadeIn, 5000); //fade all in
     setFade(&davidStrip, fadeIn, 5000);
     waitTillallEnded(random(1500,2000));
     setFlickering(&iStrip, random(2,3));
     waitTillallEnded();
     setFade(&iStrip, fadeIn, 3000);
     waitTillProcessEnded(&iStrip);
  }
}
