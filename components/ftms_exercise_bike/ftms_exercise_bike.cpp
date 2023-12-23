#include "ftms_exercise_bike.h"
#include <random>

BLECharacteristic *pCharacteristic;

namespace esphome {
namespace tallsequoia {

static const char *TAG = "ftmsexercisebike";


// SETUP

void FTMSExerciseBike::setup() {
    BLEDevice::init("FTMS Device");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(BLEUUID("00001816-0000-1000-8000-00805f9b34fb"));

    pCharacteristic = pService->createCharacteristic(
                      BLEUUID("00002AD2-0000-1000-8000-00805f9b34fb"),    // 2AD2 is the Exercise Bike
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLEUUID(pService->getUUID()));
    pAdvertising->start();

    this->set_update_interval(1000); // Update every 1 second
}


// OPERATION

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

} // namespace tallsequoia
} // namespace esphome



/*

#include "ExerciseBikeComponent.h"

void ExerciseBikeComponent::setup() {
  setupSensors();
  setupBLE();

  if (!wheel_pin_ || wheel_circumference_ == 0.0) {
    ESP_LOGW("ExerciseBike", "Wheel pin or circumference not set in YAML!");
    return;
  }
  
  // Bounds checking for wheel circumference
  if (wheel_circumference_ < MIN_WHEEL_CIRCUMFERENCE || wheel_circumference_ > MAX_WHEEL_CIRCUMFERENCE) {
    ESP_LOGW("ExerciseBike", "Invalid wheel circumference. Must be between 10 cm and 5 meters.");
    return;
  }
  
  pinMode(wheel_pin_->get_pin(), INPUT);

  ble_client_ = new BLEClient();
  // Define a service for FTMS
  BLEService *ftms_service = ble_client_->create_service(0x1826); // FTMS service UUID

  // Create characteristics for Speed, Cadence, and Machine Status within the FTMS service
  speed_characteristic_ = ftms_service->create_characteristic(0x2A6C); // Speed characteristic UUID
  cadence_characteristic_ = ftms_service->create_characteristic(0x2A5B); // Cadence characteristic UUID
  machine_status_characteristic_ = ftms_service->create_characteristic(0x2A5C); // Machine Status characteristic UUID

  // Other initialization and FTMS protocol setup
}

void ExerciseBikeComponent::loop() {
  // Existing loop code...

  unsigned long current_time = millis();
  unsigned int current_rotation_count = rotation_count_;
  
  unsigned long time_diff = current_time - last_update_time_;
  unsigned int rotation_diff = current_rotation_count - last_rotation_count_;
  
  if (time_diff >= update_interval_) {
    updateSpeed(rotation_diff, time_diff);
    updateCadence(rotation_diff, time_diff);
    updateTotalDistance(rotation_diff);

    handleReset(time_diff);
  }
  // Other tasks or events handling
}

void ExerciseBikeComponent::updateSpeed(unsigned int rotation_diff, unsigned long time_diff) {
  float rpm = rotation_diff * 60.0 / (time_diff / 1000.0);
  
  // Update total distance based on wheel rotations and circumference
  float distance_delta = (rotation_diff * wheel_circumference_) / 1000.0;  // in meters
  total_distance_ += distance_delta;
  
  speed_characteristic_->set_value(rpm);
}

void ExerciseBikeComponent::updateCadence(unsigned int rotation_diff, unsigned long time_diff) {
  static unsigned int recent_rotations[10]; // Store last 10 rotation counts
  static unsigned int index = 0; // Index for the circular buffer
  static unsigned int total_rotations = 0; // Total rotations in the last 10 seconds

  recent_rotations[index] = rotation_diff;
  total_rotations += rotation_diff - recent_rotations[(index + 1) % 10]; // Subtract oldest value

  index = (index + 1) % 10; // Move to the next position in the circular buffer

  float cadence_rpm = total_rotations * 6.0; // RPM for 10 seconds (60/10)
  float cadence = cadence_rpm; // Calculate cadence based on RPM (placeholder)

  cadence_characteristic_->set_value(cadence);
}

void ExerciseBikeComponent::updateTotalDistance(unsigned int rotation_diff) {
  float distance_delta = (rotation_diff * wheel_circumference_) / 1000.0; // in meters
  total_distance_ += distance_delta;
}

void ExerciseBikeComponent::handleReset(unsigned long time_diff) {
  if (time_diff >= NO_DISTANCE_RESET_TIME) {
    resetBike();
  }
}

void ExerciseBikeComponent::resetBike() {
  rotation_count_ = 0;
  total_distance_ = 0.0;  // Reset total distance
  
  // Publish the reset total distance to Home Assistant as a sensor
  // Logic to publish this value to Home Assistant as a sensor
}
*/
