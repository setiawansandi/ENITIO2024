/*
 * Functional Test for WANDerer
 * 
 * Checks joystick, OLED, buzzer, LEDs and IR Transmitter + Receiver
 * 
 * For Code Generalisation: This code assumes LED are still placed both in front and on top
 */
#include "ENITIO_NeoPixel.h"
#include "ENITIO_joystick.h"
#include "ENITIO_ir.h"
#include "ENITIO_buzzer.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...

// the font height for font size 1 is 8
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long last_LED_flash_time = 0;
bool current_LED_state = 0;
int LED_flash_interval = 500;
int test_xValue = 0;
int test_yValue = 0;
int test_btnState = 0;

// IR variables
int IR_sent_flag = 0;
int IR_ACTION_RECV_WAIT = 150;
unsigned long IR_last_action_recv = 0;
int recv_status = 0;
int IR_flag_active_duration = 2000;

void toggle_LED(unsigned long currTime) {
  if (current_LED_state) {
    // now turn it off
    test_NeoPixel.off_FRONT();
    test_NeoPixel.off_TOP();
  } else {
    // turn it on
    test_NeoPixel.displayRGB_FRONT(255, 255, 255);
    test_NeoPixel.displayRGB_TOP(255, 255, 255);
  }
  current_LED_state = !current_LED_state;
  last_LED_flash_time = currTime;
}

void send_test_IR_message() {
  uint16_hex_digits address_digits, command_digits;

  address_digits.digit0 = 0;
  address_digits.digit1 = 1;
  address_digits.digit2 = 2;
  address_digits.digit3 = 4;

  command_digits.digit0 = 1;
  command_digits.digit1 = 2;

  ir_signal send_signal;

  send_signal.address = address_digits;
  send_signal.command = command_digits;

  Serial.print("Sending IR message: ");
  Serial.printf("%d %d %d %d %d %d %d %d \n", address_digits.digit3, address_digits.digit2, address_digits.digit1, address_digits.digit0, \
                                                    command_digits.digit3, command_digits.digit2, command_digits.digit1, command_digits.digit0);
        
  test_IR.send(send_signal, 1);
  IR_sent_flag = 1;
}

void receive_IR_messages(unsigned long currTime) {
  if (test_IR.available()) {
    ir_signal IRsignal_ = test_IR.read();
    Serial.println("Received Message!");
    if ((currTime - IR_last_action_recv) > IR_ACTION_RECV_WAIT) {
      // verify checksum
      unsigned int check0 = IRsignal_.address.digit0;
      unsigned int check1 = IRsignal_.address.digit1;
      unsigned int check2 = IRsignal_.address.digit2;
      unsigned int check3 = IRsignal_.address.digit3;

      if (check0 == 0 && check1 == 1 && check2 == 2 && check3 == 3) {
        // receive from test device success
        recv_status = 1;
        IR_last_action_recv = currTime;
      }
    }
  }
}

void test_loop() {
  unsigned long currTime = millis();
  if (current_LED_state && (currTime - last_LED_flash_time) > LED_flash_interval) {
    toggle_LED(currTime);
  } else if (!current_LED_state && (currTime - last_LED_flash_time) > 4 * LED_flash_interval){
    toggle_LED(currTime);
  }

  // update local states of joystick
  if (test_joystick.read_Joystick() == button) {
    // button is pressed - sound buzzer and send IR
    test_Buzzer.sound(NOTE_E3);
    if (IR_sent_flag == 0) {
      send_test_IR_message();
    }
    test_btnState = 1;
  } else {
    // turn off buzzer
    test_Buzzer.end_sound();
    IR_sent_flag = 0;
    test_btnState = 0;
  }
  // evaluating the if condition will also update x and y values of joystick
  test_xValue = test_joystick.get_xValue();
  test_yValue = test_joystick.get_yValue();

  receive_IR_messages(currTime);
  // reset IR flag if required
  if ((currTime - IR_last_action_recv) > IR_flag_active_duration) {
    recv_status = 0;
  }
}

void display_test_results() {
  display.clearDisplay();
  display.setTextSize(1); // Draw SIZE
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  
  display.setCursor(0, 0);
  display.println(F("      Func Test     ")); 

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1); // Draw SIZE
  display.setCursor(0, 10);
  display.print("X Axis: "); display.println(test_xValue);

  display.setCursor(0, 20);
  display.print("Y Axis: "); display.println(test_yValue);

  display.setCursor(0, 30);
  display.print("Button: ");
  if (test_btnState == 1) {
    display.println("Pressed");
  } else {
    display.println("NIL");
  }

  display.setCursor(0, 40);
  display.print("IR Recv: ");
  if (recv_status == 1) {
    display.println("Received");
  } else {
    display.println("NIL");
  }

  display.display();
};

void setup() {
  test_NeoPixel.initialize();
  test_NeoPixel.off_FRONT();
  test_NeoPixel.off_TOP();
  
  Serial.begin(115200);
  test_IR.enable();

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  test_joystick.initialize();
}

void loop() {
  test_loop();
  display_test_results();
}
