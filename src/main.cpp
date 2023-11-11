/**
 * @file main.cpp
 * @brief This file puts together all the other files.
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "commands.h"
#include "directory.h"
#include "logger.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

// * Finding OS
#ifdef _WIN32
    #define OS_NAME "windows"
#elif __APPLE__
    #define OS_NAME "macOS"
#elif __linux__
    #define OS_NAME "linux"
#else
    #define OS_NAME "null"
#endif

// * All commands
std::vector<std::string> all_commands = {
    "init",
    "pair",
    "help",
    "version",
    "contents",
    "insert"
};

/**
 * @brief Processes command.
 *
 * @param command
 * @param arguments
 * @param flags
 * @param language
 * @return int
 */
int process_command(std::string command, std::vector<std::string> arguments, std::vector<std::string> flags, std::string language)
{
    // * Return status of command
    int r_code = 1;

    if (command == "init")
    {
        if (language != "c" && language != "cpp")
        {
            logger::error_q("is not a supported programming language", language);
            return r_code;
        }

        if (std::find(flags.begin(), flags.end(), "-post") != flags.end())
        {
            r_code = commands::post_init(language);
            return r_code;
        }

        r_code = commands::init(language);
    }
    else if (command == "pair")
        r_code = commands::file_pair(arguments, (std::find(flags.begin(), flags.end(), "-hpp") != flags.end()) ? true : false, language);
    else if (command == "help")
        r_code = commands::help();
    else if (command == "version")
        r_code = commands::version();
    else if (command == "contents")
        r_code = commands::contents(arguments, flags);
    else if (command == "insert")
        r_code = commands::insert(arguments, flags, language);

    return r_code;
}

/**
 * @brief Main function.
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char *argv[])
{
    // * Assertations
    if (argc <= 1)
    {
        logger::error("no command provided");
        logger::flush_buffer();

        return 1;
    }

    // * Parsing command
    std::string command = argv[1];
    std::vector<std::string> arguments;
    std::vector<std::string> flags;

    for (int i = 0; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg.substr(0, 1) == "-" && arg.size() >= 2)
            flags.push_back(arg);
    }

    for (int i = 2; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg.substr(0, 1) != "-")
            arguments.push_back(arg);
    }

    logger::success("parsed command");

    logger::custom("command \'" + command + "\' with " + std::to_string(arguments.size()) + " arguments and " + std::to_string(flags.size()) + " flags", "received", "blue");

    // * Changing '--version' to 'version' for simplicity
    if (command == "--version")
        command = "version";

    // * Validating command
    if (!(std::find(all_commands.begin(), all_commands.end(), command) != all_commands.end()))
    {
        logger::error_q("is not a valid command", command);
        return 1;
    }

    // * Checking initialized status
    bool initialized = commands::verify_init();

    // * All commands that don't need the directory to be initialized.
    std::vector<std::string> init_exceptions = {
        "init",
        "help",
        "version",
        "contents",
    };

    bool cmd_is_exception = false;

    if (std::find(init_exceptions.begin(), init_exceptions.end(), command) != init_exceptions.end())
    {
        initialized = true;
        cmd_is_exception = true;
    }

    // * Assertions
    if (!initialized)
    {
        logger::error("directory does not have a \'.cpm\' file. If you already have a project setup, you can use \'cpm init <language> -post\' to post-initalize cpm. If you don't have a project, you can use \'cpm init <language>\' to construct one.");
        return 1;
    }

    std::string language;

    if (!cmd_is_exception)
    {
        std::string contents_cpm = directory::slurp("./", ".cpm");
        language = (contents_cpm.find("language: c")) ? "c" : "cpp";
    }
    else if (arguments.size() >= 1)
        language = (arguments[1] == "c") ? "c" : "cpp";

    if (language == "c++")
        language = "cpp";

    // * Running command
    int result = process_command(command, arguments, flags, language);

    logger::custom("command \'" + command + "\' with exit code " + std::to_string(result), "finished", "blue");
    logger::flush_buffer();

    return 0;
}