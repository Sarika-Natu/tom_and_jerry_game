#include "sensors.h"

#include "game_accelerometer.h"

bool sensors__init(void) {
  bool status = true;

  status &= acceleration__init();
  // status &= apds__init();

  return status;
}