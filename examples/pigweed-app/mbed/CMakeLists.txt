# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19.0)

get_filename_component(CHIP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../.. REALPATH)
get_filename_component(MBED_COMMON ${CHIP_ROOT}/examples/platform/mbed REALPATH)
get_filename_component(APP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/.. REALPATH)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.in
    ${CMAKE_CURRENT_BINARY_DIR}/chip_build/config
    @ONLY
)

set(MBED_PATH $ENV{MBED_OS_PATH} CACHE INTERNAL "")
set(MBED_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_TARGET chip-mbed-pigweed-app-example)
set(PIGWEED_ROOT "${CHIP_ROOT}/third_party/pigweed/repo")

include(${MBED_PATH}/tools/cmake/app.cmake)
add_subdirectory(${MBED_PATH} ./mbed_build)

if("wifi_ism43362" IN_LIST MBED_TARGET_LABELS)
    add_subdirectory(wifi-ism43362)
endif()

add_executable(${APP_TARGET})

include(${PIGWEED_ROOT}/pw_build/pigweed.cmake)
include(${PIGWEED_ROOT}/pw_protobuf_compiler/proto.cmake)

pw_set_backend(pw_log pw_log_basic)
pw_set_backend(pw_assert pw_assert_log)
pw_set_backend(pw_sys_io pw_sys_io.nrfconnect)
set(dir_pw_third_party_nanopb "${CHIP_ROOT}/third_party/nanopb/repo" CACHE STRING "" FORCE)

add_subdirectory(${CHIP_ROOT}/third_party/nanopb/repo ./nanopb_build)
add_subdirectory(${CHIP_ROOT}/third_party/pigweed/repo ./pigweed_build)

target_include_directories(${APP_TARGET} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${MBED_COMMON}/util/include
                           ${CHIP_ROOT}/src/lib/support
                           ${PIGWEED_ROOT}/pw_sys_io/public
                           ${PIGWEED_ROOT}/pw_span/public_overrides
                           ${PIGWEED_ROOT}/pw_span/public
                           ${PIGWEED_ROOT}/pw_polyfill/public
                           ${PIGWEED_ROOT}/pw_polyfill/standard_library_public
                           ${PIGWEED_ROOT}/pw_polyfill/public_overrides
                           ${PIGWEED_ROOT}/pw_rpc/public
                           ${PIGWEED_ROOT}/pw_status/public
                           ${PIGWEED_ROOT}/pw_preprocessor/public
                           ../../common
                           ../../common/pigweed/mbed
                           ${MBED_COMMON}/pw_sys_io/public)

target_sources(${APP_TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main/main.cpp
    ../../common/pigweed/RpcService.cpp
    ../../common/pigweed/mbed/PigweedLoggerMutex.cpp
    ${MBED_COMMON}/util/LEDWidget.cpp
    ${MBED_COMMON}/util/PigweedLogger.cpp
)

add_subdirectory(${CHIP_ROOT}/config/mbed ./chip_build)

mbed_configure_app_target(${APP_TARGET})

mbed_set_mbed_target_linker_script(${APP_TARGET})

project(${APP_TARGET})

if("wifi_ism43362" IN_LIST MBED_TARGET_LABELS)
    target_link_libraries(${APP_TARGET}
            wifi-ism43362
    )
endif()

if("BlueNRG_MS" IN_LIST MBED_TARGET_LABELS)
    target_link_libraries(${APP_TARGET}
            mbed-ble-blue_nrg
    )
endif()

if("WHD" IN_LIST MBED_TARGET_LABELS)
    target_link_libraries(${APP_TARGET}
        mbed-cy_psoc6_common_network mbed-emac mbed-cy_psoc6_whd
    )
endif()

target_link_libraries(${APP_TARGET} mbed-os mbed-ble mbed-events mbed-netsocket mbed-storage mbed-storage-kv-global-api mbed-mbedtls mbed-mbedtls-cryptocell310 chip)

target_link_libraries(${APP_TARGET}
  pw_checksum
  pw_hdlc
  pw_log
  pw_rpc.nanopb.echo_service
  pw_rpc.server
)

mbed_set_post_build(${APP_TARGET})

option(VERBOSE_BUILD "Have a verbose build process")
if(VERBOSE_BUILD)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()
