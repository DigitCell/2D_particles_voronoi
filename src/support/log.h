#ifndef LOG_H
#define LOG_H

#include <memory>
#include <cstdlib>
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/async.h"


class Log {
    public:
        static void Init() ;
        static void Init( std::string logFolder);

        inline static std::shared_ptr<spdlog::logger> & GetLogger() {return _logger;}
    private:
        static std::shared_ptr<spdlog::logger> _logger;

};

#define LOG_CRITICAL(...) Log::GetLogger()->critical(__VA_ARGS__);
#define LOG_CRITICAL_THROW(msg) Log::GetLogger()->critical(msg); throw new std::runtime_error(msg);
#define LOG_ERROR(...) Log::GetLogger()->error(__VA_ARGS__);
#define LOG_WARNING(...) Log::GetLogger()->warn(__VA_ARGS__);
#define LOG_DEBUG(...) Log::GetLogger()->debug(__VA_ARGS__);
#define LOG_INFO(...)  Log::GetLogger()->info(__VA_ARGS__);
#define LOG_TRACE(...) Log::GetLogger()->trace(__VA_ARGS__);

#endif //LOG_H