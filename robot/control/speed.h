#ifndef _SPEED_CONTROL_H_
#define _SPEED_CONTROL_H_

#include <limits.h>

#define MAX_POWER 1024

#define P(e) e * 0.01         // Kp
#define I(sum) sum * 0.004    // Ki
#define D(diff) diff * 0.002  // Kd

class Speed {
   private:
    unsigned long encoderRPM;
    unsigned long lastUpdate;
    int powerLevel;
    int sumError;
    int prevError;
    int* base;

   public:
    unsigned long countSinceLast;

    Speed(int* baseSpeed) : base(baseSpeed) {
        // 64 * 12 = 768 per 1 round (314 mm)
        // 3. Pin 18 INT3 <--- front-right
        // 2. Pin 19 INT2 <--- rear-right
        // 1. Pin 20 INT1 <--- front-left
        // 0. Pin 21 INT0 <--- rear-left
    }

    void updateSpeed(bool log) {
        unsigned long now = millis();
        unsigned long last = lastUpdate;
        unsigned long count = countSinceLast;

        // clear
        lastUpdate = now;
        countSinceLast = 0;

        unsigned long elapsed = now - last;

        // if (now > last)
        //     elapsed = now - last;
        // else
        //     elapsed = now + (ULONG_MAX - last);
        // if (count < 3)
        //     encoderRPM = 0;
        // else
        //     encoderRPM = 5000 * count / elapsed;

        encoderRPM = 5000 * count / elapsed;

        // if (log) {
        //     char message[100];
        //     sprintf(message, "encoderRPM: %lu = 5000 * (count: %lu) / (elapse: %lu)\n",
        //             encoderRPM, count, elapsed);
        //     Serial.print(message);
        // }
        // 78125
    }

    unsigned long getSpeed() { return encoderRPM; }

    int checkSpeed(int tSpd) {
        if (tSpd == 0) {
            powerLevel = 0;
            return powerLevel;
        }

        int error = tSpd - encoderRPM;
        sumError += error;

        int p = P(error);
        int i = I(sumError);
        int d = D((error - prevError));
        prevError = error;

        // char message[100];
        // sprintf(message, "Target = %d Speed = %lu rpm\tPower = %d\tAdjust = %d\tP=%d I=%d D=%d",
        //     tSpd, encoderRPM, powerLevel, p+i+d, p, i, d);
        // Serial.println(message);

        powerLevel += p + i + d;
        if (powerLevel < 0)
            powerLevel = 0;
        else if (powerLevel > MAX_POWER)
            powerLevel = MAX_POWER;

        if (i) sumError = 0;
        return powerLevel;
    }

    void reset() {
        sumError = 0;
        prevError = 0;
    }
};

#endif /* _SPEED_CONTROL_H_ */