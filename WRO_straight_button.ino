#include <AccelStepper.h>
#include <math.h>

AccelStepper lMotor(AccelStepper::DRIVER, 7, 6);    // left motor: STEP, DIR
AccelStepper rMotor(AccelStepper::DRIVER, 11, 10);  // right motor: STEP, DIR

const int lEnable = 4;
const int rEnable = 9;

const int Lms1 = 33;
const int Lms2 = 35;
const int Lms3 = 37;

const int Rms1 = 32;
const int Rms2 = 34;
const int Rms3 = 36;

const int buttonPin = 28;

// Motion settings
const int STEP_MODE = 1;                    // 1=full, 2=half, 3=eighth, 4=sixteenth
const float TARGET_DISTANCE_MM = 1600.0f;  // change this to the distance you want
const float MAX_SPEED = 400.0f;             // conservative to avoid squeal/stall
const float ACCELERATION = 200.0f;

// Robot geometry
const float WHEEL_DIAMETER_MM = 88.0f;
const float WHEEL_CIRCUMFERENCE_MM = PI * WHEEL_DIAMETER_MM;

// Hardware options
const bool ENABLE_ACTIVE_LOW = true;   // most stepper drivers use active-low enable
const bool USE_INTERNAL_PULLUP = false;
const bool BUTTON_ACTIVE_STATE = !USE_INTERNAL_PULLUP;
const unsigned long BUTTON_DEBOUNCE_MS = 50;

long stepsPerRev = 200;

bool run = false;
bool lastButtonState = LOW;

bool moveInProgress = false;
bool moveFinished = false;

void setStepMode(int stepMode) {
  switch (stepMode) {
    case 1:
      stepsPerRev = 200;
      digitalWrite(Rms1, LOW);
      digitalWrite(Rms2, LOW);
      digitalWrite(Rms3, LOW);
      digitalWrite(Lms1, LOW);
      digitalWrite(Lms2, LOW);
      digitalWrite(Lms3, LOW);
      break;

    case 2:
      stepsPerRev = 400;
      digitalWrite(Rms1, HIGH);
      digitalWrite(Rms2, LOW);
      digitalWrite(Rms3, LOW);
      digitalWrite(Lms1, HIGH);
      digitalWrite(Lms2, LOW);
      digitalWrite(Lms3, LOW);
      break;

    case 3:
      stepsPerRev = 800;
      digitalWrite(Rms1, HIGH);
      digitalWrite(Rms2, HIGH);
      digitalWrite(Rms3, LOW);
      digitalWrite(Lms1, HIGH);
      digitalWrite(Lms2, HIGH);
      digitalWrite(Lms3, LOW);
      break;

    case 4:
      stepsPerRev = 1600;
      digitalWrite(Rms1, HIGH);
      digitalWrite(Rms2, HIGH);
      digitalWrite(Rms3, HIGH);
      digitalWrite(Lms1, HIGH);
      digitalWrite(Lms2, HIGH);
      digitalWrite(Lms3, HIGH);
      break;

    default:
      setStepMode(1);
      break;
  }
}

long stepsForDistanceMm(float distanceMm) {
  return (long)roundf((distanceMm / WHEEL_CIRCUMFERENCE_MM) * (float)stepsPerRev);
}

void enableMotors() {
  lMotor.enableOutputs();
  rMotor.enableOutputs();
}

void disableMotors() {
  lMotor.disableOutputs();
  rMotor.disableOutputs();
}

void configureMotors() {
  lMotor.setEnablePin(lEnable);
  rMotor.setEnablePin(rEnable);

  // With active-low drivers, this makes enableOutputs() drive the enable pin LOW.
  lMotor.setPinsInverted(false, false, ENABLE_ACTIVE_LOW);
  rMotor.setPinsInverted(false, false, ENABLE_ACTIVE_LOW);

  lMotor.setMinPulseWidth(20);
  rMotor.setMinPulseWidth(20);

  lMotor.setMaxSpeed(MAX_SPEED);
  rMotor.setMaxSpeed(MAX_SPEED);
  lMotor.setAcceleration(ACCELERATION);
  rMotor.setAcceleration(ACCELERATION);
}

void startStraightMove() {
  long targetSteps = stepsForDistanceMm(TARGET_DISTANCE_MM);

  lMotor.setCurrentPosition(0);
  rMotor.setCurrentPosition(0);

  enableMotors();

  // On this robot, right wheel forward = positive, left wheel forward = negative.
  rMotor.moveTo(targetSteps);
  lMotor.moveTo(-targetSteps);

  moveInProgress = true;
  moveFinished = false;

  Serial.print("Starting straight move, mm: ");
  Serial.println(TARGET_DISTANCE_MM);
  Serial.print("Target steps: ");
  Serial.println(targetSteps);
}

void runStraightMove() {
  if (!moveInProgress) {
    return;
  }

  lMotor.run();
  rMotor.run();

  if (lMotor.distanceToGo() == 0 && rMotor.distanceToGo() == 0) {
    moveInProgress = false;
    moveFinished = true;
    disableMotors();
    Serial.println("Move complete");
  }
}

void total_op() {
  startStraightMove();
}

void setup() {
  Serial.begin(115200);

  pinMode(Lms1, OUTPUT);
  pinMode(Lms2, OUTPUT);
  pinMode(Lms3, OUTPUT);
  pinMode(Rms1, OUTPUT);
  pinMode(Rms2, OUTPUT);
  pinMode(Rms3, OUTPUT);

  pinMode(lEnable, OUTPUT);
  pinMode(rEnable, OUTPUT);

  pinMode(buttonPin, USE_INTERNAL_PULLUP ? INPUT_PULLUP : INPUT);

  setStepMode(STEP_MODE);
  configureMotors();

  disableMotors();

  Serial.println("Ready");
  Serial.println("Press the button to drive straight");
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin);

  if (currentButtonState == HIGH && lastButtonState == LOW) {
    run = !run;

    if (run && !moveInProgress) {
      total_op();
    }

    delay(500);
  }

  lastButtonState = currentButtonState;

  if (run) {
    runStraightMove();
  }
}
