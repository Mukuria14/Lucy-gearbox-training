# 16/03/26 - Monday Assignment
## Student Record Management System in C
## Course: Introduction to C Programming
## Topic: Structs, Enums, Unions, and Dynamic Memory Allocation


###  struct
a container that groups related pieces of data.
Here, one student has several pieces of data, so Student should be a struct.
Example idea:

* name
* age
* program
* academic info
* enum

###  enum 
  for fixed choices.
Here the choices are only:
* UNDERGRADUATE
* POSTGRADUATE
This helps your code know which kind of student it is.

### union

A union lets different variables share the same memory space.
Here, a student only needs one of these:
gpa
research_hours
So instead of storing both, the program stores only the one that matches the student type.

### malloc

malloc() creates memory while the program is running.
Since the number of students is entered by the user, you do not know array size in advance.
So you must do something like:
Student *students = malloc(n * sizeof(Student));
free

### free() 
gives that memory back when you are done.
Without it, memory stays allocated unnecessarily.

below is the code:

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================= ENUM ================= */ 
// TODO: Define an enum called ProgramType
// It should have:
// UNDERGRADUATE
// POSTGRADUATE
typedef enum {
// TODO: add values here
} ProgramType;

/* ================= UNION ================= */
// TODO: Define a union called AcademicInfo
// It should store either:
// - float gpa
// - int research_hours
typedef union {
// TODO: add members here
} AcademicInfo;


/* ================= STRUCT ================= */
// TODO: Define a struct called Student
// It should contain:
// - name (string)
// - age (int)
// - program (enum ProgramType)
// - info (union AcademicInfo)
typedef struct {
// TODO: add fields here
} Student;

/* ================= FUNCTION PROTOTYPES ================= */
void inputStudent(Student *s);
void displayStudent(Student s);


/*================= MAIN FUNCTION ================= */
int main() {
int n;
printf("Enter number of students: ");
scanf("%d", &n);


// TODO: Dynamically allocate memory for n students
Student *students = malloc(n * sizeof(Student));
if (students == NULL) {
    printf("Memory allocation failed!\n");
    return 1;
}

// TODO: Check if memory allocation failed
// TODO: Loop to input student data
for (int i = 0; i < n; i++) {

printf("\n--- Student %d ---\n", i + 1);
inputStudent(&students[i]);
}
// TODO: Display all student records
printf("\n===== Student Records =====\n");
for (int i = 0; i < n; i++) {
displayStudent(students[i]);
}
// TODO: Free allocated memory
free(students);
return 0;


}
/* ================= INPUT FUNCTION ================= */
void inputStudent(Student *s) {
// TODO: Input name
printf("Enter name: ");
// Hint: use scanf(" %[^\n]", ...)
/
// TODO: Based on program:
// If Undergraduate → input GPA
// If Postgraduate → input research hours
}
/* ================= DISPLAY FUNCTION ================= */
void displayStudent(Student s) {
printf("\nName: %s\n", s.name);
printf("Age: %d\n", s.age);
// TODO: Display program type
// TODO: Display either GPA or research hours
}
```

# 18/03/26 wednesday 

### LED_Blink_Lesson 1:
first embedded program. I learnT how code turns a GPIO pin on and off to blink an LED.

### DHT22_Deep_Lesson 2:
Instead of controlling hardware, I  read a real sensor and learnt C code, memory, timing, GPIO, checksum, and serial output all work together.
carried out the simulation on wowki and run this code.


```
#include <Arduino.h>

#define DHT22_PIN 15
#define LED_PIN 25
#define DELAY_MS 1000
#define TIMEOUT_US 1000

struct DHT22Data {
  int32_t temperature; // tenths of C, e.g. 251 = 25.1C
  int32_t humidity;    // tenths of %, e.g. 653 = 65.3%
  bool valid;
};

uint32_t failedReadings = 0;

int waitForLevel(uint8_t pin, uint8_t level, uint32_t timeoutUs) {
  uint32_t start = micros();
  while (digitalRead(pin) != level) {
    if ((micros() - start) >= timeoutUs) {
      return -1;
    }
  }
  return (int)(micros() - start);
}

