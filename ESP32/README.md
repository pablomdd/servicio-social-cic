# ESP32 

## ESP32 drivers

- Driver: CP210x USB to UART Bridge Virtual COM Port https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

## ESP32 Development

ESP32 development is done with the Arduino Framework. Coding, compiling and flashing into te board can be done it 3 main diferent ways: Expressif VSCode extension, PlatformIO (also VSCode extension) or the Arduino IDE itself (easiest and recommended).

Follow instalation instructions for ESP32 in Arduino IDE or PlatformIO (VS Code).

https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html


# NodeJS

Require Node JS v. 16 or above.

## Usage

```
cd js-websocket-server
npm install
npm run start
```

# ESP32 Pin config
Reference https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
## Interrupts
All pins can be used as interrupts.

https://randomnerdtutorials.com/esp32-pir-motion-sensor-interrupts-timers/

To set an interrupt in the Arduino IDE, you use the attachInterrupt() function, that accepts as arguments: the GPIO pin, the name of the function to be executed, and mode:

```cpp
attachInterrupt(digitalPinToInterrupt(GPIO), function, mode);
```

The first argument is a GPIO number. Normally, you should use `digitalPinToInterrupt(GPIO)` to set the actual GPIO as an interrupt pin. For example, if you want to use GPIO 27 as an interrupt, use:

```cpp
digitalPinToInterrupt(27)
```

The second argument of the `attachInterrupt()` function is the name of the function that will be called every time the interrupt is triggered.

The third argument is the mode. There are 5 different modes:

- `LOW`: to trigger the interrupt whenever the pin is `LOW`;
- `HIGH`: to trigger the interrupt whenever the pin is `HIGH`;
- `CHANGE`: to trigger the interrupt whenever the pin changes value – for example from `HIGH` to `LOW` or `LOW` to `HIGH`;
- `FALLING`: for when the pin goes from HIGH to `LOW`;
- `RISING`: to trigger when the pin goes from `LOW` to `HIGH`.


## PWM
All pins except GPIOs 34 to 39 can generate PWM. It may be required to declared frequency and resolution.
https://randomnerdtutorials.com/esp32-pwm-arduino-ide/

Example: Multiple LED PWM

```cpp
// the number of the LED pin
const int ledPin = 16;  // 16 corresponds to GPIO16
const int ledPin2 = 17; // 17 corresponds to GPIO17
const int ledPin3 = 5;  // 5 corresponds to GPIO5

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
 
void setup(){
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
  ledcAttachPin(ledPin2, ledChannel);
  ledcAttachPin(ledPin3, ledChannel);
}
 
void loop(){
  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }

  // decrease the LED brightness
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);   
    delay(15);
  }
}
```
