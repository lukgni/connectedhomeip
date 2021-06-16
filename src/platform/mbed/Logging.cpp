/* See Project CHIP LICENSE file for licensing information. */

#include <core/CHIPConfig.h>
#include <platform/logging/LogV.h>
#include <support/logging/CHIPLogging.h>
#include <support/logging/Constants.h>

#include <stdio.h>
#include <string.h>

#include "mbed-trace/mbed_trace.h"

#define TRACE_GROUP "CHIP"

#ifdef MBED_TRACE_FILTER_LENGTH
#define DEFAULT_TRACE_FILTER_LENGTH MBED_TRACE_FILTER_LENGTH
#else
#define DEFAULT_TRACE_FILTER_LENGTH 24
#endif

namespace chip {
namespace DeviceLayer {

/**
 * Called whenever a log message is emitted by chip or LwIP.
 *
 * This function is intended be overridden by the application to, e.g.,
 * schedule output of queued log entries.
 */
void __attribute__((weak)) OnLogOutput() {}

} // namespace DeviceLayer

namespace Logging {
namespace Platform {

/**
 * CHIP log output functions.
 */
void LogV(const char * module, uint8_t category, const char * msg, va_list v)
{
    char formattedMsg[CHIP_CONFIG_LOG_MESSAGE_MAX_SIZE];
    size_t prefixLen = 0;

    snprintf(formattedMsg, sizeof(formattedMsg), "[%s]", module);
    formattedMsg[sizeof(formattedMsg) - 2] = 0; // -2 to allow at least one char for the vsnprintf
    prefixLen                              = strlen(formattedMsg);
    vsnprintf(formattedMsg + prefixLen, sizeof(formattedMsg) - prefixLen, msg, v);

    switch (category)
    {
    case kLogCategory_Error:
        tr_err("%s", formattedMsg);
        break;
    case kLogCategory_Progress:
        tr_info("%s", formattedMsg);
        break;
    case kLogCategory_Detail:
    default:
        tr_debug("%s", formattedMsg);
        break;
    }

    // Let the application know that a log message has been emitted.
    DeviceLayer::OnLogOutput();
}

} // namespace Platform
} // namespace Logging
} // namespace chip

void mbed_logging_init()
{
    // Mbed trace initalization
    mbed_trace_init();
    mbed_trace_include_filters_set(TRACE_GROUP);
    mbed_trace_config_set(TRACE_ACTIVE_LEVEL_ALL | TRACE_MODE_COLOR);

#if MBED_BSD_SOCKET_TRACE
    char filtersStr[DEFAULT_TRACE_FILTER_LENGTH];
    auto filters = mbed_trace_include_filters_get();
    snprintf(filtersStr, sizeof(filtersStr), "%s,BSDS,NETS", filters);
    mbed_trace_include_filters_set(filtersStr);
#endif

#if CHIP_LOG_FILTERING
    // Set default CHIP log filter = kLogCategory_Progress
    chip::Logging::SetLogFilter(chip::Logging::LogCategory::kLogCategory_Progress);
#endif
}
