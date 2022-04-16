# Copyright (c) 2022 Qingsong Gou <gouqs@hotmail.com>
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_include_directories(.)

zephyr_library_sources(axp202.c)
zephyr_library_sources_ifdef(CONFIG_AXP202_TRIGGER axp202_trigger.c)
