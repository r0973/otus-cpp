/**
 * @file lib_version.h
 * @brief Library version information
 * 
 * Contains version-related functions and definitions for the project.
 */

#pragma once

/**
 * @brief Gets the current project version
 * @return Null-terminated string containing version in "MAJOR.MINOR.PATCH" format
 * 
 * Example of usage:
 * @code
 * int version = get_version();
 * std::cout << "Project version: " << version << std::endl;
 * @endcode
 */
int version();