################################################################################
# GLOBAL CONFIGURATION
################################################################################
# Be silent by default, but 'make V=1' will show all compiler calls
ifneq ($(V),1)
MAKEFLAGS += --no-print-directory
MAKEFLAGS += -s
endif

# Enable this flag on a fresh build if generating coverage reports
ifeq ($(E),1)
CFLAGS += -g -O0 -fprofile-arcs -ftest-coverage
LDFLAGS += -ftest-coverage -lgcov
endif

################################################################################
# TOOLS
################################################################################

MAKE   := make
CPP    := g++

# Flags always used while compiling and linking
CFLAGS += -std=c++11 -Wall -Werror -I$(CRYPTOPP_DIR)
LDFLAGS := $(CFLAGS) -L$(CRYPTOPP_DIR)\cryptopp -lcryptopp

# Additional flags used for unit tests
UT_CFLAGS  := $(CFLAGS)
UT_LDFLAGS := $(LDFLAGS) -lcheck

################################################################################
# DIRECTORIES
################################################################################

# Main directories
BASE_DIR := $(CURDIR)
SRC      := $(BASE_DIR)/src
OBJ      := $(BASE_DIR)/obj
BIN      := $(BASE_DIR)/bin
DOC      := $(BASE_DIR)/doc

# Unit test directories
UT_SRC := $(SRC)/test
UT_OBJ := $(OBJ)/test
UT_BIN := $(BIN)/test

INCLUDES := -I$(SRC)
CFLAGS += $(INCLUDES)

################################################################################
# FILES
################################################################################
SOURCE             := $(wildcard $(SRC)/*.cpp)
OBJECTS            := $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SOURCE))
OUTPUT_BIN         := encrypt_cyacd

UT_SOURCE          := $(wildcard $(UT_SRC)/*.cpp)
UT_OBJECTS         := $(patsubst $(UT_SRC)/%.cpp,$(UT_OBJ)/%.o,$(UT_SOURCE))
UT_BINS            := $(patsubst $(UT_SRC)/%.cpp,$(UT_BIN)/%,$(UT_SOURCE))

################################################################################
# RULES
################################################################################
# Dependency files
-include $(OBJECTS:.o=.d)

# Turn off implicit rules to avoid conflict
.SUFFIXES:

# Compilation
.DEFAULT_GOAL :=
.PHONY: all clean tests coverage_report application

# People like to type 'make all', and tests/reports will be broken for most,
# so the make all target just builds the application
all: application
	@echo Done

application: $(OUTPUT_BIN)

tests: $(UT_BINS)
	@echo Done

coverage_report:
	@echo GENERATE : Running unit tests for coverage report
	./run_tests.sh
	@echo GENERATE : Test coverage report
	lcov -d . -c -o $(DOC)/lcov/cover.info --quiet
	lcov -r $(DOC)/lcov/cover.info '/usr*' 'test*' -o $(DOC)/lcov/filtered.info --quiet
	genhtml $(DOC)/lcov/filtered.info -o $(DOC)/lcov/coverage --quiet

$(OBJECTS): $(OBJ)/%.o: $(SRC)/%.cpp
	@echo Compiling: $@
	$(CPP) -c $(CFLAGS) -o $@ $<
	$(CPP) -MM -MT $@ $(CFLAGS) $< > $(OBJ)/$*.d

$(UT_OBJECTS): $(UT_OBJ)/%.o: $(UT_SRC)/%.cpp
	@echo Compiling: $@
	$(CPP) -c $(CFLAGS) -o $@ $<
	$(CPP) -MM -MT $@ $(CFLAGS) $< > $(UT_OBJ)/$*.d

$(OUTPUT_BIN): main.cpp $(OBJECTS)
	@echo Linking: $@
	$(CPP) -o $@ $^ $(LDFLAGS) -I./src

$(UT_BINS): $(OBJECTS) $(UT_OBJECTS)
	@echo Linking: $@
	$(CPP) -o $@ $(patsubst $(UT_BIN)%,$(UT_OBJ)%.o,$@) $(OBJECTS) $(UT_LDFLAGS)

clean::
	rm -f $(SRC)/*.gch
	@echo REMOVING: $(SRC)/*.gch
	rm -f $(OBJ)/*.o
	@echo REMOVING: $(OBJ)/*.o
	rm -f $(OBJ)/*.d
	@echo REMOVING: $(OBJ)/*.d
	rm -f $(UT_OBJ)/*.o
	@echo REMOVING: $(UT_OBJ)/*.o
	rm -f $(UT_OBJ)/*.d
	@echo REMOVING: $(UT_OBJ)/*.d
	rm -f $(BIN)/*.a
	@echo REMOVING: $(BIN)/*.a
	rm -f $(OUTPUT_BIN)
	@echo REMOVING: $(OUTPUT_BIN)
	rm -f $(UT_BIN)/*
	@echo REMOVING: $(UT_BIN)/*
	rm -rf $(DOC)/lcov/*
	rm -rf $(OBJ)/*.gcda
	rm -rf $(OBJ)/*.gcno
	rm -rf $(UT_OBJ)/*.gcda
	rm -rf $(UT_OBJ)/*.gcno
	@echo REMOVING: Coverage reports
	@echo Done.
