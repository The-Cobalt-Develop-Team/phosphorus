#include "Dynamics.hpp"
#include "JSONResolver/json.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using json = nlohmann::json;
json config;
using namespace Phosphorus;

void help()
{
    std::cout << "Using: phosphorus [Args] [Files] ...\n\
A physics engine for analyze dynamics\n\
\n\
Files:\n\
    -F=FILE   specify input filename\n\
    -O=FILE   specify output filename\
    " << std::endl;
}

void execute()
{
    std::cout << "x,y" << std::endl;
    Object a(config["object"]["m"],
        config["object"]["q"],
        config["object"]["posX"],
        config["object"]["posY"],
        { config["object"]["v0"], config["object"]["v0Deg"] });
    for (auto it : config["fields"]) {
        if (it["x1"].is_null() || it["y1"].is_null() || it["x2"].is_null() || it["y2"].is_null()) {
            FieldManager::getInstance().addField({ _single_Field(it["g"], it["gDeg"]), _single_Field(it["E"], it["EDeg"]) });
            std::cerr << "Unspecified Field pos, use default argument." << std::endl;
        } else {
            FieldManager::getInstance().addField({ _single_Field(it["x1"], it["y1"], it["x2"], it["y2"], it["g"], it["gDeg"]), _single_Field(it["x1"], it["y1"], it["x2"], it["y2"], it["E"], it["EDeg"]) });
        }
    }
    for (auto it : config["magFields"]) {
        if (it["x1"].is_null() || it["y1"].is_null() || it["x2"].is_null() || it["y2"].is_null()) {
            if (it["dir"] == "in") {
                FieldManager::getInstance().addMagField({ false, it["B"] });
            } else if (it["dir"] == "out") {
                FieldManager::getInstance().addMagField({ true, it["B"] });
            }
            std::cerr << "Unspecified magField pos, use default argument." << std::endl;
        } else {
            if (it["dir"] == "in") {
                FieldManager::getInstance().addMagField({ it["x1"], it["y1"], it["x2"], it["y2"], false, it["B"] });
            } else if (it["dir"] == "out") {
                FieldManager::getInstance().addMagField({ it["x1"], it["y1"], it["x2"], it["y2"], true, it["B"] });
            }
        }
    }
    std::cout << a.returnPos().first << ',' << a.returnPos().second << std::endl;
    for (int i = 1; i <= config["simulation"]["count"]; i++) {
        a.move(config["simulation"]["step"]);
        if (config["simulation"]["sStep"] == 1 || (i % int(config["simulation"]["sStep"]) == 1)) {
            std::cout << a.returnPos().first << ',' << a.returnPos().second << std::endl;
        }
    }
}

int main(int argc, char** argv)
{
    if (argc == 3 && strcmp(argv[1], "-F") == 0) {
        std::ifstream file(argv[2]);
        freopen("result.csv", "w", stdout);
        config = json::parse(file);
        execute();
    } else if (argc == 5 && strcmp(argv[1], "-F") == 0 && strcmp(argv[3], "-O") == 0) {
        std::ifstream file(argv[2]);
        config = json::parse(file);
        freopen(argv[4], "w", stdout);
        execute();
    } else if (argc == 5 && strcmp(argv[1], "-O") == 0 && strcmp(argv[3], "-F") == 0) {
        std::ifstream file(argv[4]);
        config = json::parse(file);
        freopen(argv[2], "w", stdout);
        execute();
    } else {
        help();
    }
}