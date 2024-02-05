#include <Vector.hpp>
#include <bits/stdc++.h>

using namespace std;

Vector a(3, 0), b(4, -90);
Vector c(1.732, -30), d(1, -120), z(0, 0);

int main()
{
    cout << sin(-1.6667) << endl;
    cout << (a + b).returnData() << ' ' << (a + b).returnDegree() << endl;
    cout << (c + d).returnData() << ' ' << (c + d).returnDegree() << endl;
    cout << (c + d + z).returnData() << ' ' << (c + d + z).returnDegree() << endl;
}