#include "FTMSExerciseBike.h"
#include <random>

BLECharacteristic *pCharacteristic;

void FTMSExerciseBike::setup() {
    BLEDevice::init("FTMS Device");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(BLEUUID("00001816-0000-1000-8000-00805f9b34fb"));

    pCharacteristic = pService->createCharacteristic(
                      BLEUUID("00002AD2-0000-1000-8000-00805f9b34fb"),
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLEUUID(pService->getUUID()));
    pAdvertising->start();

    this->set_update_interval(1000); // Update every 1 second
}

void FTMSExerciseBike::sendData(float avgCadence, float avgSpeed, float instCadence, float instSpeed, float totalDistance) {
    // Calculating field values in accordance with the FTMS specification
    uint16_t crankRevolution = calculateCrankRevolution();
    uint16_t wheelRevolution = calculateWheelRevolution();

    // Prepare the data packet adhering to the FTMS specification
    uint8_t dataPacket[12];
    dataPacket[0] = 0x07; // Flags field
    dataPacket[1] = (uint8_t)instSpeed;
    dataPacket[2] = (uint8_t)(instSpeed >> 8);
    dataPacket[3] = (uint8_t)instCadence;
    dataPacket[4] = (uint8_t)avgSpeed;
    dataPacket[5] = (uint8_t)avgCadence;
    dataPacket[6] = (uint8_t)totalDistance;
    dataPacket[7] = (uint8_t)(totalDistance >> 8);
    dataPacket[8] = (uint8_t)(totalDistance >> 16);
    dataPacket[9] = (uint8_t)(totalDistance >> 24);
    dataPacket[10] = (uint8_t)wheelRevolution;
    dataPacket[11] = (uint8_t)(wheelRevolution >> 8);

    pCharacteristic->setValue(dataPacket, sizeof(dataPacket));
    pCharacteristic->notify();
}

float FTMSExerciseBike::generateRandomFloat(float min, float max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

uint16_t FTMSExerciseBike::calculateCrankRevolution() {
    // Replace with your calculation logic
    return 0;
}

uint16_t FTMSExerciseBike::calculateWheelRevolution() {
    // Replace with your calculation logic
    return 0;
}

void FTMSExerciseBike::update() {
    // Generate random data
    float avgCadence = generateRandomFloat(50.0, 100.0);
    float avgSpeed = generateRandomFloat(15.0, 25.0);
    float instCadence = generateRandomFloat(60.0, 120.0);
    float instSpeed = generateRandomFloat(10.0, 30.0);
    float totalDistance = generateRandomFloat(500.0, 2000.0);

    sendData(avgCadence, avgSpeed, instCadence, instSpeed, totalDistance);

    // Expose data to Home Assistant
    this->publish_state("avg_cadence", avgCadence);
    this->publish_state("avg_speed", avgSpeed);
    this->publish_state("inst_cadence", instCadence);
    this->publish_state("inst_speed", instSpeed);
    this->publish_state("total_distance", totalDistance);
}
