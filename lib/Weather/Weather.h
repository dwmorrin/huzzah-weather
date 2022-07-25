#pragma once

class Weather
{
    float temp = 0;
    float feels_like = 0;
    float temp_min = 0;
    float temp_max = 0;

public:
    // returns true if update OK
    bool update();
    float getTemp() const { return temp; }
    float getFeelsLike() const { return feels_like; }
    float getTempMin() const { return temp_min; }
    float getTempMax() const { return temp_max; }
};