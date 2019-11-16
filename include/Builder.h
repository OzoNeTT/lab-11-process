#pragma once

#include <list>
#include <string>
#include "Globals.h"


class Command
{
public:
    using ArgList = std::list<std::string>;

    static const std::string DIRECTORY_BUILD;
    static const std::string DIRECTORY_INSTALL;

    static ArgList getConfig();

    static ArgList getBuild();

    static ArgList getInstall();

    static ArgList getPack();
};

const std::string Command::DIRECTORY_BUILD = ".build";

const std::string Command::DIRECTORY_INSTALL = ".install";

Command::ArgList Command::getConfig()
{
    return {"-H.",
            "-B" + DIRECTORY_BUILD,
            +"-DCMAKE_INSTALL_PREFIX=" + DIRECTORY_INSTALL,
            +"-DCMAKE_BUILD_TYPE=" + Globals::buildConfig};
}

Command::ArgList Command::getBuild()
{
    return {"--build", DIRECTORY_BUILD};
}

Command::ArgList Command::getInstall()
{
    return {"--build", DIRECTORY_BUILD,
            "--target", "install"};
}

Command::ArgList Command::getPack()
{
    return {"--build", DIRECTORY_BUILD,
            "--target", "package"};
}
