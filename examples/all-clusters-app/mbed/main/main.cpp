/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "AppTask.h"

#include "mbedtls/platform.h"
#include <platform/CHIPDeviceLayer.h>
#include <support/CHIPMem.h>
#include <support/logging/CHIPLogging.h>

#include <platform/mbed/Logging.h>

using namespace chip;
using namespace chip::Inet;
using namespace chip::Transport;
using namespace chip::DeviceLayer;
using namespace ::chip::Logging::Platform;

int main(int argc, char * argv[])
{
    int ret = 0;

    mbed_logging_init();

    ret = mbedtls_platform_setup(NULL);
    if (ret)
    {
        ChipLogError(NotSpecified, "Mbed TLS platform initialization failed with error %d", ret);
        goto exit;
    }

    ret = chip::Platform::MemoryInit();
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogError(NotSpecified, "Platform::MemoryInit() failed");
        goto exit;
    }

    ChipLogProgress(NotSpecified, "Init CHIP Stack\r\n");
    ret = PlatformMgr().InitChipStack();
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogError(NotSpecified, "PlatformMgr().InitChipStack() failed");
    }

    ChipLogProgress(NotSpecified, "Starting CHIP task");
    ret = PlatformMgr().StartEventLoopTask();
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogError(NotSpecified, "PlatformMgr().StartEventLoopTask() failed");
        goto exit;
    }

    ret = GetAppTask().StartApp();

exit:
    ChipLogProgress(NotSpecified, "Exited with code %d", ret);
    return ret;
}
