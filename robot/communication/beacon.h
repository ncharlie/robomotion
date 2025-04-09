#ifndef _BEACON_H_
#define _BEACON_H_

class Beacon {
   public:
    int x;
    int y;
    int z;

   public:
    Beacon(int a, int b, int c) {
        x = a;
        y = b;
        z = c;
    }
};

#endif /* _BEACON_H_ */