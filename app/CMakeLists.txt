#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app LANGUAGES C)

zephyr_include_directories(src)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/controller.c)
target_sources(app PRIVATE src/game_object.c)
target_sources(app PRIVATE src/sprites/SpritePlayer.c)
target_sources(app PRIVATE src/sprites/SpriteRockTile.c)