#include <Arduino.h>
#include <NeoICSerial.h>
#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>
#include <NeoSWSerial.h>


NeoSWSerial serial_lcd_port(3,2);

static NMEAGPS   gps;

void lcdClear();

static void GPSisr( uint8_t c )
{
  gps.handle( c );

} 


String lcdOutput("T=-1:-1:-1 S=-1");
bool lcdUpdated = true;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  gpsPort.attachInterrupt( GPSisr );
  gpsPort.begin( 9600 );


  serial_lcd_port.begin(9600);

  lcdClear();

}
void lcdClear() {
  serial_lcd_port.write(0xfe);
  serial_lcd_port.write(0x01);

  serial_lcd_port.write(0xfe);
  serial_lcd_port.write(0x80);
}
void lcdprint(String output) {
  lcdClear();  
  serial_lcd_port.write(output.c_str());

}

String leadingZeroes(uint8_t val) {
  char *retVal="00";
  sprintf(retVal,"%02d", val);
  return String(retVal);
}
void loop() {

  if(lcdUpdated) {
      lcdprint(lcdOutput);
      lcdUpdated = false;
  }


  if (gps.available()) {
    gps_fix fix = gps.read();
    if(fix.valid.satellites) {
      lcdUpdated = true;
      lcdOutput = lcdOutput.substring(0,13)+leadingZeroes(fix.satellites);
    } 
    if (fix.valid.time) {
      char* datechar ="             ";
      lcdUpdated = true;
      sprintf(datechar, "T=%02d:%02d:%02d", fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds);
      lcdOutput = datechar+lcdOutput.substring(10);
    }
  }


}
