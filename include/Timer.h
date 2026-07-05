#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
    Timer() : time(0.0f) {}

    void Update(float dt)
    {
        time += dt;
    }

    void Restart()
    {
        time = 0.0f;
    }

    float Get() const
    {
        return time;
    }

private:
    float time;
};

#endif