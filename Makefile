SRC_DIR = lib
HEADER_DIR = include
BUILD_DIR = build
OBJECT_DIR = $(BUILD_DIR)/obj
EXECUTABLES_DIR = $(BUILD_DIR)/executable
EXECUTABLE_NAME = farsh

CXX = g++
CXX_STANDARD = c++2a
CXXFLAGS = -I$(HEADER_DIR) --std=$(CXX_STANDARD) -MD
CXXFLAGS_RELEASE = -O3
CXXFLAGS_DEBUG = -O0 -g -DDEBUG=1
LDFLAGS = 

ifdef DEBUG
	CXXFLAGS += $(CXXFLAGS_DEBUG)
else
	CXXFLAGS += $(CXXFLAGS_RELEASE)
endif

ifdef ASAN
	CXXFLAGS += -fsanitize=address
	LDFLAGS += -fsanitize=address
endif

ifdef UBSAN
	CXXFLAGS += -fsanitize=undefined
	LDFLAGS += -fsanitize=undefined
endif

SOURCES = $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJECT_DIR)/%.o)

TARGET = $(EXECUTABLES_DIR)/$(EXECUTABLE_NAME)

ALL: $(TARGET)

.PHONY: install
install: $(TARGET)
	cp $(EXECUTABLES_DIR)/$(EXECUTABLE_NAME) /usr/bin/$(EXECUTABLE_NAME)

.PHONY: uninstall
uninstall:
	rm /usr/bin/$(EXECUTABLE_NAME)

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJECT_DIR) $(EXECUTABLES_DIR) $(TARGET)

.PHONY: cloc
cloc:
	cloc ./ --exclude-lang=D,JSON,Markdown,YAML,make

-include $(OBJECTS:.o=.d)
