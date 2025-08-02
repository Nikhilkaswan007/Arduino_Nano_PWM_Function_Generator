# Arduino Nano PWM Function Generator

A versatile function generator built with Arduino Nano that generates multiple waveforms using PWM output. Features rotary encoder control, LCD display, and multiple signal types perfect for electronics testing and prototyping.

## Features

### 🌊 Multiple Waveforms
- **Sine Wave** - Smooth sinusoidal output
- **Square Wave** - Digital logic testing
- **Triangle Wave** - Linear ramp signals
- **Sawtooth Wave** - Sweep and timing applications

### 🎛️ Interactive Controls
- **Rotary Encoder** - Smooth parameter adjustment
- **Mode Button** - Quick waveform switching
- **LCD Display** - Real-time parameter monitoring
- **Serial Output** - Debug and monitoring capabilities

### ⚙️ Adjustable Parameters
- **Frequency**: 0.1 to 100 Hz (0.1 Hz resolution)
- **Amplitude**: 0 to 255 PWM levels
- **DC Offset**: 0 to 255 PWM levels  
- **Phase Shift**: 0 to 360 degrees

## Hardware Requirements

### Components List
| Component | Quantity | Notes |
|-----------|----------|-------|
| Arduino Nano | 1 | Main microcontroller |
| I2C LCD 16x2 | 1 | Display module (0x27 address) |
| Rotary Encoder | 1 | With push button |
| Push Button | 1 | Mode selection |
| Breadboard | 1 | For prototyping |
| Jumper Wires | Various | Connections |
| Pull-up Resistors | 2x 10kΩ | For buttons (if needed) |

### Pin Connections

```
Arduino Nano    Component           Pin/Connection
─────────────   ─────────────       ──────────────
Pin 9           PWM Output          Signal Output
Pin 2           Encoder CLK         Rotary Encoder
Pin 3           Encoder DT          Rotary Encoder  
Pin 4           Encoder SW          Encoder Button
Pin 5           Mode Button         External Button
A4 (SDA)        LCD SDA             I2C Data
A5 (SCL)        LCD SCL             I2C Clock
5V              LCD VCC             Power
GND             LCD GND             Ground
```

## Circuit Diagram

```
         Arduino Nano
    ┌─────────────────────┐
    │                     │
    │  A5 ──────────────  │ SCL (to LCD)
    │  A4 ──────────────  │ SDA (to LCD)
    │                     │
    │  Pin 9 ────────────  │ PWM Output
    │                     │
    │  Pin 5 ────────────  │ Mode Button
    │  Pin 4 ────────────  │ Encoder Button
    │  Pin 3 ────────────  │ Encoder DT
    │  Pin 2 ────────────  │ Encoder CLK
    │                     │
    │  5V ───────────────  │ Power
    │  GND ──────────────  │ Ground
    └─────────────────────┘
```

## Software Installation

### Required Libraries
Install these libraries through Arduino IDE Library Manager:

```cpp
#include <LiquidCrystal_I2C.h>  // For I2C LCD control
#include <math.h>               // For mathematical functions
```

### Installation Steps
1. **Install Arduino IDE** (version 1.8.0 or newer)
2. **Install Required Libraries**:
   - Go to `Sketch → Include Library → Manage Libraries`
   - Search for "LiquidCrystal I2C" by Frank de Brabander
   - Click Install
3. **Upload Code**:
   - Connect Arduino Nano via USB
   - Select `Tools → Board → Arduino Nano`
   - Select correct COM port
   - Upload the code

## Usage Instructions

### Basic Operation
1. **Power On**: Connect Arduino to USB or external 5V supply
2. **LCD Display**: Shows current waveform type and parameters
3. **Select Waveform**: Press mode button to cycle through wave types
4. **Adjust Parameters**: 
   - Press encoder button to select parameter (FREQ/AMP/OFFSET/PHASE)
   - Rotate encoder to adjust selected parameter
