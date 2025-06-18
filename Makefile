# LGMalloc Library Makefile
# Requires: Unix/POSIX system, ELF format, GCC and Clang compilers

SHELL				:= /bin/sh
.SUFFIXES:
.NOTPARALLEL:

# Library name
LIB_NAME			:= lgmalloc

# Directory structure
SRC_DIR				:= src
BIN_DIR				:= bin
OBJ_DIR				:= obj
API_DIR				:= $(SRC_DIR)/api
DEBUG_BIN_DIR		:= $(BIN_DIR)/debug
RELEASE_BIN_DIR		:= $(BIN_DIR)/release
DEBUG_OBJ_DIR		:= $(OBJ_DIR)/debug
RELEASE_OBJ_DIR		:= $(OBJ_DIR)/release

# Installation directories
PREFIX				?= /usr/local
LIBDIR				:= $(PREFIX)/lib
INCDIR				:= $(PREFIX)/include/$(LIB_NAME)

# Source files and headers are explicitly hardcoded rather
# than dynamically discovered to mitigate supply chain
# injection vulnerabilities

# Source files
SOURCES				:= src/heap.c		\
					   src/heuristics.c	\
					   src/init.c		\
					   src/lgcalloc.c	\
					   src/lgfree.c		\
					   src/lgmalloc.c	\
					   src/lgrealloc.c	\
					   src/profiling.c	\
					   src/tests.c

HEADERS				:= src/api/lgmalloc_cpp_operators.hpp		\
					   src/api/lgmalloc_profiling.h				\
					   src/api/lgmalloc.h						\
					   src/internal/lgmalloc_config.h			\
					   src/internal/lgmalloc_decommit.h			\
					   src/internal/lgmalloc_features.h			\
					   src/internal/lgmalloc_global_include.h	\
					   src/internal/lgmalloc_heuristics.h		\
					   src/internal/lgmalloc_impl.h				\
					   src/internal/lgmalloc_platform_guard.h	\
					   src/internal/lgmalloc_size_classes.h		\
					   src/internal/lgmalloc_thread_ctx.h		\
					   src/internal/lgmalloc_types.h

# Build configuration macros (only override if explicitly set)
CONFIG_FLAGS		:=
ifdef LGMALLOC_MMAP_THRESHOLD
CONFIG_FLAGS		+= -DLGMALLOC_MMAP_THRESHOLD=$(LGMALLOC_MMAP_THRESHOLD)
endif
ifdef LGMALLOC_ENABLE_DECOMMIT
CONFIG_FLAGS		+= -DLGMALLOC_ENABLE_DECOMMIT=$(LGMALLOC_ENABLE_DECOMMIT)
endif
ifdef LGMALLOC_DEBUG_LEVEL
CONFIG_FLAGS		+= -DLGMALLOC_DEBUG_LEVEL=$(LGMALLOC_DEBUG_LEVEL)
endif
ifdef LGMALLOC_MAX_ALLOC_SIZE
CONFIG_FLAGS		+= -DLGMALLOC_MAX_ALLOC_SIZE=$(LGMALLOC_MAX_ALLOC_SIZE)
endif

# Common compiler flags
COMMON_FLAGS		:= -std=gnu17				\
					   -fPIC					\
					   -Wall					\
					   -Wextra					\
					   -Werror					\
					   -Wstrict-prototypes		\
					   -Wmissing-prototypes		\
					   -Wpedantic				\
					   -fno-common				\
					   -Wconversion				\
					   -Wshadow					\
					   -Wcast-align				\
					   -Wunused					\
					   -Wpointer-arith			\
					   -Wcast-qual				\
					   -Wmissing-braces			\
					   -Wmissing-declarations	\
					   -Wwrite-strings			\
					   -Wbad-function-cast		\
					   -Wredundant-decls		\
					   -Wformat					\
					   -Wformat-security

# Debug build configuration
DEBUG_CC			:= clang
DEBUG_FLAGS			:= $(COMMON_FLAGS)			\
					   $(CONFIG_FLAGS)			\
					   -D_DEBUG					\
					   -Og						\
					   -g						\
					   -fstack-protector-all	\
					   -D_FORTIFY_SOURCE=2		\
					   -fstack-clash-protection	\
					   -Wtrampolines			\
					   -MMD						\
					   -MP

