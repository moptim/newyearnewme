TARGET_EXEC ?= newyearnewme
SOINTU_COMPILE ?= cmd/sointu-compile/main.go
SOUNDTRACK_SRC ?= soundtrack.yml
SOUNDTRACK_OBJ ?= soundtrack.o

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
SOINTU_DIR ?= ./sointu
MUSIC_DIR ?= ./music

SOUNDTRACK_OBJ_PATH ?= $(BUILD_DIR)/$(SRC_DIRS)/$(SOUNDTRACK_OBJ)
SRCS := $(shell find $(SRC_DIRS) -name "*.cc")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o) $(SOUNDTRACK_OBJ_PATH)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d) /usr/include/freetype2
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CXXFLAGS ?= $(INC_FLAGS) -MMD -MP --std=c++14 -g # -I/usr/include/freetype2/

LDFLAGS ?= -lSDL2 -lGL -lfreetype -pthread -lIL

$(BUILD_DIR)/$(TARGET_EXEC): compile_soundtrack $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cc.o: %.cc
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

run:
	$(BUILD_DIR)/$(TARGET_EXEC)

compile_soundtrack:
	cd $(SOINTU_DIR); go run $(SOINTU_COMPILE) -o ../$(SRC_DIRS) -arch=amd64 ../$(MUSIC_DIR)/$(SOUNDTRACK_SRC)
	mkdir -p $(BUILD_DIR)/$(SRC_DIRS)
	nasm -f elf64 $(SRC_DIRS)/soundtrack.asm -o $(SOUNDTRACK_OBJ_PATH)

-include $(DEPS)

MKDIR_P ?= mkdir -p
