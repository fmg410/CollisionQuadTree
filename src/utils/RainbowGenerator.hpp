#ifndef RAINBOWGENERATOR_HPP
#define RAINBOWGENERATOR_HPP

struct RainbowGenerator{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned int cnt = 0;
    const float freq = .02f;
    RainbowGenerator()
    {
        this->operator++();
    }
    RainbowGenerator& operator++()
    {
        b = std::sin(freq * cnt + 0) * 127 + 128;
        g = std::sin(freq * cnt + 2) * 127 + 128;
        r = std::sin(freq * cnt + 4) * 127 + 128;
        if (cnt++ >= -1U)
        {
                cnt = 0;
        }
        return *this;
    }
};

#endif