#ifndef MINIDFS_LOGGER_H
#define MINIDFS_LOGGER_H

#include <memory>
#include <spdlog/spdlog.h>

namespace minidfs {

/**
 * @brief Logger utility for MiniDFS components
 * 
 * Provides a shared logger instance with colorized console output.
 */
class Logger {
public:
    /**
     * @brief Get the shared logger instance
     * @return Shared pointer to spdlog logger
     */
    static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace minidfs

#endif // MINIDFS_LOGGER_H
