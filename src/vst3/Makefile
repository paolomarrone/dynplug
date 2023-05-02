ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

NAME := dynplug
SOURCES = ${SOURCES_COMMON} ${ROOT_DIR}/../common/dynplug.c

include ${ROOT_DIR}/vst3.mk
