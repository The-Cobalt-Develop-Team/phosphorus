#include <Dynamics.hpp>
#include <bits/stdc++.h>

using namespace std;
using namespace Phosphorus;

int main()
{
    freopen("result1.csv", "w", stdout);
    freopen("result1.log", "w", stderr);
    cout << "x,y" << endl;
    Object a(1, 10, 0, 0, { 1, -90 });
    cout << a.returnPos().first << ',' << a.returnPos().second << endl;
    FieldManager::getInstance().addField({ _single_Field(10, -90), _single_Field(0, 0) });
    FieldManager::getInstance().addMagField({ false, 10 });
    for (int i = 1; i <= 100000; i++) {
        a.move(0.00001);
        cout << a.returnPos().first << ',' << a.returnPos().second << endl;
    }
}