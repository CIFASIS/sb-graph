# The Directories, Source, Includes, Objects, Binary 
ROOT_DIR    := ../..
TEST_DIR    := .
BOOST_DIR   := boost
OBJ_DIR     := $(TEST_DIR)/obj
BUILD_DIR   := $(OBJ_DIR)
BIN_DIR     := $(TEST_DIR)/bin
PERFORMANCE := $(BIN_DIR)/sbg-performance
BOOST_PERF  := $(BIN_DIR)/boost-perf

#///////////////////////////////////////////////////////////////////////////////
# Compilation ------------------------------------------------------------------ 
#///////////////////////////////////////////////////////////////////////////////

all: $(PERFORMANCE) $(BOOST_PERF)

# Flags, Libraries and Includes
GOOGLE_TEST_INSTALL = $(ROOT_DIR)/3rd-party/gtest
GOOGLE_TEST_LIB = gtest
GOOGLE_MOCK_LIB = gmock
GOOGLE_TEST_INCLUDE = $(GOOGLE_TEST_INSTALL)/usr/include

INCLUDES := -I../.. -I$(boost_libdir)/include -I$(GOOGLE_TEST_INCLUDE)
CXXFLAGS := -std=c++17 -Wno-reorder -O3
LD_FLAGS := -L../../lib -lsbgraph -L$(GOOGLE_TEST_INSTALL)/usr/lib -l$(GOOGLE_TEST_LIB) -l$(GOOGLE_MOCK_LIB)

# Source files.
PERF_SRC = main.cpp \
  set_perf.cpp \
  pw_map_perf.cpp \

create-folders::
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/$(BOOST_DIR)
	@mkdir -p $(BIN_DIR)

# Compilation
$(BUILD_DIR)/%.o : %.cpp | create-folders
	$(CC) $(INCLUDES) $(CXXFLAGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(CC) $(INCLUDES) -c $< -o $@ $(CXXFLAGS)

# Objects
PERF_OBJ=$(addprefix $(BUILD_DIR)/, $(PERF_SRC:.cpp=.o))

$(PERFORMANCE): $(PERF_OBJ)
	$(CXX) $(INCLUDES) -o $(PERFORMANCE) $(PERF_OBJ) $(CXXFLAGS) $(LD_FLAGS)

BOOST_SRC = $(BOOST_DIR)/main.cpp \
  $(BOOST_DIR)/ordinary_graph.cpp \
  $(BOOST_DIR)/ordinary_graph_builder.cpp

# Objects
BOOST_OBJ=$(addprefix $(BUILD_DIR)/, $(BOOST_SRC:.cpp=.o))

$(BOOST_PERF): $(BOOST_OBJ)
	$(CXX) $(INCLUDES) -o $(BOOST_PERF) $(BOOST_OBJ) $(CXXFLAGS) $(LD_FLAGS)

#///////////////////////////////////////////////////////////////////////////////
# Cleaning --------------------------------------------------------------------- 
#///////////////////////////////////////////////////////////////////////////////

RM = rm -rf

clean:
	$(RM) $(BIN_DIR) $(BUILD_DIR) 