5. **Monitor Output**: Connect PWM output (Pin 9) to oscilloscope or load

### Display Layout
```
Line 1: [WAVEFORM] [FREQUENCY]
Line 2: >[PARAMETER]: [VALUE]
```

Example:
```
SINE      5.5Hz
>FREQ: 5.5Hz
```

### Parameter Ranges
| Parameter | Range | Step Size | Units |
|-----------|-------|-----------|-------|
| Frequency | 0.1 - 100 | 0.1 | Hz |
| Amplitude | 0 - 255 | 5 | PWM Levels |
| Offset | 0 - 255 | 5 | PWM Levels |
| Phase | 0 - 360 | 5 | Degrees |

## Technical Specifications

### Performance
- **Sample Rate**: 1 kHz
- **PWM Resolution**: 8-bit (0-255)
- **Frequency Range**: 0.1 - 100 Hz
- **Waveform Accuracy**: ±1% at low frequencies
- **Update Rate**: Real-time parameter changes

### Output Characteristics
- **Output Voltage**: 0-5V (PWM levels)
- **Output Impedance**: ~500Ω (Arduino pin)
- **Drive Capability**: 20mA max per pin
- **Signal Type**: PWM (requires low-pass filter for analog)

## Advanced Features

### Serial Monitor Commands
Open Serial Monitor (9600 baud) for debugging:
- View real-time parameter updates
- Monitor waveform generation status
- Debug encoder and button responses

### Code Customization
The code is modular and easily extensible:

```cpp
// Add new waveforms in generateWaveform() function
// Modify parameter ranges in updateParameters()
// Add EEPROM storage with savePreset()/loadPreset()
```

### Potential Enhancements
- **EEPROM Storage**: Save/load parameter presets
- **Frequency Sweep**: Automatic frequency sweeping
- **AM/FM Modulation**: Advanced signal modulation
- **Higher Frequencies**: Using Timer2 for kHz range
- **External DAC**: True analog output via SPI DAC

## Troubleshooting

### Common Issues

| Problem | Possible Cause | Solution |
|---------|----------------|----------|
| No LCD display | Wrong I2C address | Try 0x3F instead of 0x27 |
| Encoder not responding | Wiring error | Check pin connections |
| No PWM output | Pin 9 not connected | Verify PWM pin wiring |
| Unstable readings | Power supply noise | Add decoupling capacitors |
| Frequency inaccurate | Timing issues | Check crystal oscillator |

### LCD I2C Address Detection
If LCD doesn't work, run I2C scanner code:

```cpp
#include <Wire.h>
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("I2C Scanner");
}
void loop() {
  for(byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);
    if(Wire.endTransmission() == 0) {
      Serial.print("Found I2C Device: 0x");
      Serial.println(i, HEX);
    }
  }
  delay(1000);
}
```

## Applications

### Electronics Testing
- **Signal injection** for amplifier testing
- **Clock signals** for digital circuits  
- **Test patterns** for oscilloscope calibration
- **Function simulation** in prototypes

### Educational Use
- **Waveform demonstration** in electronics courses
- **Signal processing** experiments
- **Control system** input signals
- **Arduino programming** examples

## Contributing

Feel free to contribute improvements:
1. Fork the repository
2. Create feature branch
3. Submit pull request

### Suggested Improvements
- Additional waveform types (noise, burst, etc.)
- Higher frequency ranges
- External control interfaces
- Mobile app integration
- PCB design files

## License

This project is open source. Feel free to modify and distribute.

## Version History

- **v1.0** - Initial release with basic functionality
- **v1.1** - Added phase control and improved encoder response
- **v1.2** - Enhanced display and serial monitoring

## Support

For questions or issues:
- Check troubleshooting section
- Review pin connections
- Verify library installations
- Test with minimal hardware setup

---

**Happy Signal Generating!** 🎵⚡