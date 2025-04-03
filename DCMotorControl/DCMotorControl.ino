#include <util/atomic.h>

// pin configuration
const int btn1Pin = 6;
const int btn2Pin = 7;
const int PWM = 9;
const int AIN1 = 12;
const int AIN2 = 13;
const int encoderPin1 = 2;
const int encoderPin2 = 3;

const int complRot = 413;  // pulses per rotation
volatile int pos_i = 0;
volatile int pos = 0;
volatile int lastEncoded = 0;
int pwmValue = 0;
int countRotations = 0;
int rotationRef = 5;  // set initial rotation refference
int target = -413; // initial position
bool reverseMode = false;
bool motorStopped = false;
int lastPosition = 0; // Keeps the last position reached
int button1State = 0, button2State = 0;

// PID gains
float proportional = 0.01;
float integral = 0.0001;
float derivative = 0;

// PID computing parameters
float controlSignal = 0;
float previousTime = 0, previousError = 0, errorIntegral = 0;

// speed calculation parameters
long prevT = 0;
int posPrev = 0;
float v = 0;

void setup() {
    Serial.begin(9600);
    pinMode(btn1Pin, INPUT);
    pinMode(btn2Pin, INPUT);
    pinMode(PWM, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(encoderPin1, INPUT_PULLUP);
    pinMode(encoderPin2, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(encoderPin1), updateEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encoderPin2), updateEncoder, CHANGE);
}

void loop() {

    // wait the serial value for the number of rotations to make
    if (Serial.available() > 0) {
        String receivedData = Serial.readStringUntil('\n');
        if (receivedData.length() > 0) {
            int newRotationRef = receivedData.toInt();  // Citim valoarea trimisă de Processing
            if (newRotationRef > 0) {  
                rotationRef = newRotationRef;
                countRotations = 1;
                reverseMode = false;
                motorStopped = false;

                Serial.print("New Rotation Reference Set: ");
                Serial.println(rotationRef);
            }
        }
    }

    if (motorStopped) return;

    // change the PWM value
    button1State = digitalRead(btn1Pin);
    button2State = digitalRead(btn2Pin);

    if (button1State == HIGH && pwmValue < 255) pwmValue += 1;
    if (button2State == HIGH && pwmValue > 30) pwmValue -= 1;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

    // count the rotations until rotation refference is reached
        if (!reverseMode) {
            if (pos_i >= complRot) {
                countRotations++;
                pos_i = 0;
            }
            if (countRotations >= rotationRef) {
                reverseMode = true;
            }
        }
        // calculate speed
      long currT = micros();
      float deltaT = ((float) (currT-prevT))/1.0e6;
      float velocity = (pos_i - posPrev)/deltaT;
      posPrev = pos_i;
      prevT = currT;
      v = velocity/413.0*60.0;
    }

    //if reverse mode is not activated, continue rotating counterclockwise
    if (!reverseMode) {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
    } else {

        // if the desired number of rotations is reached, apply PID control for position
        if (pos_i != target) {
            calculatePID();
            pwmValue = constrain(fabs(controlSignal), 25, 255);
            
            if (pos_i > target) {
                digitalWrite(AIN1, HIGH);
                digitalWrite(AIN2, LOW);
            } else {
                digitalWrite(AIN1, LOW);
                digitalWrite(AIN2, HIGH);
            }
        } else {
            stopMotor();
        }
    }
    analogWrite(PWM, pwmValue);

    // calculate position (degrees)
    float angle = (pos_i % 413) * (360.0 / 413.0);  

    // write the values in the serial monitor
    Serial.print("Speed: ");
    Serial.println(v);  
    Serial.print("Position: ");
    Serial.println(angle);  
    Serial.print("Rotations Made: ");
    Serial.println(countRotations);
}

// function for updating the encoder counter (depending on the direction)
void updateEncoder() {
    int MSB = digitalRead(encoderPin1);
    int LSB = digitalRead(encoderPin2);
    int encoded = (MSB << 1) | LSB;
    int sum = (lastEncoded << 2) | encoded;
    
    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        pos_i--;
    }
    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        pos_i++;
    }
    
    lastEncoded = encoded;
    checkPositionBounds();
}

// logic for calculating the number of rotations made 
void checkPositionBounds() {
    if (pos_i >= complRot) {
        pos_i = 0;
        countRotations++;
    } 
}

// PID implementation
void calculatePID() {
    float currentTime = micros();
    float deltaTime = (currentTime - previousTime) / 1000000.0;
    if (deltaTime < 0.0001) return;
    
    previousTime = currentTime;
    float errorValue = pos_i - target;
    float edot = (errorValue - previousError) / deltaTime;
    errorIntegral += errorValue * deltaTime;

    controlSignal = (proportional * errorValue) + (derivative * edot) + (integral * errorIntegral);
    previousError = errorValue;
}

// final function after all sequences
void stopMotor() {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    motorStopped = true;
    lastPosition = pos_i;
    v = 0;
    
}