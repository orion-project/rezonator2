import os

from helpers import *

# Python modules to exclude from standard library (not needed for math calculations)
PYTHON_EXCLUDE_PATTERNS = (
  '__pycache__',     # Bytecode cache
  '*.pyc',           # Compiled Python files
  '__phello__',      # Test module
  'test',            # Test suite
  'tests',           # Additional tests
  # Dynamic libraries (*.so files) from lib-dynload
  '_tkinter*.so',          # GUI support
  '_ctypes*.so',           # Foreign function interface
  '_sqlite3*.so',          # Database
  '_ssl*.so',              # Network security
  '_socket*.so',           # Network
  '_bz2*.so',              # Compression
  '_lzma*.so',             # Compression
  '_multiprocessing*.so',   # Parallel processing
  '_posixshmem*.so',       # POSIX shared memory
  '_posixsubprocess*.so',   # POSIX subprocess
  '_queue*.so',            # Multiprocessing queue
  '_testcapi*.so',         # Testing
  '_xxtestfuzz*.so',       # Testing
  '_testimportmultiple*.so', # Testing
  '_testbuffer*.so',       # Testing
  '_testinternalcapi*.so', # Testing
  '_testclinic*.so',       # Testing
  '_curses*.so',           # Terminal UI
  '_dbm*.so',              # Database
  '_gdbm*.so',             # Database
  'audioop*.so',           # Audio processing
  'ossaudiodev*.so',       # Audio
  'spwd*.so',              # Password database
  'syslog*.so',            # System logging
  'termios*.so',           # Terminal I/O
  '_uuid*.so',             # UUID generation
  '_codecs_*.so',          # Additional codecs
  '_multibytecodec*.so',   # Multibyte codecs
  'xxlimited*.so',         # Example module
  'readline*.so',          # Command line editing
  '_xx*.so',
  '_asyncio*.so',
  #'_blake2*.so',           # Cryptographic hashing (used by _hashlib)
  'pyexpat*.so',          # XML parsing
  'resource*.so',         # System resources
  'select*.so',           # I/O multiplexing
  '_contextvars*.so',     # Async context variables
  '_crypt*.so',           # Password encryption
  '_elementtree*.so',     # XML processing
  '_lsprof*.so',          # Profiler
  '_opcode*.so',          # Bytecode operations
  'fcntl*.so',           # File control
  'grp*.so',             # Unix groups
  'mmap*.so',            # Memory mapping
  #'_hashlib*.so',        # Cryptographic hashing (used by random and tempfile)
  '_zoneinfo*.so',       # Time zone database
  '_sha*.so',           # SHA hash algorithms
  '_md5*.so',           # MD5 hash algorithm
  # GUI libraries
  'tkinter', 'turtledemo', 'idlelib', 'turtle.py',
  # Development/conversion tools
  'lib2to3', 'unittest', 'pydoc_data', 'distutils', 'ensurepip', 'venv',
  # Debugging/profiling
  'pdb.py', 'profile.py', 'cProfile.py', 'pstats.py', 'trace.py', 
  'timeit.py', 'tabnanny.py', 'symbol.py', 'pickletools.py',
  # Async/concurrency
  'asyncio', 'multiprocessing', 'concurrent', 'queue.py',
  # Network protocols
  'xmlrpc', 'wsgiref', 'email', 'html', 'http', 'urllib',
  'webbrowser.py', 'socketserver.py', 'ftplib.py', 'poplib.py', 
  'smtplib.py', 'telnetlib.py', 'imaplib.py', 'nntplib.py',
  'cgi.py', 'cgitb.py', 'socket.py', 'ssl.py', 'ipaddress.py',
  # Database
  'sqlite3', 'dbm',
  # Audio/Video
  'wave.py', 'audioop.py', 'sndhdr.py', 'sunau.py', 'aifc.py', 
  'chunk.py', 'colorsys.py',
  # File formats
  'tomllib', 'plistlib.py', 'netrc.py', 'mailbox.py', 'mailcap.py',
  'mimetypes.py', 'uu.py', 'binhex.py', 'quopri.py', 'base64.py',
  # XML processing
  'xml',
  # Terminal/system
  'ctypes', 'curses', 'cmd.py', 'readline.py', 'rlcompleter.py',
  'getpass.py', 'getopt.py', 'pty.py', 'tty.py', 'pipes.py', 'sched.py',
  # System utilities
  'importlib', 'logging', 'msilib', 'zoneinfo',
  # Compression & Archives
  'zipfile', 'tarfile.py', 'gzip.py', 'bz2.py', 'lzma.py', 'zipapp.py',
  # Command-line & Argument Parsing
  'argparse.py', 'optparse.py',
  # Compilation & Bytecode
  'py_compile.py', 'compileall.py',
  # File Operations
  'glob.py', 'pathlib.py', 'filecmp.py', 'fileinput.py',
  # Security & Cryptography
  'secrets.py', 'hmac.py', 'crypt.py',
  # Development & Debugging
  'doctest.py', 'pprint.py', 'inspect.py', 'dis.py', 
  'ast.py', 'code.py', 'codeop.py', 'pydoc.py', 'pyclbr.py',
  # Internationalization
  'locale.py', 'gettext.py',
  # System & Process
  'selectors.py',
  # Code Analysis
  'modulefinder.py', 'symtable.py',
  # Text Processing
  'stringprep.py', 'difflib.py',
  # Miscellaneous
  'calendar.py', 'antigravity.py', 'this.py', '__hello__.py',
  'bdb.py', 'graphlib.py', 'imghdr.py', 'runpy.py', 'shlex.py',
  'tracemalloc.py', 'xdrlib.py'
)

def prepare_python_lib(bin_dir):
    print_header('Copy Python files...')
    if IS_WINDOWS:
        lib_dir = 'python\\Lib'
        src_dir = 'vcpkg_installed/x64-windows/tools/python3/Lib'
    elif IS_LINUX:
        lib_dir = 'lib/python3.12'
        src_dir = 'vcpkg_installed/x64-linux/lib/python3.12'
    elif IS_MACOS:
        lib_dir = 'rezonator.app/Contents/MacOS/lib/python3.12'
        src_dir = 'vcpkg_installed/x64-osx/lib/python3.12'
    else:
        raise NotImplementedError('OS is not supported')
    bin_dir = os.path.abspath(bin_dir)
    src_dir = os.path.join(get_project_dir(), src_dir)
    lib_dir = os.path.join(bin_dir, lib_dir)
    if os.path.exists(lib_dir):
        print(f'Target directory is already exists: {lib_dir}')
        return
    print(f'Source dir: {src_dir}')
    print(f'Target dir: {lib_dir}')
    shutil.copytree(src_dir, lib_dir, ignore=shutil.ignore_patterns(*PYTHON_EXCLUDE_PATTERNS))
    if IS_WINDOWS:
        print('Pack Python files...')
        shutil.make_archive(bin_dir + '\\python312', 'zip', lib_dir)
        shutil.rmtree(bin_dir + '\\python')

if __name__ == '__main__':
    navigate_to_project_dir()
    prepare_python_lib('./bin')
