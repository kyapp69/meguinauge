#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
#include <LiquidCrystal.h>
#include <math.h>

//RS(4),EN(6),DB4(11),DB5(12),DB6(13),DB7(14)
//                7  8  9  10  11  12
LiquidCrystal lcd(14, 3, 4, 5, 6, 7);

//#define REDLITE 3 
//#define GREENLITE 5 
//#define BLUELITE 6

int brightness = 255;

byte fill1[8] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};
byte fill2[8] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};
byte fill3[8] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};
byte fill4[8] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};
byte fill5[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};


char can_buffer[64];
char can_data;

//possible parameters to display
double  engine_afr;         //AFR
double  engine_afr_tgt;     //AFR target
double  engine_map;         //MAP sensor
double  engine_boost;       //boost (psi) calculated from map value
double  engine_adv;         //ignition advance
int     engine_rpm;         //RPM
int     engine_tps;         //throttle position
double  engine_vbat;        //battery voltage
int     engine_clt;         //coolant temp
int     engine_iat;         //intake air temp



void setup() {


  lcd.createChar(0, fill1);
  lcd.createChar(1, fill2);
  lcd.createChar(2, fill3);
  lcd.createChar(3, fill4);
  lcd.createChar(4, fill5);
  
  lcd.begin(20, 4);
  lcd.print("AFR");
  lcd.setCursor(0, 2);
  lcd.print("TGT");

  //pinMode(REDLITE, OUTPUT); 
  //pinMode(GREENLITE, OUTPUT); 
  //pinMode(BLUELITE, OUTPUT);
  //setBacklight(255, 0, 0);

  //Serial.begin(115200);
  int can_init = mcp2515_init(1); //CANSPEED_500 = 1

}

void loop() {

  tCAN message;
  ecu_req(&message);
  double tgt = (double)message.data[0] / 10.0;
  double afr = (double)message.data[1] / 10.0;
  
  //double afr = 12.5;
  //double tgt = 14.7;
  lcd.clear();

  lcd.begin(20, 4);
  lcd.print("AFR");
  lcd.setCursor(0, 2);
  lcd.print("TGT");
  

  

  if(afr >= 10.0 && afr <= 20.0) {
    lcd.setCursor(4, 0);
    lcd.print(afr);
    draw_bar(afr, 1, 10, 20);  
  }
  if(tgt >= 10.0 && tgt <= 20.0) {
    lcd.setCursor(4, 2);
    lcd.print(tgt);
    draw_bar(tgt, 3, 10, 20);  
  }
  
  delay(300);
}

void draw_bar(double value, int row, double minimum, double maximum) {
  lcd.setCursor(0, row);
  int bars = ((value - minimum) * 100) / (maximum - minimum);
  int fullBars = bars/5;
  int partialBars = bars % 5;
  lcd.setCursor(0, row);
  for(int i = 0; i < fullBars; i++) {
    lcd.write(byte(4));
  }
  if(partialBars > 0) {
    lcd.write(byte(partialBars - 1));    
  }
}

void setBacklight(uint8_t r, uint8_t g, uint8_t b) {
  // normalize the red LED - its brighter than the rest! 
  r = map(r, 0, 255, 0, 100);
  g = map(g, 0, 255, 0, 150);
  r = map(r, 0, 255, 0, brightness); 
  g = map(g, 0, 255, 0, brightness); 
  b = map(b, 0, 255, 0, brightness);
  // common anode so invert!
  r = map(r, 0, 255, 255, 0);
  g = map(g, 0, 255, 255, 0);
  b = map(b, 0, 255, 255, 0);
  //analogWrite(REDLITE, r); 
  //analogWrite(GREENLITE, g); 
  //analogWrite(BLUELITE, b);
}




void ecu_req(tCAN *message) 
{
  //tCAN message;
  float engine_data;
  int timeout = 0;
  char message_ok = 0;
  // Prepair message
  message->id = 0x5ea; //group 1514 //0x5e8;
  message->header.rtr = 0;
  message->header.length = 8;

  mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
  
  if (mcp2515_get_message(message)) {
    //Serial.println(message.data[0]);
    //Serial.println(message.data[1]);
    //return &message.data;
  }
  else {
  }
}

