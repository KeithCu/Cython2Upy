# Define source files
CYTHON_FILES = my_list.pyx
C_FILES = main.c
C_FILES_GENERATED = $(CYTHON_FILES:.pyx=.c)
ALL_C_FILES = $(C_FILES_GENERATED) $(C_FILES)

# Compiler and linker settings (default to gcc, adjustable)
CC ?= gcc
CFLAGS ?= -O2

# Path configuration (use forward slashes, override with env var or cmdline)
MICROPYTHON_DIR ?= micropython  # Relative default, assumes subdirectory
MICROPYTHON_INCLUDE_DIR = $(MICROPYTHON_DIR)/include
MICROPYTHON_LIB_DIR = $(MICROPYTHON_DIR)/lib

# Consolidated compiler and linker flags
INCLUDES = -I. -I$(MICROPYTHON_INCLUDE_DIR)
LIBS = -L$(MICROPYTHON_LIB_DIR) -lmicropython

# File extensions and commands for portability
EXE_SUFFIX = $(if $(filter %.exe,$(wildcard $(CC)*)),.exe,)  # .exe on Windows if gcc produces it
RM = rm -f  # Default for Unix, override on Windows
TOUCH = touch  # Default for Unix, override below if needed

# Adjust for Windows if running in a non-POSIX environment
ifdef SystemRoot  # Detect Windows via cmd.exe environment variable
    RM = del /Q 2>nul
    TOUCH = type nul >  # Windows equivalent to touch
    MICROPYTHON_DIR ?= micropython  # Still relative, use backslashes if absolute
    INCLUDES := $(subst /,\,$(INCLUDES))  # Convert / to \ for Windows
    LIBS := $(subst /,\,$(LIBS))
endif

# Phony targets for convenience
.PHONY: all clean paths check_paths

# Default target
all: check_paths your_program$(EXE_SUFFIX)

# Rule to generate C files from Cython
$(C_FILES_GENERATED): %.c: %.pyx
	cython -o $@ $<

# Modify include directives in specific C files
.includes_modified: $(ALL_C_FILES)
	python modify_includes.py $(ALL_C_FILES)
	$(TOUCH) .includes_modified

# Compile C files to object files
%.o: %.c .includes_modified
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDES)

# Link everything into the final program
your_program$(EXE_SUFFIX): $(ALL_C_FILES:.c=.o)
	$(CC) -o $@ $^ $(LIBS)

# Clean up generated files
clean:
	-$(RM) $(C_FILES_GENERATED) *.o your_program$(EXE_SUFFIX) .includes_modified

# Print paths for debugging (use @echo for Make, not shell echo)
paths:
	@echo MICROPYTHON_DIR=$(MICROPYTHON_DIR)
	@echo MICROPYTHON_INCLUDE_DIR=$(MICROPYTHON_INCLUDE_DIR)
	@echo MICROPYTHON_LIB_DIR=$(MICROPYTHON_LIB_DIR)
	@echo INCLUDES=$(INCLUDES)
	@echo LIBS=$(LIBS)

# Validate paths using Make's built-in conditionals
check_paths:
ifeq ($(wildcard $(MICROPYTHON_INCLUDE_DIR)/micropython.h),)
	$(error "Error: MicroPython header not found at $(MICROPYTHON_INCLUDE_DIR)/micropython.h. Set MICROPYTHON_DIR correctly.")
endif
ifeq ($(wildcard $(MICROPYTHON_LIB_DIR)/libmicropython.a),)
	$(error "Error: MicroPython library not found at $(MICROPYTHON_LIB_DIR)/libmicropython.a. Set MICROPYTHON_DIR correctly.")
endif