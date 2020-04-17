#pragma once

struct IntVal {
    int val;

    float Get01() const;

    static const int Max;

    IntVal();
    IntVal(const int &);
    IntVal(const float &);  // Converts from 0-1 range
    static IntVal Convert01(float v);
};

inline const int IntVal::Max = 65535;

struct ColorRGB {
    float r, g, b;
};

struct IntColorRGB {
    IntVal r, g, b;

    ColorRGB
    GetColor()
    {
        return {r.Get01(), g.Get01(), b.Get01()};
    }
};

struct Location {
    int x, y;
};

inline IntVal::IntVal()
    : val(0){};

inline IntVal::IntVal(const int &v)
{
    this->val = v;
};

inline IntVal::IntVal(const float &v)
{
    *this = IntVal::Convert01(v);
};

inline IntVal
IntVal::Convert01(float v)
{
    return (int)((double)v * (double)IntVal::Max);
};

inline float
IntVal::Get01() const
{
    return (double)val / (double)IntVal::Max;
};
