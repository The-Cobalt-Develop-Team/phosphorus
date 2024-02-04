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

#include <algorithm>
#include <cmath>

constexpr double Pi = 3.1415926535897932384626;

class Vector {
protected:
    double _data, _deg;

public:
    Vector() = default;
    Vector(double data, double degree)
    {
        _data = data;
        _deg = degree;
    }
    void fixing()
    {
        if (_data < 0) {
            _deg = 360. - _deg;
            _data = -_data;
        }
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
        Vector tmp = { _data * std::cos(_deg / 180 * Pi), 0 };
        tmp.fixing();
        return tmp;
    }
    [[nodiscard]] inline Vector returnVy() const // Maybe need to be optimized
    {
        Vector tmp = { _data * std::sin(_deg / 180 * Pi), 90 };
        tmp.fixing();
        return tmp;
    }
    [[nodiscard]] static Vector mix_vertexes(const Vector& lhs, const Vector& rhs)
    {
        return {
            std::sqrt(
                std::pow(lhs.returnVx()._data + rhs.returnVx()._data, 2)
                + std::pow(lhs.returnVy()._data + rhs.returnVy()._data, 2)),
            std::atan((lhs.returnVy()._data + rhs.returnVy()._data) / lhs.returnVx()._data + rhs.returnVx()._data) / Pi * 180
        };
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
    Vector& operator=(const Vector& rhs)
        = default;
};
#endif