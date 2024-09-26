/**
 * @file commands.cpp
 * @brief Contains the functionality of all commands.
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

#include <fstream>
#include <cstdlib>
#include <iostream>

namespace commands
{
  // Logger
  Logger &logger = Logger::get();
  Data_Handler &data_handler = Data_Handler::get();

  /**
   * @brief All templates supported by the create command.
   *
   */
  std::vector<std::string> supported_structures = {
      "default",
      "standard",
      "simple",
  };

  /**
   * @brief Runs test code for CPM.
   *
   * @param args
   * @param flags
   * @return int
   */
  int test(const std::vector<std::string> &args, const std::vector<std::string> &flags)
  {
    logger.execute("ls -la");

    return 0;
  }

  /**
   * @brief Lists all commands and provides useful information about CPM.
   *
   * @return int
   */
  int help(const std::vector<std::string> &args)
  {
    // CPM ASCII art
    std::cout << "\n"
              << logger.colors["blue"]
              << "      ___           ___         ___     \n"
              << "     /  /\\         /  /\\       /__/\\    \n"
              << "    /  /:/        /  /::\\     |  |::\\   \n"
              << "   /  /:/        /  /:/\\:\\    |  |:|:\\  \n"
              << "  /  /:/  ___   /  /:/~/:/  __|__|:|\\:\\ \n"
              << " /__/:/  /  /\\ /__/:/ /:/  /__/::::| \\:\\\n"
              << " \\  \\:\\ /  /:/ \\  \\:\\/:/   \\  \\:\\~~\\__\\/\n"
              << "  \\  \\:\\  /:/   \\  \\::/     \\  \\:\\      \n"
              << "   \\  \\:\\/:/     \\  \\:\\      \\  \\:\\     \n"
              << "    \\  \\::/       \\  \\:\\      \\  \\:\\    \n"
              << "     \\__\\/         \\__\\/       \\__\\/    \n\n\n"
              << logger.colors["reset"];

    logger.custom("https://github.com/vkeshav300/cpm\n\n\n", "github",
                  "red");

    // Specifc command focus
    if (args.size() > 0)
    {
      std::string command = args[0];

      if (command == "help")
        std::cout << "help {command} --> gets information related to CPM and it's commands\n"
                  << "  {command} --> specific command to focus information on";
      else if (command == "version")
        std::cout << "version --> states version of CPM installed";
      else if (command == "create")
        std::cout << "create [language] --> creates new c/c++ project\n"
                  << "  language --> language project will be based off of, C or C++ (ex. cpp)\n"
                  << "  -v=... --> Specify a specific language verison to use (ex. -v=17)"
                  << "Different Project Templates:\n"
                  << "  default  --> 'full' project structure, built around CMake\n"
                  << "  standard --> alias for 'default'\n"
                  << "  simple   --> only creates one main file in working directory";
      else if (command == "fpair")
        std::cout << "fpair [method] [name] --> performs method on header/source file pair\n"
                  << "  [method] --> what operation to perform on file pair\n"
                  << "    create --> creates new file pair\n"
                  << "    destroy --> destroys existing file pair\n"
                  << "  [name] --> name of file pair\n"
                  << "  -hpp --> use hpp header format (only useful for create method)";
      else
      {
        logger.error_q("does not have any help-related information", command);
        return 1;
      }

      std::cout << "\n\n";
    }
    else
      std::cout << "Reading Guide:\n"
                << "  [Square brackets] are required arguments\n"
                << "  {Curly brackets} are optional arguments / flags\n\n\n"
                << "Commands:\n"
                << "  help {command} --> lists commands + other useful information related to CPM\n"
                << "  version --> states version of CPM installed\n"
                << "  create [language] --> creates new c/c++ project\n"
                << "  fpair [method] [name] --> performs method on header/source file pair\n"
                << "\n";

    return 0;
  }

  /**
   * @brief Outputs current CPM version.
   *
   * @return int
   */
  int version()
  {
    logger.custom("cpm version 0.1.0", "version", "red");

    return 0;
  }

  /**
   * @brief Checks if cpm has been initialized in the directory by looking for a
   * "cpm" file.
   *
   * @return true
   * @return false
   */
  bool verify_init()
  {
    return directory::has_file("cpm.data");
  }

  /**
   * @brief Creates new project in working directory.
   *
   * @param args
   * @param flags
   * @return int
   */
  int create(const std::vector<std::string> &args, const std::vector<std::string> &flags)
  {
    std::string lang = args[0];

    if (lang == "c++")
      lang = "cpp";

    if (lang != "cpp" && lang != "c")
    {
      logger.error_q("is not a supported language", lang);
      return 1;
    }

    std::string project_name = logger.prompt("enter project name");

    std::string structure;

    if (logger.prompt_yn("use custom template"))
    {
      while (true)
      {
        structure = logger.prompt("enter the name of a template you would like to use");

        if (!misc::vector_contains(supported_structures, structure))
          logger.warn_q("is an invald template name", structure);
        else
          break;
      }
    }
    else
      structure = "default";

    bool git_support = logger.prompt_yn("add git support");

    std::vector<std::string> folders = {};

    std::vector<std::string> files = {};

    std::string main_file;

    if (structure == "default" || structure == "standard")
    {
      folders.emplace_back("include");
      folders.emplace_back("build");
      folders.emplace_back("tests");
      folders.emplace_back("src");

      files.emplace_back("CMakeLists.txt");

      main_file = "src/" + project_name + "." + lang;
      files.emplace_back(main_file);
    }
    else if (structure == "simple")
    {
      main_file = project_name + "." + lang;
      files.emplace_back(main_file);
    }

    if (git_support)
    {
      files.emplace_back(".gitignore");
      files.emplace_back("LICENSE");
    }

    for (const auto &folder : folders)
      directory::create_directory(folder);

    for (const auto &file : files)
      directory::create_file(file);

    // Writing to files
    std::ofstream writing_file;
    std::ifstream reading_file;

    writing_file.open(main_file);

    writing_file << "#include <iostream>\n"
                 << "\n"
                 << "int main(int argc, char *argv[])\n"
                 << "{\n"
                 << "  std::cout << \"Hello World!\" << std::endl;"
                 << "  return 0;\n"
                 << "}";

    writing_file.close();

    if (misc::vector_contains(files, "CMakeLists.txt") && logger.execute("cmake --version"))
    {
      writing_file.open("CMakeLists.txt");
      reading_file.open("cpm.tmp");

      // Use installed CMAKE version
      std::string cmake_current_version;
      std::getline(reading_file, cmake_current_version);
      cmake_current_version = misc::split_string(cmake_current_version, " ")[2];

      std::string cmake_lang = (lang == "cpp") ? "CXX" : "C";
      std::string version = (flags.size() > 0) ? misc::get_flag_value(flags[0]) : "23";

      writing_file << "cmake_minimum_required(VERSION "
                   << cmake_current_version
                   << ")\n\n"
                   << "project(\n"
                   << "    "
                   << project_name
                   << "\n"
                   << "    LANGUAGES "
                   << cmake_lang
                   << "\n)\n\n"
                   << "set(CMAKE_"
                   << cmake_lang
                   << "_STANDARD "
                   << version
                   << ")\n";

      writing_file.close();
    }

    reading_file.close();

    data_handler.data["language"] = lang;
    data_handler.data["structure"] = structure;

    return 0;
  }

  /**
   * @brief Creates / destroys header and source file pairs.
   *
   * @param args
   * @param flags
   * @return int
   */
  int file_pair(const std::vector<std::string> &args, const std::vector<std::string> &flags)
  {
    if (!data_handler.has_key("structure"))
    {
      logger.error("structure information lacking from cpm.store");
      return 1;
    }

    // Whether to use include/ src/ folders
    bool prefix = (data_handler.data["structure"] == "simple") ? false : true;

    if (args[0] == "create")
    {
      // File related variables
      std::string header_extension = (misc::vector_contains(flags, "hpp")) ? ".hpp" : ".h";
      std::string header_path = (prefix ? "include/" : "") + args[1] + header_extension;
      std::string source_path = (prefix ? "src/" : "") + args[1] + ((data_handler.data["language"] == "cpp") ? ".cpp" : ".c");

      // Creating and writing to header file
      std::ofstream file(header_path);

      if (!file.is_open())
      {
        logger.error_q("could not be opened", header_path);
        return 1;
      }

      file << "#pragma once\n\n";

      file.close();

      // Creating and writing to source file
      file.open(source_path);

      if (!file.is_open())
      {
        logger.error_q("could not be opened", source_path);
        return 1;
      }

      file << "#include \""
           << args[1]
           << header_extension
           << "\"\n\n";
    }
    else if (args[0] == "destroy")
    {
      if (prefix)
      {
        directory::destroy_file("include/" + args[1] + ".h");
        directory::destroy_file("include/" + args[1] + ".hpp");
        directory::destroy_file("src/" + args[1] + ".c");
        directory::destroy_file("src/" + args[1] + ".cpp");
      }
      else
      {
        directory::destroy_file(args[1] + ".h");
        directory::destroy_file(args[1] + ".hpp");
        directory::destroy_file(args[1] + ".c");
        directory::destroy_file(args[1] + ".cpp");
      }
    }
    else
    {
      logger.error_q("is an invalid sub-command", args[0]);
      return 1;
    }

    return 0;
  }
} // namespace commands