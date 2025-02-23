CYTHON_FILES = my_list.pyx
C_FILES = main.c
C_FILES_GENERATED = $(CYTHON_FILES:.pyx=.c)
ALL_C_FILES = $(C_FILES_GENERATED) $(C_FILES)

# Rule to generate C files from Cython
$(C_FILES_GENERATED): %.c: %.pyx
	cython -o $@ $<

.includes_modified: $(ALL_C_FILES)
	python modify_includes.py $(ALL_C_FILES)
	touch .includes_modified

%.o: %.c .includes_modified
	gcc -c -o $@ $< -I. -I$(MICROPYTHON_INCLUDE_DIR)

your_program: $(ALL_C_FILES:.c=.o)
	gcc -o $@ $^ -L$(MICROPYTHON_LIB_DIR) -lmicropython