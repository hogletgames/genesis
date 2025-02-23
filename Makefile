BUILD_TYPE          ?= Release
BUILD_STATIC        ?= OFF
DISABLE_ASSERTS     ?= OFF
BUILD_APPS          ?= OFF
BUILD_EXAMPLES      ?= OFF
BUILD_TESTS         ?= OFF

SRC_DIR             := $(PWD)
BUILD_DIR           ?= build

CMAKE_OPTIONS       ?= -DCMAKE_BUILD_TYPE:STRING=$(BUILD_TYPE)                 \
                       -DGE_STATIC:BOOL=$(BUILD_STATIC)                        \
                       -DGE_DISABLE_ASSERTS:BOOL=$(DISABLE_ASSERTS)            \
                       -DGE_BUILD_APPS:BOOL=$(BUILD_APPS)                      \
                       -DGE_BUILD_EXAMPLES:BOOL=$(BUILD_EXAMPLES)              \
                       -DGE_BUILD_TESTS:BOOL=$(BUILD_TESTS)

CLANG_FORMAT_BIN    ?= clang-format
RUN_CLANG_TIDY_BIN  ?= run-clang-tidy

export DOCKER_BUILDKIT := 1
export USERID          ?= $(shell id -u)
export GROUPID         ?= $(shell id -g)

# Building project
.PHONY: all
all: build_project

.PHONY: build_project
build_project: generate_makefiles
	$(MAKE) -C $(BUILD_DIR)

.PHONY: generate_makefiles
generate_makefiles:
	cmake -S $(SRC_DIR) -B $(BUILD_DIR) $(CMAKE_OPTIONS)

.PHONY: clean
clean:
	rm -rf build

# Linters
.PHONY: clang-format
clang-format:
	bash tools/clang_format.sh --clang-format-bin $(CLANG_FORMAT_BIN)

.PHONY: clang-tidy
clang-tidy: CMAKE_OPTIONS += -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON           \
                             -DGE_BUILD_APPS:BOOL=ON                           \
                             -DGE_BUILD_EXAMPLES:BOOL=ON
clang-tidy: generate_makefiles
	$(MAKE) -C $(BUILD_DIR) sdl_headers_copy
	$(RUN_CLANG_TIDY_BIN) -p $(BUILD_DIR) -j$$(nproc)

# Docker
.PHONY: docker_initialize
docker_initialize:
	docker compose build

.PHONY: docker_build
docker_build:
	CMAKE_OPTIONS="$(CMAKE_OPTIONS)"                                           \
	    docker compose run                                                     \
	        --rm                                                               \
	        --remove-orphans                                                   \
	        build-env

.PHONY: docker_env_up
docker_env_up:
	docker compose up --detach --remove-orphans runtime-env

.PHONY: docker_down
docker_env_down:
	docker compose down runtime-env

.PHONY: docker_env_attach
docker_env_attach:
	docker compose exec runtime-env bash

# Tests
.PHONY: test
test:
	cd $(BUILD_DIR) && ctest --output-on-failure -j$$(nproc)
