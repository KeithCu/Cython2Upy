# Define the Cython files and additional C files in your build
CYTHON_FILES = mycythoncode.pyx
C_FILES = main.c  # List additional C files here
C_FILES_GENERATED = $(CYTHON_FILES:.pyx=.c)
ALL_C_FILES = $(C_FILES_GENERATED) $(C_FILES)

# Rule to generate C files from Cython
$(C_FILES_GENERATED): %.c: %.pyx
    cython -o $@ $<

# Target to modify includes for specific C files
.includes_modified: $(ALL_C_FILES)
    python modify_includes.py $(ALL_C_FILES)
    touch .includes_modified

# Compilation rule: depends on the C file and the modification stamp
%.o: %.c .includes_modified
    gcc -c -o $@ $< -I. -I$(MICROPYTHON_INCLUDE_DIR)

# Linking rule to create the final program
your_program: $(ALL_C_FILES:.c=.o)
    gcc -o $@ $^ -L$(MICROPYTHON_LIB_DIR) -lmicropython