int FLOW_SENSOR_PIN = 2; // Interrupt-enabled pin
int SOLENOID_PIN = 12;

float ML_IN_L = 1000.0; // 1000 mL in 1 L
float SEC_IN_MIN = 60.0; // 60 seconds in 1 minute
float CAPTURE_VOLUME_ML = 1000.0; // 5 litres
float FLOW_SENSOR_CALIBRATION_FACTOR = 11.0; // From datasheet: F = 11 * Q (L/min)
float TURBIDITY_VOLTAGE_RANGE_CEIL   = 4.37;
float TURBIDITY_VOLTAGE_RANGE_FLOOR  = 3.7;
int TURBIDITY_COUNTER_THRESHOLD = 3;

int TURBIDITY_READ_PERIOD = 50; // 50 ms -> 20 Hz
float FLOW_RATE_READ_PERIOD = 250.0; // 0.25 seconds
float MAX_CAPTURE_LINE_OPEN_TIME = 300000.0; // 300,000 ms = 5 minutes

volatile byte pulseCount;  
unsigned long oldTime;
bool isSolenoidOpen;
float totalCaptureVolume;
float totalTimeValveOpen;
int turbidityThresholdCounter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(SOLENOID_PIN, OUTPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT);
  digitalWrite(FLOW_SENSOR_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);

  pulseCount = 0;
  oldTime = 0;
  totalCaptureVolume = 0.0;
  totalTimeValveOpen = 0.0;

  isSolenoidOpen = false;
  closeSolenoid();
}

void loop() {
  int sensorValue = analogRead(A5);
  float voltage = convertTurbiditySensorValueToVoltage(sensorValue);
  delay(TURBIDITY_READ_PERIOD);
  Serial.print("DATA(turbidity): ");
  Serial.println(voltage);

  if (isContaminantDetected(voltage)) {
    turbidityThresholdCounter++;
  } else {
    turbidityThresholdCounter = 0;    
  }

  if (turbidityThresholdCounter >= TURBIDITY_COUNTER_THRESHOLD && !isSolenoidOpen) {
    Serial.println("----------------------------------------------");
    Serial.println("INFO: turbidity sensor value passed threshold!");
    Serial.println("----------------------------------------------");
    openSolenoid();
    turbidityThresholdCounter = 0;
  }

  if (isSolenoidOpen) {
    if ((millis() - oldTime) > FLOW_RATE_READ_PERIOD) {
      detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
      float flowRate = ((FLOW_RATE_READ_PERIOD / (millis() - oldTime)) * pulseCount) / FLOW_SENSOR_CALIBRATION_FACTOR;
      oldTime = millis();
      float flowMilliLitres = max(0, (flowRate / SEC_IN_MIN) * ML_IN_L);    
      totalCaptureVolume += flowMilliLitres;
      Serial.print("DATA(total_capture_volume): ");
      Serial.println(totalCaptureVolume);
      pulseCount = 0;
      attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);
      totalTimeValveOpen += FLOW_RATE_READ_PERIOD;
    }

    if (totalCaptureVolume >= CAPTURE_VOLUME_ML) {
      closeSolenoid();
      Serial.print("DATA(total_capture_time): ");
      Serial.println(totalTimeValveOpen);
    }

    if (totalTimeValveOpen >= MAX_CAPTURE_LINE_OPEN_TIME) {
      Serial.println("WARN: solenoid valve open timeout");
      closeSolenoid();
      Serial.print("DATA(total_capture_time): ");
      Serial.println(totalTimeValveOpen);
    }
  }
}

bool isContaminantDetected(float voltage) {
  // should detect suspended solids, but ignore air bubbles
  return (voltage < TURBIDITY_VOLTAGE_RANGE_CEIL) && (voltage > TURBIDITY_VOLTAGE_RANGE_FLOOR);
}

int openSolenoid() {
  if (!isSolenoidOpen) {
    Serial.println("INFO: opening solenoid valve");
    digitalWrite(SOLENOID_PIN, HIGH);
    pulseCount = 0;
    totalCaptureVolume = 0.0;
    totalTimeValveOpen = 0.0;
    isSolenoidOpen = true;
  } else {
    Serial.println("WARN: attempted to open solenoid valve while already open");
  }

  return 0;
}

int closeSolenoid() {
  if (isSolenoidOpen) {
    Serial.println("INFO: closing solenoid valve");
    digitalWrite(SOLENOID_PIN, LOW);
    isSolenoidOpen = false;
  } else {
    Serial.println("WARN: attempted to close solenoid valve while already closed");
  }

  return 0;
}

// scales 0-1023 analog reading to 0.00-5.00 volts 
float convertTurbiditySensorValueToVoltage(int sensorValue) {
  return sensorValue * (5.0 / 1024.0);
}

void flowPulseCounter() {
  pulseCount++;
}

