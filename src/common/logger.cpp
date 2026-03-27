#include "common/logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace minidfs {
    std::shared_ptr<spdlog::logger> Logger::get_logger() {
        static auto logger = spdlog::stdout_color_mt("minidfs");
        logger->set_level(spdlog::level::debug);
        return logger;
    }
}
