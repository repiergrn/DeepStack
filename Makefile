PROJECT_NAME = Stack
BUILD_DIR = cmake-build-debug
RELEASE_DIR = cmake-build-release

ELF_FILE_DEBUG = $(BUILD_DIR)/$(PROJECT_NAME).elf
BIN_FILE_DEBUG = $(BUILD_DIR)/$(PROJECT_NAME).bin
ELF_FILE_RELEASE = $(RELEASE_DIR)/$(PROJECT_NAME).elf

PROGRAMMER ?= STM32_Programmer_CLI
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
PARALLEL_JOBS = $(shell sysctl -n hw.ncpu)
else
PARALLEL_JOBS = $(shell nproc)
endif

.PHONY: build rebuild release clean flash flash_bin erase size

build: configure_debug
	cmake --build $(BUILD_DIR) -j$(PARALLEL_JOBS)

rebuild: clean build

release: configure_release
	cmake --build $(RELEASE_DIR) -j$(PARALLEL_JOBS)

clean:
	@rm -rf $(BUILD_DIR) $(RELEASE_DIR)

flash: build
	$(PROGRAMMER) -c port=SWD -w $(ELF_FILE_DEBUG)

flash_bin: build
	$(PROGRAMMER) -c port=SWD -w $(BIN_FILE_DEBUG) 0x08000000

erase:
	$(PROGRAMMER) -c port=SWD -e all

size: build
	arm-none-eabi-size --format=berkeley $(ELF_FILE_DEBUG)

configure_debug:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -G "Unix Makefiles" .. > /dev/null

configure_release:
	@mkdir -p $(RELEASE_DIR)
	@cd $(RELEASE_DIR) && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -G "Unix Makefiles" .. > /dev/null