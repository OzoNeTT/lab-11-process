#pragma once

#include <list>
#include <string>
#include "Globals.h"


class Command
{
public:

    static const std::string DIRECTORY_BUILD;
    static const std::string DIRECTORY_INSTALL;
    static std::list<std::string> getConfig();
    static std::list<std::string> getBuild();
    static std::list<std::string> getInstall();
    static std::list<std::string> getPack();
};

const std::string Command::DIRECTORY_BUILD = ".build";

const std::string Command::DIRECTORY_INSTALL = ".install";

std::list<std::string> Command::getConfig()
{
    return {"-H.",
            "-B" + DIRECTORY_BUILD,
            +"-DCMAKE_INSTALL_PREFIX=" + DIRECTORY_INSTALL,
            +"-DCMAKE_BUILD_TYPE=" + Globals::buildConfig};
}

std::list<std::string> Command::getBuild()
{
    return {"--build", DIRECTORY_BUILD};
}

std::list<std::string> Command::getInstall()
{
    return {"--build", DIRECTORY_BUILD,
            "--target", "install"};
}

std::list<std::string> Command::getPack()
{
    return {"--build", DIRECTORY_BUILD,
            "--target", "package"};
}
