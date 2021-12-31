#include "pch.h"

#include "Log.h"
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

const std::string PATTERN = "%^%I:%M:%S %-8l %-11n: %v%$";

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init() {

   auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>();
   auto stdout_color_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
   auto vs_output_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

   dist_sink->add_sink(stdout_color_sink);
   dist_sink->add_sink(vs_output_sink);
   dist_sink->set_pattern(PATTERN);

   s_CoreLogger = std::make_shared<spdlog::logger>("Adventure2", dist_sink);
   s_CoreLogger->set_level(spdlog::level::trace);

   s_ClientLogger = std::make_shared<spdlog::logger>("Application", dist_sink);
   s_ClientLogger->set_level(spdlog::level::trace);
}
