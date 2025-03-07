/**
 * @file version_command.cpp
 * @brief Adds functionality to version command
 * @version 0.1
 * @date 2025-02-23
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "../../include/commands/version_command.h"

#include "../../include/config.h"

/**
 * @brief Construct a new Version_Command object
 *
 */
Version_Command::Version_Command() {}

/**
 * @brief Execute version command
 *
 * @param args
 * @param flags
 * @return uint8_t
 */
uint8_t Version_Command::execute(const std::vector<std::string> &args,
                                 const std::vector<std::string> &flags) const {
  logger.custom(version_string, "version", "theme");
  return 0;
}

/**
 * @brief Gets description of command
 *
 * @return std::string
 */
std::string Version_Command::get_description() const {
  return "Logs installed version of CPM";
}

/**
 * @brief Gets command arguments
 *
 * @return std::string
 */
std::string Version_Command::get_arguments() const { return "None"; }

/**
 * @brief Gets command flags
 *
 * @return std::string
 */
std::string Version_Command::get_flags() const { return "None"; }

/**
 * @brief Gets minimum arguments
 *
 * @return uint16_t
 */
uint16_t Version_Command::get_min_args() const { return 0; }