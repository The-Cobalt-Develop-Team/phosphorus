# Makefile to automatically call CMake for building the project

# Variables
BUILD_DIR := build
GENERATOR := Ninja
CMAKE := cmake
BUILD := $(CMAKE) --build $(BUILD_DIR)

# Default target
all: configure build

# Configure the project with CMake
configure:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -G $(GENERATOR)

# Build the project
build:
	@$(BUILD) -j$(shell nproc)

# Clean the build directory
clean:
	@rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all configure build clean