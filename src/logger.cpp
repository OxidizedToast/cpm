/**
 * @file logger.cpp
 * @brief Provides majority of functionality to Logger singleton.
 * @version 0.1
 * @date 2023-11-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "logger.h"
#include "misc.h"

#include <iostream>
#include <cstdlib>
#include <fstream>

/**
 * @brief Get method for logger class.
 *
 * @return Logger*
 */
Logger &Logger::get()
{
  static Logger logger;
  return logger;
}

/**
 * @brief Sets color map for logger.
 *
 * @param new_colors
 */
void Logger::set_colors(const std::map<std::string, std::string> &new_colors)
{
  colors = new_colors;
}

/**
 * @brief Flushes output buffer.
 *
 */
void Logger::flush_buffer() { std::cout.flush(); }

/**
 * @brief Handles logger count.
 *
 */
void Logger::handle_logger_count()
{
  std::cout << colors["count"]
            << "["
            << logger_count
            << "]"
            << colors["reset"];

  if (logger_count < 10)
    std::cout << " ";

  if (logger_count < 100)
    std::cout << " ";

  logger_count++;
}

/**
 * @brief Logs success message to console.
 *
 * @param message Text to be logged.
 */
void Logger::success(const std::string &message)
{
  handle_logger_count();
  std::cout << colors["success"]
            << "[success]: "
            << colors["reset"]
            << message
            << "\n";
}

/**
 * @brief Logs success message with quote to console.
 *
 * @param message Text to be logged.
 * @param quote Text in quote.
 */
void Logger::success_q(const std::string &message, const std::string &quote)
{
  success("\'" + quote + "\' " + message);
}

/**
 * @brief Logs error message to console.
 *
 * @param message Text to be logged.
 */
void Logger::error(const std::string &message)
{
  handle_logger_count();
  std::cerr << colors["error"]
            << "[error]: "
            << colors["reset"]
            << message
            << "\n";
}

/**
 * @brief Logs error message with quote to console.
 *
 * @param message Text to be logged.
 * @param quote Text in quote.
 */
void Logger::error_q(const std::string &message, const std::string &quote)
{
  error("\'" + quote + "\' " + message);
}

/**
 * @brief Logs warning message to console.
 *
 * @param message
 */
void Logger::warn(const std::string &message)
{
  handle_logger_count();
  std::cout << colors["warn"]
            << "[warning]: "
            << colors["reset"]
            << message
            << "\n";
}

/**
 * @brief Logs warning message with quote to console.
 *
 * @param message Text to be logged.
 * @param quote Text in quote.
 */
void Logger::warn_q(const std::string &message, const std::string &quote)
{
  warn("\'" + quote + "\' " + message);
}

/**
 * @brief Logs custom message to console.
 *
 * @param message Text to be logged.
 * @param mtype Message type.
 * @param color Message color.
 */
void Logger::custom(const std::string &message, const std::string &mtype, const std::string &color)
{
  handle_logger_count();
  std::cout << raw_colors[color]
            << "["
            << mtype
            << "]: "
            << colors["reset"]
            << message
            << "\n";
}

/**
 * @brief Logs an input prompt to console.
 *
 * @param message Prompt.
 * @return std::string
 */
std::string Logger::prompt(const std::string &message)
{
  handle_logger_count();

  std::cout << colors["prompt"]
            << "[prompt]: "
            << colors["reset"]
            << message
            << ": ";

  std::string line;
  getline(std::cin, line);

  return line;
}

/**
 * @brief Logs y/n prompt to console.
 *
 * @param message Prompt.
 * @return true
 * @return false
 */
bool Logger::prompt_yn(const std::string &message)
{
  std::string response;
  while (true)
  {
    response = prompt(message + " [y/n]");

    if (response == "y" || response == "yes")
      return true;
    else if (response == "n" || response == "no")
      return false;
    else
      warn_q("is not a valid response, try again", response);
  }
}

/**
 * @brief Executes terminal command.
 * 
 * @param command Command to execute.
 * @param must_populate_file (Optional -> default: true) whether to throw error if command does not output.
 * @return true 
 * @return false 
 */
bool Logger::execute(const std::string &command, const bool &must_populate_file)
{
  handle_logger_count();

  // Prefix
  std::cout << colors["execute"]
            << "[executing]: "
            << colors["reset"]
            << command
            << "\n";

  // Command execution
  std::system((command + " > cpm.tmp").c_str());

  // Empty response is indicator of command failure (unless command does not give a response)
  if (must_populate_file)
  {
    std::ifstream file("cpm.tmp");

    if (!file.is_open())
    {
      error("could not open cpm.tmp");
      return false;
    }

    // Checks if cpm.tmp file is empty
    if (file.peek() == std::ifstream::traits_type::eof())
    {
      error("command failed to execute");
      file.close();

      return false;
    }

    file.close();
  }

  handle_logger_count();
  
  std::cout << colors["execute"]
            << "[executed]: "
            << colors["reset"]
            << command
            << "\n";

  return true;
}