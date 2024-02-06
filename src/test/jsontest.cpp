#include "JSONResolver/json.hpp"
#include <bits/stdc++.h>

using namespace std;
using json = nlohmann::json;

// ...

std::ifstream f("test2.json");
json d = json::parse(f);

int main()
{
}