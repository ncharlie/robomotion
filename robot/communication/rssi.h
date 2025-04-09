#ifndef _RSSI_H_
#define _RSSI_H_

class Rssi {
   public:
    String name;
    int rssi;

   public:
    Rssi(String beaconName, int signalStrength) {
        name = beaconName;
        rssi = signalStrength;
    }
};

#endif /* _RSSI_H_ */