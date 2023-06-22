#include "MQ135.h"

class Lufqualitaet
{
private:
    MQ135 *p_mq135_sensor;
    float *p_temp;
    float *p_humi;

    float ppm;
    float correctedppm;

public:
    bool readSensor();
    float get_ppm();
    float get_correctedppm();

    /* Konstruktor & Destructor */
    Lufqualitaet(int pin, float *temp, float *humi);
    ~Lufqualitaet();
};
