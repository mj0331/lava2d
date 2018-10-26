#include "../include/logging.h"

namespace l2d{
    log::log() 
    {
        output_file_path = "";
        file_logger = nullptr;
        spdlog::set_pattern("%^[%H:%M:%S %z] [%n] [---%L---] [thread %t] %v %$");
        console_logger = spdlog::stdout_color_mt("l2d_console_logger");
    }

    log::log(std::string logfile_path)
    {
        output_file_path = logfile_path;
        //spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
        spdlog::set_pattern("%^[%H:%M:%S %z] [%n] [---%L---] [thread %t] %v %$");        
        console_logger = spdlog::stdout_color_mt("l2d_console_logger");
        file_logger = spdlog::basic_logger_mt("l2d_file_logger", output_file_path);
    }
}