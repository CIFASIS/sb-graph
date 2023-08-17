# The Directories, Source, Includes, Objects, Binary 
ROOT_DIR       := ../..
SRC_DIR        := .
TEST_DIR   	   := $(SRC_DIR)
BUILD_DIR  	   := $(TEST_DIR)/obj
BIN_DIR        := $(TEST_DIR)/bin

# Flags, Libraries and Includes
GOOGLE_TEST_INSTALL = $(ROOT_DIR)/3rd-party/gtest
GOOGLE_TEST_LIB = gtest
GOOGLE_MOCK_LIB = gmock
GOOGLE_TEST_INCLUDE = $(GOOGLE_TEST_INSTALL)/usr/include
LIB 		+= -lgmp  
G++ = g++
G++_FLAGS = -c -Wall -I ../../ -I $(GOOGLE_TEST_INCLUDE) -std=c++17
LD_FLAGS = -L $(GOOGLE_TEST_INSTALL)/usr/lib -l $(GOOGLE_TEST_LIB) -l $(GOOGLE_MOCK_LIB) -L ../../lib -l pthread -lsbgraph 
RM = rm -rf

# The Target Binary Program
TARGET      := $(BIN_DIR)/sbg-performance
RUN_TESTS   := sbg-performance

# Source files.
MAIN_SRC = $(SRC_DIR)/main.cpp

TEST_SRC = $(TEST_DIR)/set_perf.cpp \
  $(TEST_DIR)/pw_map_perf.cpp

# Objects
TEST_OBJ=$(addprefix $(BUILD_DIR)/test_, $(notdir $(TEST_SRC:.cpp=.o)))

MAIN_OBJ=$(addprefix $(BUILD_DIR)/, $(notdir $(MAIN_SRC:.cpp=.o)))

$(BUILD_DIR)/test_%.o : $(TEST_DIR)/%.cpp
	$(G++) $(G++_FLAGS) $< -o $@

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(G++) $(G++_FLAGS) $< -o $@ 

all: $(TARGET) $(RUN_TESTS)

$(TARGET): $(MAIN_OBJ) $(TEST_OBJ) $(BUILD_DIR)
		g++ -o $(TARGET) $(MAIN_OBJ) $(TEST_OBJ) $(LD_FLAGS) $(LIB)

$(RUN_TESTS): $(TARGET)
		@echo Running performance tests.
		$(TARGET)
		@echo Done.

$(TEST_OBJ): | $(BUILD_DIR)

$(MAIN_OBJ): | $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(BUILD_DIR)

clean:
	$(RM) $(BIN_DIR) $(BUILD_DIR) 
                    
.PHONY: all clean
