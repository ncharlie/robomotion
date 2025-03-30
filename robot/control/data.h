#ifndef _USER_DATA_H_
#define _USER_DATA_H_
#include <HardwareSerial.h>
#include <avr/io.h>

#define BAUD 9600

#include <util/setbaud.h>

class Data {
   private:
    uint8_t value = 0;
    uint8_t lastReading = 0;            // previous reading of input pins
    unsigned long lastReadingTime = 0;  // last time the input pin was triggered
   public:
    Data() {
        /* Set baud rate */
        UBRR3H = UBRRH_VALUE;
        UBRR3L = UBRRL_VALUE;
        /* Enable receiver and transmitter */
        UCSR3B = _BV(RXEN3) | _BV(TXEN3);
        /* Set frame format: 8data, 2stop bit */
        UCSR3C = _BV(USBS3) | _BV(UCSZ31) | _BV(UCSZ30);

        // Serial3.begin(9600);
    }

    void USART_Transmit(unsigned char data) {
        /* Wait for empty transmit buffer */
        while (!(UCSR3A & _BV(UDRE3)));
        /* Put data into buffer, sends the data */
        UDR3 = data;
    }

    void USART_Flush(void) {
        unsigned char dummy;
        while (UCSR3A & _BV(RXC3)) dummy = UDR3;
    }

    unsigned char USART_Receive(void) {
        /* Wait for data to be received */
        while (!(UCSR3A & _BV(RXC3)));
        /* Get and return received data from buffer */
        return UDR3;
    }
};

Data data;

#endif /* _USER_DATA_H_ */