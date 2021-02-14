BUILD_TYPE          ?= Relese
BUILD_STATIC        ?= OFF
DISABLE_ASSERTS     ?= OFF
BUILD_EXAMPLES      ?= OFF

SRC_DIR             := $(PWD)
BUILD_DIR           := build

CMAKE_OPTIONS       := -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DGE_STATIC=$(BUILD_STATIC) \
                       -DGE_DISABLE_ASSERTS=$(DISABLE_ASSERTS) -DGE_BUILD_EXAMPLES=$(BUILD_EXAMPLES)

CLANG_FORMAT_BIN    ?= clang-format
RUN_CLANG_TIDY_BIN  ?= run-clang-tidy

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
