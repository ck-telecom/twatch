# SPDX-License-Identifier: Apache-2.0

add_subdirectory_ifdef(CONFIG_BMA423 bma423)
add_subdirectory_ifdef(CONFIG_PCF8653 pcf8653)
add_subdirectory_ifdef(CONFIG_PCF8653 axp202)
