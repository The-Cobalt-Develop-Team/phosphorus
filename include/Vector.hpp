/*
    The Vector definition and implementation of the Phosphorus Engine
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

#ifndef VECTOR_H
#define VECTOR_H
#define DEBUG 1

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

constexpr double Pi = 3.1415926535897932384626;
#if DEBUG
void debug(const std::string& src, const double& msg)
{
    std::cerr << src << ":" << msg << std::endl;
}
#endif
class Vector {
protected:
    double _data, _deg;

public:
    Vector() = default;
    inline void fixing()
    {
        if (_data < 0) {
            _deg = 180 + _deg;
            _data = -_data;
        }
        while (_deg > 180) {
            _deg -= 360;
        }
        while (_deg < -180) {
            _deg += 360;
        }
    }
    Vector(double data, double degree) noexcept
    {
        _data = data;
        _deg = degree;
        fixing();
    }
    [[nodiscard]] inline double returnData() const
    {
        return _data;
    }
    [[nodiscard]] inline double returnDegree() const
    {
        return _deg;
    }
    [[nodiscard]] inline Vector returnVx() const // Maybe need to be optimized
    {
        Vector tmp {};
        tmp._data = _data * std::cos(_deg / 180 * Pi);
        tmp._deg = 0;
        return tmp;
    }
    [[nodiscard]] inline Vector returnVy() const // Maybe need to be optimized
    {
        Vector tmp {};
        tmp._data = _data * std::sin(_deg / 180 * Pi);
        tmp._deg = 90;
        return tmp;
    }
    [[nodiscard]] static Vector mix_vertexes(const Vector& lhs, const Vector& rhs)
    {
        auto Vx = lhs.returnVx()._data + rhs.returnVx()._data;
        auto Vy = lhs.returnVy()._data + rhs.returnVy()._data;
        return { std::sqrt(std::pow(Vx, 2) + std::pow(Vy, 2)), std::atan(Vy / Vx) / Pi * 180 };
    }
    Vector operator+(const Vector& rhs) const
    {
        auto tmp = mix_vertexes(*this, rhs);
        tmp.fixing();
        return tmp;
    }
    Vector operator-() const
    {
        return { _data, 360 - _deg };
    }
    Vector operator-(const Vector& rhs) const
    {
        auto tmp = mix_vertexes(*this, -rhs);
        tmp.fixing();
        return tmp;
    }
    Vector operator*(const double& rhs) const
    {
        return { _data * rhs, _deg };
    }
    Vector operator/(const double& rhs) const
    {
        return { _data / rhs, _deg };
    }
};
#endif