#include <Dynamics.hpp>
#include <bits/stdc++.h>

using namespace std;
using namespace Phosphorus;

int main()
{
    freopen("result1.csv", "w", stdout);
    cout << "x,y" << endl;
    Object a(1, 0.1, 0, 0, { 1000, 0 });
    // FieldManager::getInstance().addField({ _single_Field(10, 270), _single_Field(0, 0) });
    FieldManager::getInstance().addMagField({ false, 1000 });
    for (int i = 1; i <= 1000; i++) {
        a.move(0.0001);
        cout << a.returnPos().first << ',' << a.returnPos().second << endl;
    }
}