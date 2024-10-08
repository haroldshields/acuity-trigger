// Arduino Due Square Wave Generator with RS-232 Interface

// Pin for outputting the square wave
const int wavePin = 13;

// Variables for frequency and signal generation
volatile unsigned int frequency = 5; // Default frequency 1000Hz
volatile bool isGenerating = false;     // Flag to control wave generation
unsigned long highTime;                 // High part of the signal in microseconds

// Timing constants
const unsigned long lowTime = 70;       // Low part duration in microseconds

void setup() {
  // Set up wave pin as output
  pinMode(wavePin, OUTPUT);
  digitalWrite(wavePin, LOW);

  // Set up serial communication
  Serial.begin(9600);

  // Calculate initial high time based on the default frequency
  calculateHighTime();
}

void loop() {
  // Check for incoming serial data
  if (Serial.available() > 0) {
    handleCommand();
  }

  // Generate the square wave if the flag is set
  if (isGenerating) {
    generateSquareWave();
  }
}

// Function to handle incoming RS-232 commands
void handleCommand() {
  delay(100); // wait for more to showup.
  if (Serial.available() == 5) {
    char command = Serial.read(); // read the first byte
    if (command == 'f') {
      // the next 4 are the frequency
      char buffer[5];
      Serial.readBytes(buffer, 4);
      buffer[4] = '\0';
      int tempFrequency = atoi(buffer);
      if (tempFrequency >= 1 && tempFrequency <= 4500) {
        frequency = tempFrequency;
        calculateHighTime();
        Serial.print(command);
        Serial.println(buffer);  // Acknowledge the set frequency command
      } else {
        Serial.println("Frequency out of range");
      }
    } else {
      Serial.println("Invalid command: " + String(command));
    }     
  } else if (Serial.available() == 2) {
    String command = Serial.readString();
//     Serial.println(command);
    if (command == "f?") {
      Serial.println(command + String(frequency));
    } else {
      Serial.println("Invalid command: " + command);
    }
  } else if (Serial.available() == 1) {
    char command = Serial.read();   
    if (command == 'e') {
      Serial.println("command is e");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(wavePin, HIGH);
      isGenerating = true;
    } else if (command == 'h') {
      Serial.println("command is h");
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(wavePin, LOW);
      isGenerating = false;
//      digitalWrite(wavePin, LOW);  // Ensure the signal stops in LOW state
    } else {
      Serial.println("Invalid command: " + String(command));
    }
  } else { // not 5, 2 or 1
    String command = Serial.readString();
    Serial.println("Invalid command: " + command);
  }
}

// Function to calculate the high time based on the current frequency
void calculateHighTime() {
  // Period in microseconds = 1,000,000 / frequency
  unsigned long period = 1000000 / frequency;
  
  // High time is the period minus the low time (80 microseconds)
  if (period > lowTime) {
    highTime = period - lowTime;
  } else {
    highTime = 0; // Prevent negative or invalid high time
  }
}

// Function to generate the square wave signal
void generateSquareWave() {
  // Set the pin HIGH for the calculated high time
  digitalWrite(LED_BUILTIN, HIGH);
  delayMicroseconds(highTime);

  // Set the pin LOW for the constant low time
  digitalWrite(LED_BUILTIN, LOW);
  delayMicroseconds(lowTime);
}
