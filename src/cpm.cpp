/**
 * @file main.cpp
 * @brief Puts together all the other files.
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "commands.h"
#include "directory.h"
#include "logger.h"
#include "misc.h"
#include "data.h"
#include <chrono>
#include <map>
#include <string>
#include <vector>

std::map<std::string, std::map<std::string, int>> command_info = {
    {
        "create",
        {
            {"init_exception", true},
            {"min_args", 1},
        },
    },
    {
        "help",
        {
            {"init_exception", true},
            {"min_args", 0},
        },
    },
    {
        "version",
        {
            {"init_exception", true},
            {"min_args", 0},
        },
    },
    {
        "test",
        {
            {"init_exception", false},
            {"min_args", 0},
        },
    },
    {
        "fpair",
        {
            {"init_exception", false},
            {"min_args", 2},
        },
    },
};
/**
 * @brief Main function.
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char *argv[])
{
  // Start time
  auto start = std::chrono::high_resolution_clock::now();

  // Singleton accessing
  Logger &logger = Logger::get();
  Data_Handler &data_handler = Data_Handler::get();

  logger.set_colors({
      {"success", logger.raw_colors["green"]},
      {"error", logger.raw_colors["red"]},
      {"warn", logger.raw_colors["orange"]},
      {"count", logger.raw_colors["cyan"]},
      {"prompt", logger.raw_colors["yellow"]},
      {"execute", logger.raw_colors["orange"]},
      {"reset", logger.raw_colors["reset"]},
  });

  data_handler.read();

  // Verifies command is inputted
  if (argc <= 1)
  {
    logger.error("no command provided");
    logger.flush_buffer();

    return 1;
  }

  // Initial validations
  std::string command = argv[1];
  bool command_found = false;

  // Checks if command is valid
  for (const auto &[k, v] : command_info)
  {
    if (k == command)
    {
      command_found = true;
      break;
    }
  }

  // Checks if command exists
  if (!command_found)
  {
    logger.error_q("is not a valid command", command);
    return 1;
  }

  // Checks if command requires cpm.data to exist
  if (!command_info[command]["init_exception"])
  {
    if (!commands::verify_init())
    {
      logger.error_q("requires a valid cpm.data file", command);
      return 1;
    }

    if (!data_handler.has_key("language"))
    {
      logger.error("language information lacking from cpm.store");
      return 1;
    }
  }

  // Parsing
  std::vector<std::string> arguments;
  std::vector<std::string> flags;

  // Flags
  for (int i = 0; i < argc; i++)
  {
    std::string arg = argv[i];
    if ("-" == arg.substr(0, 1) && arg.size() >= 2)
      flags.push_back(arg.substr(1, arg.size()));
  }

  // Arguments
  for (int i = 2; i < argc; i++)
  {
    std::string arg = argv[i];
    if (arg.substr(0, 1) != "-")
      arguments.push_back(arg);
  }

  logger.success("parsed command");

  // Minimum arguments
  if (arguments.size() < command_info[command]["min_args"])
  {
    logger.error_q("requires more than " + std::to_string(arguments.size()) + " arguments", command);
    return 1;
  }

  // Command processing
  int result = 0;

  if (command == "help")
    result = commands::help(arguments);
  else if (command == "version")
    result = commands::version();
  else if (command == "create")
    result = commands::create(arguments, flags);
  else if (command == "test")
    result = commands::test(arguments, flags);
  else if (command == "fpair")
    result = commands::file_pair(arguments, flags);

  // Save data
  if (result == 0 && command != "help" && command != "version")
    data_handler.write();

  // Cleanup artifacts
  directory::destroy_file("cpm.tmp");

  // Measure process time
  auto end = std::chrono::high_resolution_clock::now();

  logger.custom("command \'" + command + "\' with exit code " + std::to_string(result) + " in " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + " ms", "finished", "blue");
  logger.flush_buffer();

  return result;
}