BUILD_TYPE          ?= Release
BUILD_STATIC        ?= OFF
DISABLE_ASSERTS     ?= OFF
BUILD_EXAMPLES      ?= OFF
BUILD_TESTS         ?= OFF

SRC_DIR             := $(PWD)
BUILD_DIR           := build

CMAKE_OPTIONS       ?= -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DGE_STATIC=$(BUILD_STATIC) \
                       -DGE_DISABLE_ASSERTS=$(DISABLE_ASSERTS) -DGE_BUILD_EXAMPLES=$(BUILD_EXAMPLES) \
                       -DGE_BUILD_TESTS=$(BUILD_TESTS)

CLANG_FORMAT_BIN    ?= clang-format
RUN_CLANG_TIDY_BIN  ?= run-clang-tidy

DOCKER_IMAGE_NAME   := genesis-engine-image
DOCKER_CMD          ?= make -j$$(nproc)

# Building project
.PHONY: all
all: build_project

.PHONY: build_project
build_project: generate_makefiles
	$(MAKE) -C $(BUILD_DIR)

.PHONY: generate_makefiles
generate_makefiles:
	mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && cmake $(CMAKE_OPTIONS) $(SRC_DIR)

.PHONY: clean
clean:
	rm -rf build

# Linters
.PHONY: clang-format
clang-format:
	bash tools/clang_format.sh --clang-format-bin $(CLANG_FORMAT_BIN)

.PHONY: clang-tidy
clang-tidy: CMAKE_OPTIONS += -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
clang-tidy: generate_makefiles
	$(RUN_CLANG_TIDY_BIN) -p $(BUILD_DIR)

# Docker
.PHONY: docker_initialize
docker_initialize:
	DOCKER_BUILDKIT=1 docker build -t $(DOCKER_IMAGE_NAME) $(PWD)

.PHONY: docker_cleenup
docker_cleanup:
	docker image rm -f $(DOCKER_IMAGE_NAME)

.PHONY: docker_run
docker_run:
	docker run --rm \
		-ti \
		-v $(PWD):$(PWD) \
		-w $(PWD) \
		-u $$(id -u):$$(id -g) \
		-e CMAKE_OPTIONS="$(CMAKE_OPTIONS)" \
		-e CLANG_FORMAT_BIN="clang-format-11" \
		-e RUN_CLANG_TIDY_BIN="run-clang-tidy-11" \
		$(DOCKER_IMAGE_NAME) \
		bash -c "$(DOCKER_CMD)"

.PHONY: docker_build
docker_build: DOCKER_CMD = make -j$$(nproc)
docker_build: docker_run

# Tests
.PHONY: test
test:
	$(MAKE) -C $(BUILD_DIR) test
