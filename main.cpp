#include <Arduino.h>
#include "sensors/Sensors.h"
enum state {STATE_CALIBRATE, STATE_ALIGN, STATE_IDLE, D};

state current_state = A;
state next_state = current_state;

SensorHandler sh;

void setup() {

  // put your setup code here, to run once:
}

void state_calibration()
{
 if(sh.CollectCalibrationData())
 {
    sh.CalibrateInertial();
    sh.AlignInertial();
    next_state = STATE_ALIGN;
 }
}

void state_align()
{
  if (sh.AlignInertial())
  {
    next_state = STATE_IDLE;
  }
}

void state_get_shot()
{
  
}

void state_BLE()
{

}

void loop() {
  switch(current_state)
  {
    case (STATE_CALIBRATE):

    break;

    case (STATE_ALIGN):
    break;

    case (C):
    break;
  }
}