#pragma once

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <random>

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include "Globals.h"
struct ThreadData
{
    ThreadData() = delete;

    bool isTerminated = false;
    boost::process::child currentProcess;
};

int programArguments(int argc, char **argv)
{

    boost::program_options::positional_options_description positionalArgs;
    positionalArgs.add("input", -1);

    boost::program_options::options_description visibleOptions("Available options");
    visibleOptions.add_options()
            ("log-level",
             boost::program_options::value<std::string>(&Globals::logLevel)->default_value("info"),
             "info, warning or error")
            ("config",
             boost::program_options::value<std::string>(&Globals::buildConfig)->default_value("Debug"),
             "DCMAKE_BUILD_TYPE")
            ("install",
             "Add install step")
            ("pack",
             "Add pack step")
            ("timeout",
             boost::program_options::value<int>(&Globals::timeout)->default_value(-1),
             "Process timeout (ms)")
            ("help", "Prints help message");

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, visibleOptions), variablesMap);
    boost::program_options::notify(variablesMap);

    if (variablesMap.count("help")) {
        std::cout << visibleOptions << "\n";
        return 1;
    }
    if (variablesMap.count("install")) {
        Globals::isInstallEnabled = true;
    }
    if (variablesMap.count("pack")) {
        Globals::isPackEnabled = true;
    }

    return 0;
}