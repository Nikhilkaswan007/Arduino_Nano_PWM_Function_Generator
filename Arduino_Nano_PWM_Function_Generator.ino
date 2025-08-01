#include <LiquidCrystal_I2C.h>
#include <math.h>

// LCD setup (I2C address 0x27, 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
#define PWM_PIN 9           // PWM output pin
#define ENCODER_CLK 2       // Rotary encoder CLK
#define ENCODER_DT 3        // Rotary encoder DT
#define ENCODER_SW 4        // Rotary encoder button
#define MODE_BUTTON 5       // Mode change button

// Waveform types
enum WaveType {
  SINE,
  SQUARE,
  TRIANGLE,
  SAWTOOTH
};

// Global variables
WaveType currentWave = SINE;
volatile int encoderPos = 0;
volatile bool encoderChanged = false;
int lastCLK = HIGH;
bool lastButtonState = HIGH;
bool lastModeButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long lastModeDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Waveform parameters
float frequency = 1.0;      // Hz
int amplitude = 255;        // PWM amplitude (0-255)
int offset = 127;           // DC offset
int phase = 0;              // Phase shift

// Menu system
enum MenuState {
  FREQ_MENU,
  AMP_MENU,
  OFFSET_MENU,
  PHASE_MENU
};

MenuState currentMenu = FREQ_MENU;
const char* waveNames[] = {"SINE", "SQUARE", "TRIANGLE", "SAWTOOTH"};
const char* menuNames[] = {"FREQ", "AMP", "OFFSET", "PHASE"};

// Timing variables
unsigned long lastUpdate = 0;
float timeStep = 0;
const float sampleRate = 1000.0; // 1kHz sampling rate

void setup() {
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Function Gen v1.0");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  
  // Setup pins
  pinMode(PWM_PIN, OUTPUT);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  
  // Setup interrupts for encoder
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), encoderISR, CHANGE);
  
  // Setup Timer1 for precise PWM generation
  setupTimer1();
  
  updateDisplay();
}

void loop() {
  handleEncoder();
  handleButtons();
  generateWaveform();
  
  if (encoderChanged) {
    updateParameters();
    updateDisplay();
    encoderChanged = false;
  }
}

void encoderISR() {
  int CLK = digitalRead(ENCODER_CLK);
  int DT = digitalRead(ENCODER_DT);
  
  if (CLK != lastCLK) {
    if (DT != CLK) {
      encoderPos++;
    } else {
      encoderPos--;
    }
    encoderChanged = true;
  }
  lastCLK = CLK;
}

void handleEncoder() {
  // Handle encoder button press (parameter selection)
  bool encoderButton = digitalRead(ENCODER_SW);
  if (encoderButton != lastButtonState) {
    if (millis() - lastDebounceTime > debounceDelay) {
      if (encoderButton == LOW) {
        // Cycle through menu options
        currentMenu = (MenuState)((currentMenu + 1) % 4);
        updateDisplay();
      }
      lastDebounceTime = millis();
    }
  }
  lastButtonState = encoderButton;
}

void handleButtons() {
  // Handle mode button press (waveform type)
  bool modeButton = digitalRead(MODE_BUTTON);
  if (modeButton != lastModeButtonState) {
    if (millis() - lastModeDebounceTime > debounceDelay) {
      if (modeButton == LOW) {
        // Cycle through waveform types
        currentWave = (WaveType)((currentWave + 1) % 4);
        updateDisplay();
      }
      lastModeDebounceTime = millis();
    }
  }
  lastModeButtonState = modeButton;
}

