# Copyright (c) 2022 Qingsong Gou <gouqs@hotmail.com>
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_include_directories(.)

zephyr_library_sources(pcf8653.c)
zephyr_library_sources_ifdef(CONFIG_PCF8653_TRIGGER pcf8653_trigger.c)
