#include <carlsim.h>

#include <cstdlib>


class MyFullConnection : public ConnectionGenerator {
    public:
    MyFullConnection() {}
    ~MyFullConnection() {}

    void connect(CARLsim* sim, int srcGrp, int i, int destGrp, int j, float& weight, float& maxWt,
            float& delay, bool& connected) {
        connected = 1;
        weight = 0.3f * drand48();
        maxWt = 1.0f;
        delay = 1;
    }
};

class MyOneToOneConnection : public ConnectionGenerator {
    public:
    MyOneToOneConnection() {}
    ~MyOneToOneConnection() {}

    void connect(CARLsim* sim, int srcGrp, int i, int destGrp, int j, float& weight, float& maxWt,
            float& delay, bool& connected) {
        connected = (i==j);
        weight = 10.4f * drand48();
        maxWt = weight;
        delay = 1;
    }
};