#if !defined(L2D_LOGGING_H)
#define L2D_LOGGING_H

#define L2D_LOG_NONE 33
#define L2D_LOG_IMPORTANT 32
#define L2D_LOG_ALL 31 

#if !defined(L2D_LOG_TARGET)
#define L2D_LOG_TARGET L2D_LOG_ALL
#endif

#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace l2d{
    class log{
        private:
            std::string output_file_path;
            std::shared_ptr<spdlog::logger> console_logger;
            std::shared_ptr<spdlog::logger> file_logger;
        public:
            log();
            log(std::string logfile_path);
            ~log() = default;

            inline void set_level(unsigned int level) 
            { 
                spdlog::set_level((spdlog::level::level_enum)level);
            }
#if L2D_LOG_TARGET == L2D_LOG_ALL
            template<typename... Args>
            inline void info(const char *fmt, const Args &... args)
            {
                console_logger->info(fmt, args...);
                file_logger->info(fmt, args...);
            }

            template<typename... Args>
            inline void warn(const char *fmt, const Args &... args)
            {
                console_logger->warn(fmt, args...);
                file_logger->warn(fmt, args...);
            }

            template<typename... Args>
            inline void error(const char *fmt, const Args &... args)
            {
                console_logger->error(fmt, args...);
                file_logger->error(fmt, args...);
            }

            template<typename... Args>
            inline void critical(const char *fmt, const Args &... args)
            {
                console_logger->critical(fmt, args...);
                file_logger->critical(fmt, args...);
            }
#elif L2D_LOG_TARGET == L2D_LOG_IMPORTANT
            template<typename... Args>
            inline void info(const char *fmt, const Args &... args)
            {
            }

            template<typename... Args>
            inline void warn(const char *fmt, const Args &... args)
            {
            }

            template<typename... Args>
            inline void error(const char *fmt, const Args &... args)
            {
                console_logger->error(fmt, args...);
                file_logger->error(fmt, args...);
            }

            template<typename... Args>
            inline void critical(const char *fmt, const Args &... args)
            {
                console_logger->critical(fmt, args...);
                file_logger->critical(fmt, args...);
            }
#elif L2D_LOG_TARGET == L2D_LOG_NONE
            template<typename... Args>
            inline void info(const char *fmt, const Args &... args)
            {
            }

            template<typename... Args>
            inline void warn(const char *fmt, const Args &... args)
            {
            }

            template<typename... Args>
            inline void error(const char *fmt, const Args &... args)
            {
            }

            template<typename... Args>
            inline void critical(const char *fmt, const Args &... args)
            {
            }
#endif
    };
}

#endif // L2D_LOGGING_H
