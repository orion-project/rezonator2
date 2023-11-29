from __future__ import print_function

import sys
print('Python ' + sys.version + '\n')

import os
import subprocess
import platform
import requests
import shutil
import struct
import errno
from zipfile import ZipFile, ZIP_DEFLATED

IS_WINDOWS = False
IS_LINUX = False
IS_MACOS = False
p = platform.system()
if p == 'Windows':
  IS_WINDOWS = True
elif p == 'Linux':
  IS_LINUX = True
elif p == 'Darwin':
  IS_MACOS = True
else:
  print('ERROR: Unknown platform {}'.format(p))
  exit()


PROJECT_NAME = 'rezonator'
PROJECT_FILE = 'rezonator.pro'
if IS_WINDOWS: PROJECT_EXE = 'rezonator.exe'
if IS_LINUX: PROJECT_EXE = 'rezonator'
if IS_MACOS: PROJECT_EXE = 'rezonator.app'
OUT_DIR = 'out'
BUILD_DIR = 'build'
REDIST_DIR = 'redist'


class Colors:
  HEADER = '\033[95m'
  OKBLUE = '\033[94m'
  OKGREEN = '\033[92m'
  WARNING = '\033[93m'
  FAIL = '\033[91m'
  ENDC = '\033[0m'
  BOLD = '\033[1m'
  UNDERLINE = '\033[4m'


def printc(txt, color):
  if IS_WINDOWS:
    print(txt)
  else:
    print(color + txt + Colors.ENDC)


def print_error_and_exit(txt):
  printc('ERROR: ' + txt, Colors.FAIL)
  exit()


def print_header(txt):
  print('')
  if IS_WINDOWS:
    print('***** ' + txt)
  else:
    printc(txt, Colors.HEADER)


def execute(cmd, print_stdout = True, return_stdout = False, check_return_code = True):
  # It works in Python 3.5 but Python 2.7 can't find command with parameters
  # given as a string (e.g.: 'qmake -v'), it should be splitted to array (['qmake', '-v'])
  p = subprocess.Popen(cmd.split(' '), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
  if return_stdout:
    stdout = []
  for line in iter(p.stdout.readline, ''):
    if return_stdout:
      stdout.append(line)
    if print_stdout:
      print(line, end='')
  p.stdout.close()
  return_code = p.wait()
  if return_code and check_return_code:
    raise subprocess.CalledProcessError(return_code, cmd)
  if return_stdout:
    return stdout


def navigate_to_project_dir():
  curdir = os.path.dirname(os.path.realpath(__file__))
  os.chdir(os.path.join(curdir, '..'))
  if not os.path.isfile(PROJECT_FILE):
    print_error_and_exit((
      'Project file {} not found in the current directory.\n' +
      'This script should be run from the project directory.'
    ).format(PROJECT_FILE))


def check_qt_path(cmd = 'qmake -v', print_stdout = True, check_return_code = True):
  print_header('Check if Qt is in PATH...')

  def get_qt_path_example():
    if IS_WINDOWS:
      return 'set PATH=c:\\Qt\\5.12.0\\mingw73_64\\bin;%PATH%'
    if IS_LINUX:
      return 'export PATH=/home/user/Qt/5.10.0/gcc_64/bin:$PATH'
    if IS_MACOS:
      return 'export PATH=/Users/user/Qt/5.10.0/clang_64/bin:$PATH'

  try:
    execute(cmd,
            print_stdout = print_stdout,
            check_return_code = check_return_code)
    print()
  except OSError as e:
    if e.errno == errno.ENOENT:
      printc('ERROR: qmake not found in PATH', Colors.FAIL)
      print('Find Qt installation and update your PATH like:')
      printc(get_qt_path_example(), Colors.BOLD)
      exit()
    else:
      raise e


def check_make_path():
  if not IS_WINDOWS:
    return
  try:
    execute('mingw32-make --version')
    print()
  except OSError as e:
    if e.errno == errno.ENOENT:
      printc('ERROR: mingw32-make not found in PATH', Colors.FAIL)
      print('Ensure you have MinGW installed and update PATH like:')
      printc('set PATH=c:\\Qt\\Tools\\mingw730_64\\bin;%PATH%', Colors.BOLD)
      exit()
    else:
      raise e


def create_dir_if_none(dirname):
  print_header('Create "{}" dir if none...'.format(dirname))
  if not os.path.exists(dirname):
    os.mkdir(dirname)
  else:
    print('Already there')


def recreate_dir_if_exists(dirname):
  print_header('Recreate "{}" directory...'.format(dirname))
  if os.path.exists(dirname):
    shutil.rmtree(dirname)
  os.mkdir(dirname)


def remove_files(filenames):
  for filename in filenames:
    if os.path.exists(filename):
      os.remove(filename)

def remove_files_in_dir(dir, filenames):
  for filename in filenames:
    filepath = os.path.join(dir, filename)
    if os.path.exists(filepath):
      os.remove(filepath)


def copy_file(source_file_path, target_dir, target_fn=''):
  (source_dir, filename) = os.path.split(source_file_path)
  target_file_path = os.path.join(target_dir, target_fn if target_fn else filename) 
  shutil.copyfile(source_file_path, target_file_path)


def copy_files(source_dir, filenames, target_dir):
  for filename in filenames:
    copy_file(os.path.join(source_dir, filename), target_dir)


def remove_dir(dirname):
  if os.path.exists(dirname):
    shutil.rmtree(dirname)


def get_file_text(file_name):
    with open(file_name, 'r') as f:
        return f.read()


def set_file_text(file_name, text):
    with open(file_name, 'w') as f:
        f.write(text)


def get_exe_bits(file_name):
  if IS_LINUX:
    exe_info = execute('file ' + file_name, print_stdout = False, return_stdout = True)
    exe_bits = exe_info[0].split(',')[1].strip()
    return exe_bits.split('-')[1]

  with open(file_name, 'rb') as f:
    if f.read(2) != b'MZ':
      raise Exception('Invalid exe file {}: invalid MZ signature'.format(file_name))
    f.seek(0x3A, 1) # seek to e_lfanew
    f.seek(struct.unpack('=L', f.read(4))[0], 0) # //seek to the start of the NT header.
    if f.read(4) != b'PE\0\0':
      raise Exception('Invalid exe file {}: invalid PE signature'.format(file_name))
    f.seek(20, 1) # seek past the file header
    arch = f.read(2)
    if arch ==  b'\x0b\x01':
      return 32
    elif arch == b'\x0b\x02':
      return 64
    else:
      raise Exception('Invalid exe file {}: unknown magic number {}'.format(file_name, str(arch)))


def find_qt_dir():
  paths = os.getenv('PATH', '').split(';' if IS_WINDOWS else ':')
  qmake = 'qmake.exe' if IS_WINDOWS else 'qmake'
  for path in paths:
    if os.path.exists(os.path.join(path, qmake)):
      return path
  return ''


def zip_dir(dir_name, zip_name):
  with ZipFile(zip_name, mode = 'w', compression = ZIP_DEFLATED) as z:
     for dirname, subdirs, filenames in os.walk(dir_name):
        for filename in filenames:
          z.write(os.path.join(dirname, filename))


def download_file(url, filename, mark_executable = False):
  if os.path.exists(filename):
    print('Already there')
    return

  r = requests.get(url + filename);
  with open(filename, 'wb') as f: f.write(r.content)
  if mark_executable:
    execute('chmod +x ' + filename)
