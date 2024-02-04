/*
    The Dynamics definition and implementation of the Phosphorus Engine
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

#ifndef DYNAMICS_H
#define DYNAMICS_H
#include "Field.hpp"
#include "Vector.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace Phosphorus {

class Velocity : public Vector {
private:
public:
    Velocity() = default;
    explicit Velocity(const Vector& rhs)
        : Vector()
    {
        _data = rhs.returnData();
        _deg = rhs.returnDegree();
    }
    Velocity(double speed, double degree)
        : Vector()
    {
        _data = speed;
        _deg = degree;
    }
};

class Movement : public Vector {
private:
public:
    Movement() = default;
    Movement(double length, double degree)
        : Vector()
    {
        _data = length;
        _deg = degree;
    }
    Movement(Velocity v1, Velocity v2, double divT)
        : Vector()
    {
        double Xx = (v1.returnVx().returnData() + v2.returnVx().returnData()) / 2 * divT;
        double Xy = (v1.returnVy().returnData() + v2.returnVy().returnData()) / 2 * divT;
        _data = sqrt(Xx * Xx + Xy * Xy);
        _deg = atan(Xy / Xx) / Pi * 180;
    }
};

class Force : public Vector {
private:
public:
    Force() = default;
    Force(double intensity, double degree)
        : Vector()
    {
        _data = intensity;
        _deg = degree;
    }
    explicit Force(const Vector& rhs)
        : Vector()
    {
        _data = rhs.returnData();
        _deg = rhs.returnDegree();
    }
    [[nodiscard]] inline Velocity toAddition(double mass, double divT) const
    {
        return { _data / mass * divT, _deg };
    }
    static inline Force fromField(const _single_Field& rhs, double d)
    {
        return { rhs.returnData() * d, rhs.returnDegree() };
    }
    [[nodiscard]] static inline Force fromMagField(const MagField& rhs, const Velocity& v, double q)
    {
        return {
            rhs._b * v.returnData() * q,
            rhs._dir ? (v.returnDegree() - 90) : (v.returnDegree() + 90)
        };
    }
};

class FieldManager {
private:
    FieldManager() {};
    ~FieldManager() = default;
    std::vector<Field> fields;
    std::vector<MagField> magFields;

public:
    FieldManager(const FieldManager&) = delete;
    FieldManager& operator=(const FieldManager&) = delete;

public:
    static FieldManager& getInstance()
    {
        static FieldManager instance;
        return instance;
    }
    void addField(const Field& rhs)
    {
        fields.emplace_back(rhs);
    }
    void addMagField(const MagField& rhs)
    {
        magFields.emplace_back(rhs);
    }
    Force calcForce(double x, double y, double m, double q, const Velocity& v)
    {
        Force ans;
        for (const auto& it : fields) {
            if ((x >= it.first._x1) && (y >= it.first._y1)
                && (x <= it.first._x2) && (y <= it.first._y2)) {
                ans = Force(ans + Force::fromField(it.first, m));
            }
            if ((x >= it.second._x1) && (y >= it.second._y1)
                && (x <= it.second._x2) && (y <= it.second._y2)) {
                ans = Force(ans + Force::fromField(it.first, q));
            }
        }
        for (auto it : magFields) {
            if ((x >= it.x1) && (y >= it.y1)
                && (x <= it.x2) && (y <= it.y2)) {
                ans = Force(ans + Force::fromMagField(it, v, q));
            }
        }
        return ans;
    }
};

class Object {
private:
    double _m {}, _q {};
    std::pair<double, double> _pos;
    Velocity _v {};

public:
    Object() = default;
    Object(double mass, double quantity, double x, double y, const Velocity& v)
    {
        _m = mass;
        _q = quantity;
        _pos = { x, y };
        _v = v;
    }
    void move(double divT) // divT/s
    {
        auto v0 = _v;
        auto _f = FieldManager::getInstance().calcForce(_pos.first, _pos.second, _m, _q, _v);
        auto v1 = Velocity(_v + _f.toAddition(_m, divT));
        Movement x(v0, v1, divT);
        _pos.first += x.returnVx().returnData();
        _pos.second += x.returnVy().returnData();
        _v = v1;
    }
    [[nodiscard]] auto returnPos() const
    {
        return _pos;
    }
};
}
#endif