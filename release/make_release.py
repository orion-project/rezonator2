#!/usr/bin/env python3

import time

from helpers import *

printc('Build release version of the application', Colors.BOLD)

check_qt_path()
check_make_path()

navigate_to_project_dir()

create_dir_if_none(OUT_DIR)
os.chdir(OUT_DIR)

recreate_dir_if_exists(BUILD_DIR)
os.chdir(BUILD_DIR)

print_header('Building...')

start_time = time.time()

execute('qmake -config release ' + os.path.join('..', '..', PROJECT_FILE))
execute('mingw32-make' if IS_WINDOWS else 'make')

print('\nBuilt in {0:.2f} seconds\n'.format(time.time() - start_time))
