import sys
import re

def modify_c_file(file_path):
    with open(file_path, 'r') as f:
        content = f.read()
    # Replace #include <Python.h> with #include "micropython.h", accounting for spaces/tabs
    modified_content = re.sub(r'^(\s*#\s*include\s*)<Python\.h>', r'\1"micropython.h"', content, flags=re.MULTILINE)
    with open(file_path, 'w') as f:
        f.write(modified_content)

if __name__ == '__main__':
    for file_path in sys.argv[1:]:
        modify_c_file(file_path)