void updateParameters() {
  switch (currentMenu) {
    case FREQ_MENU:
      frequency += encoderPos * 0.1;
      if (frequency < 0.1) frequency = 0.1;
      if (frequency > 100.0) frequency = 100.0;
      break;
      
    case AMP_MENU:
      amplitude += encoderPos * 5;
      if (amplitude < 0) amplitude = 0;
      if (amplitude > 255) amplitude = 255;
      break;
      
    case OFFSET_MENU:
      offset += encoderPos * 5;
      if (offset < 0) offset = 0;
      if (offset > 255) offset = 255;
      break;
      
    case PHASE_MENU:
      phase += encoderPos * 5;
      if (phase < 0) phase = 0;
      if (phase > 360) phase = 360;
      break;
  }
  encoderPos = 0;
}

void generateWaveform() {
  if (millis() - lastUpdate >= (1000.0 / sampleRate)) {
    int pwmValue = 0;
    float t = timeStep;
    float omega = 2 * PI * frequency;
    float phaseRad = phase * PI / 180.0;
    
    switch (currentWave) {
      case SINE:
        pwmValue = offset + (amplitude/2) * sin(omega * t + phaseRad);
        break;
        
      case SQUARE:
        pwmValue = offset + (sin(omega * t + phaseRad) >= 0 ? amplitude/2 : -amplitude/2);
        break;
        
      case TRIANGLE:
        {
          float triWave = (2.0/PI) * asin(sin(omega * t + phaseRad));
          pwmValue = offset + (amplitude/2) * triWave;
        }
        break;
        
      case SAWTOOTH:
        {
          float sawWave = 2 * (omega * t + phaseRad)/(2*PI) - floor(2 * (omega * t + phaseRad)/(2*PI)) - 1;
          pwmValue = offset + (amplitude/2) * sawWave;
        }
        break;
    }
    
    // Constrain PWM value
    pwmValue = constrain(pwmValue, 0, 255);
    analogWrite(PWM_PIN, pwmValue);
    
    timeStep += 1.0 / sampleRate;
    lastUpdate = millis();
  }
}

void setupTimer1() {
  // Setup Timer1 for higher frequency PWM
  // This provides better resolution for waveform generation
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(CS10);
  ICR1 = 255; // 8-bit resolution
}

void updateDisplay() {
  lcd.clear();
  
  // First line: Waveform type and frequency
  lcd.setCursor(0, 0);
  lcd.print(waveNames[currentWave]);
  lcd.setCursor(8, 0);
  lcd.print(frequency, 1);
  lcd.print("Hz");
  
  // Second line: Current parameter and value
  lcd.setCursor(0, 1);
  lcd.print(">");
  lcd.print(menuNames[currentMenu]);
  lcd.print(":");
  
  switch (currentMenu) {
    case FREQ_MENU:
      lcd.setCursor(7, 1);
      lcd.print(frequency, 1);
      lcd.print("Hz");
      break;
      
    case AMP_MENU:
      lcd.setCursor(7, 1);
      lcd.print(amplitude);
      break;
      
    case OFFSET_MENU:
      lcd.setCursor(7, 1);
      lcd.print(offset);
      break;
      
    case PHASE_MENU:
      lcd.setCursor(7, 1);
      lcd.print(phase);
      lcd.print("°");
      break;
  }
}

// Additional utility functions for advanced features
void savePreset(int presetNumber) {
  // Could be extended to save parameters to EEPROM
  Serial.print("Saving preset ");
  Serial.println(presetNumber);
}

void loadPreset(int presetNumber) {
  // Could be extended to load parameters from EEPROM
  Serial.print("Loading preset ");
  Serial.println(presetNumber);
}

// Function to output current parameters via Serial
void printStatus() {
  Serial.println("=== Function Generator Status ===");
  Serial.print("Waveform: ");
  Serial.println(waveNames[currentWave]);
  Serial.print("Frequency: ");
  Serial.print(frequency);
  Serial.println(" Hz");
  Serial.print("Amplitude: ");
  Serial.println(amplitude);
  Serial.print("Offset: ");
  Serial.println(offset);
  Serial.print("Phase: ");
  Serial.print(phase);
  Serial.println("°");
  Serial.println("==============================");
}