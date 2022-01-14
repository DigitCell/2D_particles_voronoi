#include "../include/log.h"

std::shared_ptr<spdlog::logger> Log::_logger;

void Log::Init( std::string logFolder) {
    std::string fileName = "slicer.log";

    std::string consolePattern = "[slicer-%t][%H:%M:%S:%e / %o] [%^%l%$] %v";

    std::string filePattern = "[%t][%H:%M:%S:%e / %o] [%^%l%$] %v";
    auto max_size = 1048576 * 2; // 2MB size
    auto max_files = 3; // 3 rolling files

        
    std::string mkdirCmd = std::string("mkdir -p ") + logFolder;
    const int dir_err = system( mkdirCmd.c_str());
    
    std::string filePath = logFolder + "/" + fileName;



    spdlog::init_thread_pool(8192, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
    stdout_sink->set_level(spdlog::level::trace);
    stdout_sink->set_pattern(consolePattern);
    

    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filePath,max_size, max_files);
    rotating_sink->set_level(spdlog::level::trace);
    rotating_sink->set_pattern(filePattern);
    

    std::vector<spdlog::sink_ptr> sinks {stdout_sink, rotating_sink};  
    
    _logger= std::make_shared<spdlog::logger>("slice-logger", sinks.begin(), sinks.end());
    
    
    _logger->set_level(spdlog::level::trace);
    _logger->flush_on(spdlog::level::trace);

}
void Log::Init() {
    Init("logs");
}
