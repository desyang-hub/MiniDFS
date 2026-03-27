#ifndef MINIDFS_LOGGER_H
#define MINIDFS_LOGGER_H

#include <memory>
#include <spdlog/spdlog.h>

namespace minidfs {
    class Logger {
    public:
        static std::shared_ptr<spdlog::logger> get_logger();
    };
}

#endif // MINIDFS_LOGGER_H
