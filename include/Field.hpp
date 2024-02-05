/*
    The Field definition and implementation of the Phosphorus Engine
    Copyright (C) 2024  Andy Shen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef FIELD_H
#define FIELD_H
#include "Vector.hpp"
#include <utility>

namespace Phosphorus {
class _single_Field : public Vector {
private:
public:
    /*
        (x1,y1)
                    (x2,y2)
        */
    double _x1, _y1, _x2, _y2;
    _single_Field() = default;
    _single_Field(double intensity, double degree)
        : Vector()
    {
        _x1 = -1000000;
        _y1 = -1000000;
        _x2 = 1000000;
        _y2 = 1000000;
        _data = intensity;
        _deg = degree;
    }
    _single_Field(double x1, double y1, double x2, double y2, double intensity, double degree)
        : Vector()
    {
        _x1 = x1;
        _y1 = y1;
        _x2 = x2;
        _y2 = y2;
        _data = intensity;
        _deg = degree;
    }
};

typedef std::pair<_single_Field, _single_Field> Field; // Gravity, Electricity

class MagField {
public:
    bool _dir = false; // False is in, True is out.
    double _b {};
    double x1 {}, y1 {}, x2 {}, y2 {};

    MagField() = default;
    MagField(bool direct, double B)
    {
        _dir = direct;
        _b = B;
        x1 = -1000000;
        y1 = -1000000;
        x2 = 1000000;
        y2 = 1000000;
    }
};
}

#endif