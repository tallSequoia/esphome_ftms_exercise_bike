#ifndef FTMS_EXERCISE_BIKE_H
#define FTMS_EXERCISE_BIKE_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "esphome.h"

class FTMSExerciseBike : public PollingComponent {
public:
    void setup() override;
    void update() override;

private:
    void sendData(float avgCadence, float avgSpeed, float instCadence, float instSpeed, float totalDistance);
    uint16_t calculateCrankRevolution();
    uint16_t calculateWheelRevolution();
    float generateRandomFloat(float min, float max);
};

#endif
