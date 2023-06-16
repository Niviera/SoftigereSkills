#include "Lufqualität.h"

bool Lufqualitaet::readSensor()
{
    ppm = this->p_mq135_sensor->getPPM();
    correctedppm = this->p_mq135_sensor->getCorrectedPPM((*this->p_temp), (*this->p_humi));
    return true;
}

/* Getter */
float Lufqualitaet::get_ppm()
{
    return this->ppm;
}
float Lufqualitaet::get_correctedppm()
{
    return this->correctedppm;
}

/* Konstruktor & Destructor */
Lufqualitaet::Lufqualitaet(int pin, float *temp, float *humi)
{
    MQ135 mq135_sensor = MQ135(pin);
    this->p_mq135_sensor = &mq135_sensor;
    this->p_temp = temp;
    this->p_humi = humi;
}

Lufqualitaet::~Lufqualitaet()
{
}