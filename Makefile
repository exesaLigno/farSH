SRC_DIR = lib
HEADER_DIR = include
BUILD_DIR = build
OBJECT_DIR = $(BUILD_DIR)/obj
EXECUTABLES_DIR = $(BUILD_DIR)/executable

CXX = g++
CXX_STANDARD = c++2a
CXXFLAGS = -I$(HEADER_DIR) --std=$(CXX_STANDARD)
CXXFLAGS_RELEASE = -O3
CXXFLAGS_DEBUG = -O0 -g
LDFLAGS = 

ifdef DEBUG
	CXXFLAGS += $(CXXFLAGS_DEBUG)
else
	CXXFLAGS += $(CXXFLAGS_RELEASE)
endif

SOURCES = $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJECT_DIR)/%.o)

TARGET = $(EXECUTABLES_DIR)/farsh

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(TARGET)
