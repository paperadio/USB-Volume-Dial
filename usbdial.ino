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

#include <TimerOne.h>

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
#define LIGHTROOM_MODE 2
int mode = VOLUME_MODE;

volatile bool is_rotating = false;  //是否正在旋转中
bool curr_rotation_dir = false;  //false:left, true:right
unsigned long last_rotate_time;  //最后一次旋转动作时间, ms
#define ROTATE_PERIOD_WINDOW 20  //ms


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

// this loop() function repeats its code eternally
void loop() {
  //read signal from OUTPUT_A and save its state to aState
  aState = digitalRead(OUTPUT_A);

  // if aLastState is not currentState, it meant there's something changed.
  if (aState != aLastState) {
    // read another pin's state.
    // if you want to know about the theory, watch this video
    // https://www.youtube.com/watch?v=v4BbSzJ-hz4
    
    //Check the value of OUTPUT_B
    bool tmp_readout = (digitalRead(OUTPUT_B) == aState);

    // Rotate left if OUTPUT_B changed, otherwise rotate right
    rotate(tmp_readout);

    // Delay for 10 milliseconds
    delay(10);

    //Serial.println(tmp_readout);

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
    else {  // HIGH -> HIGH : noting to do
    }
    lastButtonState = HIGH;
  }

  if (is_rotating) {
    if ((millis() - last_rotate_time)>ROTATE_PERIOD_WINDOW) {
      is_rotating = false;
      //Serial.println(F("一次旋转动作结束"));
    }
  }
}

void changeMode() {
  //mode = ++mode % numMode;
  int a = 0;
}

/**
 * rotate - Rotate function that handles volume control
 *
 * @direction: false for rotate left, true for rotate right
 * 滤波方法:以一个timer作为判断是否为一次连续旋转动作的依据，并以本次动作第一个数据
 * 作为动作方向，在整个动作过程中保持该方向不变。经实际测试，当ROTATE_PERIOD_WINDOW
 * 为20ms时，对旋转方向的判断灵敏且准确，可以消除快速旋转时产生的信号噪音。可以满足
 * 游戏或打碟等高速来回旋转的使用场景。
 */
void rotate(bool direction) {
  // Set the current rotation direction to the first value received
  if (!is_rotating) {
    curr_rotation_dir = direction;
    //Serial.println(F("一次旋转动作开始"));
  }

  // Always use the first value received as direction
  direction = curr_rotation_dir;

  // Record the time of the last rotation action
  last_rotate_time = millis();

  // Set is_rotating to true to indicate that a rotation action is in progress
  is_rotating = true;

  //Serial.println(direction);

  /* 
  Based on the current mode, either control the media volume or send a command to press a button in Lightroom.
  If mode is VOLUME_MODE, control the media volume by sending a command to increase or decrease it.
  If mode is LIGHTROOM_MODE, send a command to press a button in Lightroom.
  */
  if (mode == VOLUME_MODE) {
      // Control the media volume
      Consumer.write(direction?MEDIA_VOLUME_UP:MEDIA_VOLUME_DOWN);
  } else if (mode == LIGHTROOM_MODE) {
    // Send command to press button in Lightroom
    Serial.println(F("pressButton"));
  }
}


void pressButton() {
  Serial.println(F("MEDIA_VOLUME_MUTE"));
  Consumer.write(MEDIA_VOLUME_MUTE);
}
