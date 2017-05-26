CXX = c++
CXX_FLAGS = -std=c++11 -Wfatal-errors -Wall

BIN = katana
# Put all auto generated stuff to this build dir.
BUILD_DIR = ./build

# List of all .cpp source files.
SOURCE = $(wildcard src/*.cc)

# All .o files go to build dir.
OBJ = $(SOURCE:%.cc=$(BUILD_DIR)/%.o)
# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJ:%.o=%.d)

# Default target named after the binary.
$(BIN) : $(BUILD_DIR)/$(BIN)

debug: CXX_FLAGS += -DDEBUG_ENABLED -g
debug: $(BIN)

# Actual target of the binary - depends on all .o files.
$(BUILD_DIR)/$(BIN) : $(OBJ)
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $^ -o $@

# Include all .d files
-include $(DEP)

# Build target for every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
$(BUILD_DIR)/%.o : %.cc
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@

.PHONY : clean
clean :
	-rm $(BUILD_DIR)/$(BIN) $(OBJ) $(DEP)
