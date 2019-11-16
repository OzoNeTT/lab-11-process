#pragma once

#include <string>

struct Globals
{
    static std::string logLevel;
    static int timeout;
    static bool isInstallEnabled;
    static bool isPackEnabled;
    static std::string buildConfig;
};

std::string Globals::logLevel{};
int Globals::timeout{};
bool Globals::isInstallEnabled{};
bool Globals::isPackEnabled{};
std::string Globals::buildConfig{};
