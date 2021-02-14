BUILD_TYPE          ?= Relese
BUILD_STATIC        ?= OFF
DISABLE_ASSERTS     ?= OFF

SRC_DIR             := $(PWD)
BUILD_DIR           := build

CMAKE_OPTIONS       := -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DGE_STATIC=$(BUILD_STATIC) \
                       -DGE_DISABLE_ASSERTS=$(DISABLE_ASSERTS)

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