# Release build configuration  
RELEASE_CC			:= gcc
RELEASE_FLAGS		:= $(COMMON_FLAGS)	\
					   $(CONFIG_FLAGS)	\
					   -DNDEBUG			\
					   -Ofast			\
					   -flto=full		\
					   -Wlogical-op		\
					   -MMD				\
					   -MP
RELEASE_LDFLAGS		:= -flto=full

# Linker flags for malloc replacement
MALLOC_REPLACE_FLAGS	:= -Wl,--wrap=malloc,--wrap=free,--wrap=realloc,--wrap=calloc

# Object files
DEBUG_OBJECTS		:= $(SOURCES:$(SRC_DIR)/%.c=$(DEBUG_OBJ_DIR)/%.o)
RELEASE_OBJECTS		:= $(SOURCES:$(SRC_DIR)/%.c=$(RELEASE_OBJ_DIR)/%.o)

# Dependency files
DEBUG_DEPS			:= $(DEBUG_OBJECTS:.o=.d)
RELEASE_DEPS		:= $(RELEASE_OBJECTS:.o=.d)

# Library targets
DEBUG_STATIC		:= $(DEBUG_BIN_DIR)/lib$(LIB_NAME).a
DEBUG_SHARED		:= $(DEBUG_BIN_DIR)/lib$(LIB_NAME).so
RELEASE_STATIC		:= $(RELEASE_BIN_DIR)/lib$(LIB_NAME).a
RELEASE_SHARED		:= $(RELEASE_BIN_DIR)/lib$(LIB_NAME).so

# Platform verification
UNAME_S				:= $(shell uname -s 2>/dev/null || echo "Unknown")
ifeq ($(UNAME_S),Unknown)
$(error Unable to detect Unix/POSIX system)
endif

# ELF format verification
ELF_CHECK			:= $(shell readelf -h /bin/sh >/dev/null 2>&1 && echo "yes" || echo "no")
ifneq ($(ELF_CHECK),yes)
$(error Target system must support ELF format)
endif

# Compiler verification
DEBUG_CC_CHECK		:= $(shell which $(DEBUG_CC) >/dev/null 2>&1 && echo "yes" || echo "no")
RELEASE_CC_CHECK	:= $(shell which $(RELEASE_CC) >/dev/null 2>&1 && echo "yes" || echo "no")

ifneq ($(DEBUG_CC_CHECK),yes)
$(error $(DEBUG_CC) compiler not found or not installed)
endif

ifneq ($(RELEASE_CC_CHECK),yes)  
$(error $(RELEASE_CC) compiler not found or not installed)
endif

# Default target
.PHONY: all release debug clean fclean re install tests help
all: release

# Release build target
release:
	@echo "Building release libraries..."
	@start_time=$$(date +%s);					\
	$(MAKE) release_build;						\
	end_time=$$(date +%s);						\
	elapsed_time=$$((end_time - start_time));	\
	echo "Release build completed in $$elapsed_time seconds."

# Debug build target  
debug:
	@echo "Building debug libraries..."
	@start_time=$$(date +%s);					\
	$(MAKE) debug_build;						\
	end_time=$$(date +%s);						\
	elapsed_time=$$((end_time - start_time));	\
	echo "Debug build completed in $$elapsed_time seconds."

# Actual build targets
.PHONY: release_build debug_build
release_build: $(RELEASE_STATIC) $(RELEASE_SHARED)
debug_build: $(DEBUG_STATIC) $(DEBUG_SHARED)

# Release static library
$(RELEASE_STATIC): $(RELEASE_OBJECTS) | $(RELEASE_BIN_DIR)
	@echo "Creating release static library: $@"
	@ar rcs $@ $(RELEASE_OBJECTS)

