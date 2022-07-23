class Weather
{
    float temp = 0;
    float feels_like = 0;
    float temp_min = 0;
    float temp_max = 0;

public:
    void update();
    float getTemp() const { return temp; }
    float getFeelsLike() const { return feels_like; }
};