# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/npu_sim

# Include directories
INCLUDES = -I$(INC_DIR)

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(INC_DIR) $(SRC_DIR)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run the program using stepping
step: $(TARGET)
	./$(TARGET) --step

# Debug build
debug: CXXFLAGS += -DDEBUG -g
debug: clean all

# Release build
release: CXXFLAGS += -O3 -DNDEBUG
release: clean all

# Install dependencies (if needed)
install-deps:
	# Add any dependency installation commands here
	@echo "No external dependencies required"

# Format code (requires clang-format)
format:
	find $(SRC_DIR) $(INC_DIR) -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Static analysis (requires cppcheck)
analyze:
	cppcheck --enable=all --std=c++17 $(SRC_DIR)

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  clean     - Remove build files"
	@echo "  run       - Build and run the program"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  format    - Format source code (requires clang-format)"
	@echo "  analyze   - Run static analysis (requires cppcheck)"
	@echo "  help      - Show this help message"

# Phony targets
.PHONY: all clean run debug release directories install-deps format analyze help