DHT22Data readDHT22() {
  DHT22Data result = {0, 0, false};
  uint8_t bits[40] = {0};
  uint8_t bytes[5] = {0};

  // Start signal
  pinMode(DHT22_PIN, OUTPUT);
  digitalWrite(DHT22_PIN, LOW);
  delay(1); // 1 ms
  digitalWrite(DHT22_PIN, HIGH);
  delayMicroseconds(30);
  pinMode(DHT22_PIN, INPUT_PULLUP);

  // Sensor response: LOW 80us, HIGH 80us
  if (waitForLevel(DHT22_PIN, LOW, TIMEOUT_US) < 0) return result;
  if (waitForLevel(DHT22_PIN, HIGH, TIMEOUT_US) < 0) return result;
  if (waitForLevel(DHT22_PIN, LOW, TIMEOUT_US) < 0) return result;

  // Read 40 bits
  for (int i = 0; i < 40; i++) {
    if (waitForLevel(DHT22_PIN, HIGH, TIMEOUT_US) < 0) return result;

    uint32_t startHigh = micros();
    if (waitForLevel(DHT22_PIN, LOW, TIMEOUT_US) < 0) return result;
    uint32_t highDuration = micros() - startHigh;

    bits[i] = (highDuration > 40) ? 1 : 0;
  }

  // Pack bits into bytes
  for (int b = 0; b < 5; b++) {
    for (int bit = 0; bit < 8; bit++) {
      bytes[b] = (bytes[b] << 1) | bits[b * 8 + bit];
    }
  }

  // Checksum
  uint8_t checksum = (bytes[0] + bytes[1] + bytes[2] + bytes[3]) & 0xFF;
  if (checksum != bytes[4]) {
    return result;
  }

  // Humidity in tenths
  result.humidity = ((uint16_t)bytes[0] << 8) | bytes[1];

  // Temperature in tenths
  int32_t tempRaw = (((uint16_t)(bytes[2] & 0x7F)) << 8) | bytes[3];
  if (bytes[2] & 0x80) {
    tempRaw = -tempRaw;
  }
  result.temperature = tempRaw;

  result.valid = true;
  return result;
}

const char* getStatus(int32_t tempC, int32_t humidity) {
  if (tempC >= 200 && tempC <= 250 && humidity >= 400 && humidity <= 600) {
    return "PERFECT";
  }
  if (tempC > 250) {
    return "TOO HOT";
  }
  if (tempC < 200) {
    return "TOO COLD";
  }
  return "Comfortable";
}

void updateLed(const char* status) {
  if (strcmp(status, "TOO HOT") == 0) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  } else if (strcmp(status, "TOO COLD") == 0) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

void printTenths(int32_t value) {
  if (value < 0) {
    Serial.print("-");
    value = -value;
  }
  Serial.print(value / 10);
  Serial.print(".");
  Serial.print(value % 10);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(DHT22_PIN, INPUT_PULLUP);

  Serial.println("DHT22 Assignment Demo");
  Serial.println("Note: DELAY_MS is 1000 ms for the assignment, though DHT22 normally prefers >2 seconds.");
}

void loop() {
  DHT22Data data = readDHT22();

  if (!data.valid) {
    failedReadings++;
    Serial.println("Failed to read from DHT22");
    Serial.print("Failed readings: ");
    Serial.println(failedReadings);
    digitalWrite(LED_PIN, LOW);
    delay(DELAY_MS);
    return;
  }

  const char* status = getStatus(data.temperature, data.humidity);

  // Fahrenheit in tenths: F = (C * 9/5) + 32
  int32_t tempF = (data.temperature * 9) / 5 + 320;

  Serial.print("Temperature: ");
  printTenths(data.temperature);
  Serial.print(" C | ");

  Serial.print("Fahrenheit: ");
  printTenths(tempF);
  Serial.print(" F | ");

  Serial.print("Humidity: ");
  printTenths(data.humidity);
  Serial.print(" % | ");

  Serial.print("Status: ");
  Serial.println(status);

  Serial.print("Failed readings: ");
  Serial.println(failedReadings);

  if (strcmp(status, "Comfortable") == 0 || strcmp(status, "PERFECT") == 0) {
    digitalWrite(LED_PIN, LOW);
    delay(DELAY_MS);
  } else {
    updateLed(status);
  }
}
```

## Wokwi Circuit Diagram
[![](https://wokwi.com/api/screenshot?url=https://wokwi.com/projects/462003683806887937)](https://wokwi.com/projects/462003683806887937)


### Physics_of_Embedded_Systems 3:
The deep “why.” It explains what is physically happening underneath the code: transistors, logic gates, registers, memory, timing, and how a sensor reading becomes a number.
