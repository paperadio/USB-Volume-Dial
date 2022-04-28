//[ Original Code (Thanks to youtuber *EunChan Park*) ]
//https://github.com/happythingsmaker/PremiereProDialDIY/blob/master/PremiereProDialDIY_LEFT_RIGHT_SPACE/PremiereProDialDIY_LEFT_RIGHT_SPACE.ino

// [ Introduction ]
//Youtube https://www.youtube.com/watch?v=-YmWcLdHauM
//Source Code https://github.com/paperadio/USB-Volume-Dial
//3D Printing File https://www.thingiverse.com/thing:5371158

// Resolution : some encoder is too sensitive. this reduces sensitivity.
#define RESOLUTION 2

// include a HID library
#include "HID-Project.h"

// From here, text "OUTPUT_B" is going to be replaced with "15".
#define  OUTPUT_B A1

// "OUTPUT_A" is going to be replaced with "A0".
#define  OUTPUT_A 15

// "BUTTON" is going to be replaced with "A1".
#define  BUTTON A2

// Declare variables aState, aLastState for checking the state of OUTPUT_A of the encoder
bool aState;

// We need to save the previous state of OUTPUT_A
bool aLastState;

// this variable for check the state of button.
// in order to prevent the button from chattering, we need to check the first moment the button is pressed.
// Becasue when a button pressed, the button usually sends a lot of signals.
// So, we need to ignore following noise signals for a while.
bool lastButtonState = 0;

// mode selection
#define VOLUME_MODE 0
#define PREMIERE_MODE 1
#define LIGHTROOM_MODE 2
int mode = VOLUME_MODE;

// void setup(){} function is for one time setting
void setup() {

  // Sends a clean report to the host. This is important on any Arduino type.
  Consumer.begin();

  mode == VOLUME_MODE;
  Serial.begin(115200);
  delay(100);
}

// in order to prevent chattering, we need to check the moment when was the last click moment
// for 1000ms, we will ignore all signals
long lastClickTime = 0;
long tempCount = 0;

// this loop() function repeats its code eternally
void loop() {
  //read signal from OUTPUT_A and save its state to aState
  aState = digitalRead(OUTPUT_A);

  // if aLastState is not currentState, it meant there's something changed.
  if (aState != aLastState) {

    // read another pin's state.
    // if you want to know about the theory, watch this video
    // https://www.youtube.com/watch?v=v4BbSzJ-hz4
    if (digitalRead(OUTPUT_B) != aState) {
      //Serial.println(F("rotateLeft"));
      rotateLeft();
      delay(8);
    } else {
      //Serial.println(F("rotateRight"));
      rotateRight();
      delay(8);
    }
    // save current State as last State
    aLastState = aState;
  }

  // read button (short or long)
  if (digitalRead(BUTTON) == LOW) {
    if (lastButtonState == LOW) {
      // LOW -> LOW : nothing to do

    } else {
      // HIGH-> LOW
      lastClickTime = millis();
      delay(300); // ignoring chattering

    }
    lastButtonState = LOW;
  } else {
    if (lastButtonState == LOW) {   // LOW -> HIGH : check whether long press or not
      if (millis() - lastClickTime >= 3000) {
        // long press : mode change
        changeMode();
      } else {
        // short press :
        Serial.println(F("pressButton"));
        pressButton();
      }
    }
    else {                          // HIGH -> HIGH : noting to do
    }
    lastButtonState = HIGH;
  }
}

void changeMode() {
  //mode = ++mode % numMode;
  int a = 0;
}

void rotateLeft() {
  if (tempCount++ % RESOLUTION == 0) {
    if (mode == VOLUME_MODE) {
      Consumer.write(MEDIA_VOLUME_DOWN);
    } else if (mode == LIGHTROOM_MODE) {
      Serial.println(F("pressButton"));
    }
  }
}

void rotateRight() {
  if (tempCount++ % RESOLUTION == 0) {
    if (mode == VOLUME_MODE) {
      Consumer.write(MEDIA_VOLUME_UP);
    } else if (mode == LIGHTROOM_MODE) {
      Serial.println(F("pressButton"));
    }
  }
}

void pressButton() {
  Serial.println(F("MEDIA_VOLUME_MUTE"));
  Consumer.write(MEDIA_VOLUME_MUTE);
}
