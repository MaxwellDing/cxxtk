#pragma once

#include <cstdlib>
#include <cstring>
#include <string>

namespace util {

/**
 * @brief Get bool value from environment variable
 *
 * @note "false" and "0" means false, otherwise true
 * @param env_str Name of environment variable
 * @param default_val Default value if specified environment variable is not set
 * @return bool Bool value in environment variable
 */
inline bool getBoolFromEnv(const char* env_str, bool default_val = false) noexcept {
  char* var = std::getenv(env_str);
  if (!var) return default_val;

  if (strcmp(var, "false") == 0 || strcmp(var, "0") == 0) {
    return false;
  } else {
    return true;
  }
}

/**
 * @brief Get int value from environment variable
 *
 * @exception Same as std::stoi
 * @param env_str Name of environment variable
 * @param default_val Default value if specified environment variable is not set
 * @return int Int value in environment variable
 */
inline int getIntFromEnv(const char* env_str, int default_val = 0) {
  char* var = std::getenv(env_str);
  if (!var) return default_val;

  return std::stoi(var);
}

/**
 * @brief Get ulong value from environment variable
 *
 * @exception Same as std::stuol
 * @param env_str Name of environment variable
 * @param default_val Default value if specified environment variable is not set
 * @return unsigned long Unsigned long value in environment variable
 */
inline unsigned long getUlongFromEnv(const char* env_str, unsigned long default_val = 0) {  // NOLINT
  char* var = std::getenv(env_str);
  if (!var) return default_val;

  return std::stoul(var);
}

/**
 * @brief Get string value from environment variable
 *
 * @param env_str Name of environment variable
 * @param default_val Default value if specified environment variable is not set
 * @return std::string String value in environment variable
 */
inline std::string getStringFromEnv(const char* env_str, const std::string& default_val = "") noexcept {
  char* var = std::getenv(env_str);
  if (!var) return default_val;
  return std::string(var);
}

}  // namespace util