# Release shared library
$(RELEASE_SHARED): $(RELEASE_OBJECTS) | $(RELEASE_BIN_DIR)
	@echo "Creating release shared library: $@"
	@$(RELEASE_CC) -shared $(RELEASE_LDFLAGS) -o $@ $(RELEASE_OBJECTS)

# Debug static library
$(DEBUG_STATIC): $(DEBUG_OBJECTS) | $(DEBUG_BIN_DIR)
	@echo "Creating debug static library: $@"
	@ar rcs $@ $(DEBUG_OBJECTS)

# Debug shared library  
$(DEBUG_SHARED): $(DEBUG_OBJECTS) | $(DEBUG_BIN_DIR)
	@echo "Creating debug shared library: $@"
	@$(DEBUG_CC) -shared -o $@ $(DEBUG_OBJECTS)

# Release object compilation
$(RELEASE_OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(RELEASE_OBJ_DIR)
	@echo "Compiling release object: $<"
	@$(RELEASE_CC) $(RELEASE_FLAGS) -c $< -o $@

# Debug object compilation
$(DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(DEBUG_OBJ_DIR)
	@echo "Compiling debug object: $<"
	@$(DEBUG_CC) $(DEBUG_FLAGS) -c $< -o $@

# Directory creation
$(DEBUG_BIN_DIR):
	@echo "Creating debug binary directory"
	@mkdir -p $(DEBUG_BIN_DIR)

$(RELEASE_BIN_DIR):
	@echo "Creating release binary directory"  
	@mkdir -p $(RELEASE_BIN_DIR)

$(DEBUG_OBJ_DIR):
	@echo "Creating debug object directory"
	@mkdir -p $(DEBUG_OBJ_DIR)

$(RELEASE_OBJ_DIR):
	@echo "Creating release object directory"
	@mkdir -p $(RELEASE_OBJ_DIR)

# Clean targets
clean:
	@echo "Cleaning object files and dependencies"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Cleaning all build artifacts"
	@rm -rf $(BIN_DIR)

re: fclean all

# Installation target
install: release
	@echo "Installing liblgmalloc to system directories"
	@echo "Installing libraries to $(LIBDIR)"
	@install -d $(LIBDIR)
	@install -m 644 $(RELEASE_STATIC) $(LIBDIR)/
	@install -m 644 $(RELEASE_SHARED) $(LIBDIR)/
	@echo "Installing headers to $(INCDIR)"
	@install -d $(INCDIR)
	@install -m 644 $(API_DIR)/*.h $(INCDIR)/ 2>/dev/null || true
	@install -m 644 $(API_DIR)/*.hpp $(INCDIR)/ 2>/dev/null || true
	@echo "Running ldconfig to update library cache"
	@ldconfig
	@echo "Installation completed successfully"

# Tests target
tests:
	@echo "Running test suite"
	@$(MAKE) -C tests

# Help target
help:
	@echo "LGMalloc Library Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build release libraries (default)"
	@echo "  release   - Build optimized release libraries"
	@echo "  debug     - Build debug libraries with symbols"
	@echo "  clean     - Remove object files and dependencies"
	@echo "  fclean    - Remove all build artifacts"
	@echo "  re        - Clean and rebuild"
	@echo "  install   - Install libraries and headers to system"
	@echo "  tests     - Run test suite"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Configuration options (override defaults via make variables):"
	@echo "  LGMALLOC_MMAP_THRESHOLD    - Memory threshold for mmap usage"
	@echo "  LGMALLOC_ENABLE_DECOMMIT   - Enable memory decommit (0/1)"
	@echo "  LGMALLOC_DEBUG_LEVEL       - Debug verbosity level"
	@echo "  LGMALLOC_MAX_ALLOC_SIZE    - Maximum allocation size"
	@echo ""
	@echo "Example: make LGMALLOC_MMAP_THRESHOLD=1048576 LGMALLOC_DEBUG_LEVEL=2 release"
	@echo ""
	@echo "Usage with LD_PRELOAD:"
	@echo "  LD_PRELOAD=$(LIBDIR)/lib$(LIB_NAME).so your_program"

# Include dependency files
-include $(DEBUG_DEPS) $(RELEASE_DEPS)
