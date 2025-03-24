// #include <avr/io.h>

// #define MAX_SPEED 256
// #define FORWARD B01100110
// #define RIGHT B01011010

// void lightOn() {
//     PORTB |= _BV(PB7);
// }

// void lightOff() {
//     PORTB &= ~_BV(PB7);
// }

// void setDirection(int8_t dir) {
//     PORTC = dir;
// }

// void setSpeed(uint16_t speed) {
//     OCR3B = OCR3C = OCR4B = OCR4C = speed;
// }

// void setup() {
//     SREG &= ~_BV(SREG_I);  // Disable global interrupt

//     // ---------------- INPUT --------------------
//     DDRL &= ~(_BV(DDL7) | _BV(DDL6));  // set pin 42,43 to input
//     PORTL |= _BV(PL7) | _BV(PL6);      // use pull-up resister

//     // ----------------- LED ---------------------
//     DDRB |= _BV(DDB7);

//     // -------------- DIRECTION ------------------
//     // 30 (PC7), 31 (PC6) <--- front-right
//     // 32 (PC5), 33 (PC4) <--- rear-right
//     // 34 (PC3), 35 (PC2) <--- front-left
//     // 36 (PC1), 37 (PC0) <--- rear-left
//     DDRC = B11111111;   // set pins 30-37 to output
//     PORTC = B00000000;  // init direction to 0

//     // ---------------- SPEED --------------------
//     // counter 3 for pin 2,3,5 <--- right
//     // counter 4 for pin 6,7,8 <--- left
//     DDRE |= _BV(DDE5) | _BV(DDE4);  // set speed out pins 2,3 to output
//     DDRH |= _BV(DDH5) | _BV(DDH4);  // set speed out pins 7,8 to output

//     // Clear the Power Reduction Timer/Counter3,4 bit to enable Timer/Counter3,4
//     PRR1 &= ~_BV(PRTIM4) & ~_BV(PRTIM3);

//     // Set Pin Low on compare match (Non-inverting mode) COM3x[1:0] = 1 0
//     // WGM to Fast PWM (Mode 14, p145 Datasheet) WGM3[1:0] = 1 0
//     TCCR3A = _BV(COM3B1) | _BV(COM3C1) | _BV(WGM31);
//     TCCR4A = _BV(COM4B1) | _BV(COM3C1) | _BV(WGM41);

//     // No input ICNC3 = 0, ICES3 = 0
//     // WGM to Fast PWM (Mode 14) WGM3[3:2] = 1 1
//     // No prescaling CS3[2:0] = 0 0 1
//     TCCR3B = _BV(WGM33) | _BV(WGM32) | _BV(CS30);
//     TCCR4B = _BV(WGM43) | _BV(WGM42) | _BV(CS40);

//     ICR3 = ICR4 = MAX_SPEED;  // Set top value for 16-bit PWM
//     OCR3B = OCR3C = 0;        // Reset the compare value(speed)
//     OCR4B = OCR4C = 0;        // Reset the compare value(speed)
//     TCNT3 = TCNT4 = 0;        // Reset Timer

//     // ---------------- START --------------------
//     SREG |= _BV(SREG_I);  // Enable global interrupt
//     Serial.begin(9600);
// }

// int buttonState = B11000000;
// int lastDirection = B11000000;       // previous reading of input pins
// unsigned long lastDebounceTime = 0;  // last time the input pin was triggered
// unsigned long debounceDelay = 100;   // debounce time

// void loop() {
//     // put your main code here, to run repeatedly:
//     int reading = (PINL & _BV(PL7)) | (PINL & _BV(PL6));

//     if (reading != lastDirection) {
//         lastDebounceTime = millis();  // keep timestamp
//     }
//     lastDirection = reading;

//     if (millis() - lastDebounceTime > debounceDelay) {
//         buttonState = reading;  // most likely not a debounce,
//     }

//     // pressed = 0
//     if (buttonState == 64) {  // forward
//         lightOn();
//         setDirection(FORWARD);
//         setSpeed(MAX_SPEED);
//         Serial.println("F");
//     } else if (buttonState == 128) {  // backward
//         lightOn();
//         setDirection(~FORWARD);
//         setSpeed(MAX_SPEED / 2);
//         Serial.println("B");
//     } else if (buttonState == 0) {  // rightward
//         lightOn();
//         setDirection(RIGHT);
//         setSpeed(MAX_SPEED / 4);
//         Serial.println("R");
//     } else if (false) {  // leftward
//         lightOn();
//         setDirection(~RIGHT);
//         setSpeed(MAX_SPEED / 4);
//         Serial.println("L");
//     } else {
//         lightOff();
//         setSpeed(0);
//     }
// }

// ISR(TIMER1_COMPA_vect) {
//     // PORTB = PORTB ^ _BV(PB7);
//     digitalWrite(13, digitalRead(13) ^ 1);
// }