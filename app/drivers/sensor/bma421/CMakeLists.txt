# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_include_directories(.)
zephyr_include_directories(bma421)

zephyr_library_sources_ifdef(CONFIG_BMA423 bma423/bma4.c)
zephyr_library_sources_ifdef(CONFIG_BMA423 bma423/bma421.c)

zephyr_library_sources_ifdef(CONFIG_BMA423 bma423.c)
zephyr_library_sources_ifdef(CONFIG_BMA423_TRIGGER bma423_trigger.c)
