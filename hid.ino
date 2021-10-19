
// USB host mouse from USB Host Shield Library. Install using Library Manager
#include <hidboot.h>
#if ARDUINO >= 10606
#include <Mouse.h>

#define HID_SendReport(id,data,len) HID().SendReport(id,data,len)
#endif
// USB device mouse library included with Arduino IDE 1.8.5

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

class MouseRptParser : public MouseReportParser
{
  protected:
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

void MouseRptParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{

#if 0
  // Demo -- Swap left and right buttons on 4 button Kensington trackball
  if (len > 0) {
    uint8_t button1 = buf[0] & 0x01;
    uint8_t button2 = buf[0] & 0x02;
    uint8_t button3 = buf[0] & 0x04;
    uint8_t button4 = buf[0] & 0x08;
    buf[0] = (buf[0] & 0xF0) | (button1 << 1) | (button2 >> 1) |
             (button3 << 1) | (button4 >> 1);
  }
#endif
  // Run parent class method.
  MouseReportParser::Parse(hid, is_rpt_id, len, buf);


  // Show USB HID mouse report


  if (len > 2) {
    uint8_t mouseRpt[4];
    mouseRpt[0] = buf[0];
    mouseRpt[1] = buf[1];
    mouseRpt[2] = buf[2];
    // If the mouse/trackball has a scroll wheel, send the value
    if (len > 3) {
      mouseRpt[3] = buf[3];
    }
    else {
      mouseRpt[3] = 0;
    }
    HID().SendReport(1, mouseRpt, sizeof(mouseRpt));
  }
}

USB     Usb;
HIDBoot<USB_HID_PROTOCOL_MOUSE>    HidMouse(&Usb);

MouseRptParser Prs;

void setup()
{
  Serial.begin( 115200 );
  uint8_t attempts = 30;
  while (!Serial && attempts--) {
    delay(100); // Wait for serial port to connect for up to 3 seconds
  }
  Serial.println("Start");

  if (Usb.Init() == -1) {
    Serial.println("USB host shield did not start.");
  }
  
  delay( 200 );

  HidMouse.SetReportParser(0, &Prs);

  Mouse.begin();


  
}

String InBytes;
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

void recvWithEndMarker() {
 static byte ndx = 0;
 char endMarker = '\n';
 char rc;
 
if (Serial.available() > 0) {
 rc = Serial.read();

 if (rc != endMarker) {
 receivedChars[ndx] = rc;
 ndx++;
 if (ndx >= numChars) {
 ndx = numChars - 1;
 }
 }
 else {
 receivedChars[ndx] = '\0'; // terminate the string
 ndx = 0;
 newData = true;
 }
 }
}
void showNewData() {
 if (newData == true) {
  char* buffer = receivedChars;
  float x = atoi(strtok(buffer, ","));
  float y = atof(strtok(NULL, ","));
  Serial.print(x+y);
  Mouse.move(x, y, 0);
 newData = false;
 }
}

void loop()
{
  

recvWithEndMarker();
Usb.Task();
showNewData();



}
