# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(.)

zephyr_library_sources(event_service_mem.c)
zephyr_library_sources(event_service.c)
zephyr_library_sources(service_accel.c)
