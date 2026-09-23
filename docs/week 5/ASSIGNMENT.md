# OBJECTIVE
Control LEDs using inputs (buttons)
Understand pull-up / pull-down resistors
Use conditional logic in code 

# Task overview 
You are designing a basic control panel using:
- LEDs (output)
- Push buttons (input)
The system should respond to user interaction in different ways.
You will be required to complete the assignment in TinkerCAD:
1. Create an account at TinkerCAD
2. Create new Circuits project.
3. From the right panel find and place the items you can identify in this image below.

![alt text](image.png)

# part 1:
# Button controlled LED

## source code
'''
// Pin definitions
const int buttonPin = 2;
const int ledPin = 13;

void setup() {
  pinMode(buttonPin, INPUT);   // Using external pull-down resistor
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  // Read button state
  int buttonState = digitalRead(buttonPin);

  // PART 1: BASIC CONTROL
  // If button is pressed → turn LED ON
  // Else → turn LED OFF
  if (buttonState == HIGH) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}
'''
# Explanation.
## pin definitions.

const int buttonPin = 2;
const int ledPin = 13;

## setup function
'''
pinMode(buttonPin, INPUT);   
pinMode(ledPin, OUTPUT);
Serial.begin(9600);
Serial.begin(9600) initializes serial communication (useful for debugging).
'''

## Loop function
int buttonState = digitalRead(buttonPin);

## LED control function

'''
if (buttonState == HIGH) {
  digitalWrite(ledPin, HIGH);
} else {
  digitalWrite(ledPin, LOW);
}
'''

# part 2
# Toggle systems(state changes)
## source code
'''
// Pin definitions
const int buttonPin = 2;
const int ledPin = 13;

// ==============================
// GLOBAL VARIABLES
// ==============================

// Variable to store LED state (ON/OFF)
int ledState = LOW;          // LED starts OFF

// Variable to store last button state
int lastButtonState = LOW;   // Previous button reading

// ==============================

void setup() {
  pinMode(buttonPin, INPUT);   // Using external pull-down resistor
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

// ==============================

void loop() {

  // Read current button state
  int buttonState = digitalRead(buttonPin);

  // ==========================
  // TOGGLE LOGIC
  // ==========================

  // Detect rising edge (LOW -> HIGH)
  if (buttonState == HIGH && lastButtonState == LOW) {

    // Flip LED state (if ON → OFF, if OFF → ON)
    ledState = !ledState;
  }

  // Write LED state to LED pin
  digitalWrite(ledPin, ledState);

  // Update last button state
  lastButtonState = buttonState;
}
'''

## Explanation
- pin definitions
const int buttonPin = 2;
const int ledPin = 13;

- Global Variables
int ledState = LOW;  
int lastButtonState = LOW;

- setup function()
void setup() {
  pinMode(buttonPin, INPUT);   // Using external pull-down resistor
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

- loop function()
int buttonState = digitalRead(buttonPin);

- Toggle logic
if (buttonState == HIGH && lastButtonState == LOW) {
    ledState = !ledState;
}

- Write the LED state
digitalWrite(ledPin, ledState);
