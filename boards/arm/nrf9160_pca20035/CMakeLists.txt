# Kconfig - nRF91 PCA20035 board configuration
#
# Copyright (c) 2019 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic

include(../../../../nrf/cmake/boilerplate.cmake NO_POLICY_SCOPE)

if(CONFIG_BOARD_NRF9160_PCA20035 AND NOT DEFINED CONFIG_MCUBOOT)
	zephyr_library()
	zephyr_library_sources(board_secure.c)
endif()

if(CONFIG_BOARD_NRF9160_PCA20035NS)
	zephyr_library()
	zephyr_library_sources(board_nonsecure.c)
endif()

if(CONFIG_BOARD_NRF52840_GPIO_RESET)
  zephyr_library_sources(nrf52840_reset.c)
endif()
