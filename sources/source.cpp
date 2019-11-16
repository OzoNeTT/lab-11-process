#include <async++.h>
#include <memory>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/log/trivial.hpp>
#include "Globals.h"
#include "Parser.h"
#include "Builder.h"
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/trivial.hpp>
#include <boost/unordered_map.hpp>
#include <thread>

void create(std::unique_ptr<ThreadData> &data)
{
    using namespace std::chrono;

    if (Globals::timeout <= 0) {
        return;
    }

    while (Globals::timeout > 0 && (!data || !data->isTerminated)) {
        static const int DATA_TIMEOUT = 50;

        std::this_thread::sleep_for(milliseconds{DATA_TIMEOUT});
        Globals::timeout -= DATA_TIMEOUT;
    }

    data->isTerminated = true;

    try {
        data->currentProcess.terminate();
    } catch (std::exception &exception) {
        BOOST_LOG_TRIVIAL(error) << exception.what();
    }

    BOOST_LOG_TRIVIAL(info) << "Timeout";
}

void init()
{
    boost::log::register_simple_formatter_factory<
            boost::log::trivial::severity_level,
            char
    >("Severity");
    static const std::string format = "[%TimeStamp%][%ThreadID%][%Severity%]: %Message%";

    auto sinkFile = boost::log::add_file_log(
            boost::log::keywords::file_name = ".build/logs/log_%N.log",
            boost::log::keywords::rotation_size = 128 * 1024 * 1024,
            boost::log::keywords::auto_flush = true,
            boost::log::keywords::format = format
    );
    sinkFile->set_filter(
            boost::log::trivial::severity >= boost::log::trivial::trace
    );          // Log file setup

    // From config to boost enum
    static const boost::unordered_map<std::string, boost::log::trivial::severity_level> CONSOLE_FILTER = {
            {"debug", boost::log::trivial::debug},
            {"info", boost::log::trivial::info},
            {"warning", boost::log::trivial::warning},
            {"error", boost::log::trivial::error},
    };

    auto sinkConsole = boost::log::add_console_log(
            std::cout,
            boost::log::keywords::format = format
    );
    sinkConsole->set_filter(
            boost::log::trivial::severity >= CONSOLE_FILTER.at(Globals::logLevel)
    );      // Log console setup

    boost::log::add_common_attributes();
}

void startProcess(const std::list<std::string> &commandArgs, std::unique_ptr<ThreadData> &data)
{
    using namespace boost::process;

    if (data && data->isTerminated) {
        return;
    }

    // Start
    BOOST_LOG_TRIVIAL(info) << "Command: cmake";
    for (const auto &arg : commandArgs) {
        BOOST_LOG_TRIVIAL(info) << "                " << arg;
    }
    BOOST_LOG_TRIVIAL(info) << "\n";

    ipstream stream;

    child child{search_path("cmake"),
                args(commandArgs),
                std_out > stream};

    // Update data
    if (!data) {
        data = std::make_unique<ThreadData>(ThreadData{false, std::move(child)});
    } else {
        data->currentProcess = std::move(child);
    }
    BOOST_LOG_TRIVIAL(debug) << "Data updated";

    // Get output
    for (std::string line; data->currentProcess.running() && std::getline(stream, line);) {
        BOOST_LOG_TRIVIAL(info) << line;
    }

    // Wait until exit
    data->currentProcess.wait();

    auto exitCode = data->currentProcess.exit_code();
    BOOST_LOG_TRIVIAL(info) << "Exit code: " << exitCode;

    if (exitCode != 0) {
        BOOST_LOG_TRIVIAL(error) << "Non zero exit code. Exiting...";
        data->isTerminated = true;
    }
}

int main(int argc, char *argv[])
{
    if (auto returnCode = programArguments(argc, argv); returnCode != 0) {
        return returnCode;
    }

    init();

    std::unique_ptr<ThreadData> data{nullptr};

    auto timer = async::spawn(boost::bind(&create, std::ref(data)));

    auto build = async::spawn(
        boost::bind(startProcess, Command::getConfig(), std::ref(data))
    ).then(
        boost::bind(startProcess, Command::getBuild(), std::ref(data))
    );

    if (Globals::isInstallEnabled) {
        build = build.then(
            boost::bind(startProcess, Command::getInstall(), std::ref(data))
        );
    }
    if (Globals::isPackEnabled) {
        build = build.then(
            boost::bind(startProcess, Command::getPack(), std::ref(data))
        );
    }
    build = build.then([&data]() {
        data->isTerminated = true;
    });

    build.wait();
    timer.wait();
}