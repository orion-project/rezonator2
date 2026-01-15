#!/usr/bin/env python3

import glob
from helpers import *

navigate_to_project_dir()

version_str = get_file_text(os.path.join('release', 'version.txt'))
printc('Create redistributable package version {}'.format(version_str), Colors.BOLD)


# when run with -v, *deployqt returns 1 and prints long help message,
# so don't print stdout and don't check return code
if IS_WINDOWS:
  check_qt_path(cmd = 'windeployqt -v', print_stdout = False, check_return_code = False)
if IS_MACOS:
  check_qt_path(cmd = 'macdeployqt -v', print_stdout = False, check_return_code = False)
if IS_LINUX:
  check_qt_path()


create_dir_if_none(OUT_DIR)
os.chdir(OUT_DIR)

recreate_dir_if_exists(REDIST_DIR)
os.chdir(REDIST_DIR)

package_name = PROJECT_NAME + '-' + version_str

# Python modules to exclude from standard library (not needed for math calculations)
PYTHON_EXCLUDE_PATTERNS = (
  '__pycache__',     # Bytecode cache
  '*.pyc',           # Compiled Python files
  '__phello__',      # Test module
  'test',            # Test suite
  'tests',           # Additional tests
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

########################################################################
#                             Windows

def make_package_for_windows():
  qt_dir = find_qt_dir()

  print_header('Run windeployqt...')
  execute(f'windeployqt ..\\..\\bin\\{PROJECT_EXE} --dir . ' +
          '--no-translations --no-system-d3d-compiler --no-opengl-sw')
  execute(f'windeployqt {qt_dir + "\\assistant.exe"} --dir . ' +
          '--no-translations --no-system-d3d-compiler --no-opengl-sw')

  print_header('Clean some excessive files...')
  remove_files(['libEGL.dll', 'libGLESV2.dll', 'vc_redist.x64.exe'])
  remove_files_in_dir('sqldrivers', ['qsqlmysql.dll', 'qsqlodbc.dll', 'qsqlpsql.dll'])
  remove_files_in_dir('imageformats', ['qicns.dll', 'qtga.dll', 'qtiff.dll', 'qwbmp.dll', 'qwebp.dll'])

  print_header('Copy project files...')
  copy_files('..\\..\\bin', [PROJECT_EXE, 'rezonator.qch', 'rezonator.qhc', 'lua.dll', \
                             'python312.dll', 'zlib1.dll'], '.')
  shutil.copytree('..\\..\\bin\\examples', 'examples')
  copy_files(qt_dir, ['assistant.exe'], '.')
  
  print_header('Copy Python files...')
  python_src = '..\\..\\vcpkg_installed\\x64-windows\\tools\\python3'
  shutil.copytree(python_src + '\\Lib', 'python\\Lib', ignore=shutil.ignore_patterns(*PYTHON_EXCLUDE_PATTERNS))
  shutil.make_archive('python312', 'zip', 'python\\Lib')
  # copy_files(python_src + '\\DLLs', [], 'python\\DLLs', skip_non_exitent=True, create_dir=True)
  shutil.rmtree('python')
  
  # It seems there is some bug in windeployqt.exe on Qt 5.15.2
  # (or something changed in the env becasue it worked on another machine before)
  # it doesn't copy libwinpthread and copies other libs from some wrong place, so the app crashes
  # Copy them manually, just to be sure
  copy_files(qt_dir, ['libstdc++-6.dll', 'libgcc_s_dw2-1.dll', 'libwinpthread-1.dll'], '.', skip_non_exitent=True)

  print_header('Pack files to zip...')
  global package_name
  package_name = '{}-win-x{}.zip'.format(package_name, get_exe_bits(PROJECT_EXE))
  zip_dir('.', '..\\' + package_name)


########################################################################
#                           Linux

def make_package_for_linux():
  print_header('Create AppDir structure...')
  os.makedirs('usr/bin')
  os.makedirs('usr/lib')
  os.makedirs('usr/share/applications')
  os.makedirs('usr/share/icons/hicolor/256x256/apps')

  print_header('Copy project files...')
  copy_file('../../bin/' + PROJECT_EXE, 'usr/bin')
  copy_file(find_qt_dir() + '/assistant', 'usr/bin')
  copy_file('../../bin/rezonator.qch', 'usr/bin')
  copy_file('../../bin/rezonator.qhc', 'usr/bin')
  shutil.copytree('../../bin/examples', 'usr/bin/examples')
  copy_file(f'../../release/{PROJECT_NAME}.desktop', 'usr/share/applications')
  shutil.copyfile('../../img/icon/main_2_256.png', f'usr/share/icons/hicolor/256x256/apps/{PROJECT_NAME}.png')
  
  print_header('Copy Python files...')
  python_src = '../../vcpkg_installed/x64-linux'
  python_dst = 'usr/bin/python'
  python_lib = python_dst + '/Lib'
  shutil.copytree(python_src + '/lib/python3.12', python_lib, ignore=shutil.ignore_patterns(*PYTHON_EXCLUDE_PATTERNS))
  #shutil.make_archive(python_dst + '/python312', 'zip', python_lib)
  #shutil.rmtree(python_lib)

  # There will be error 'Could not determine the path to the executable' otherwise
  execute('chmod +x usr/bin/' + PROJECT_EXE)
  execute('chmod +x usr/bin/assistant') # just in case

  os.chdir('..')

  print_header('Download linuxdeployqt...')
  # Download linuxdeplyqt if none (https://github.com/probonopd/linuxdeployqt)
  # NOTE: It've broken compatibility with newer OS versions forsing to stick at Ubuntu 14 LTS.
  # See discussion here: https://github.com/probonopd/linuxdeployqt/issues/340
  # But I have nor a machine running Trusty or a wish to stick at Qt 5.5
  # (the last supported for Trusty) so have to use a more relaxed 5th version of the tool.
  linuxdeployqt = 'linuxdeployqt-5-x86_64.AppImage'
  linuxdeployqt_url = 'https://github.com/probonopd/linuxdeployqt/releases/download/5/'
  download_file(linuxdeployqt_url, linuxdeployqt, mark_executable = True)

  print_header('Create AppImage...')
  execute(f'./{linuxdeployqt} {REDIST_DIR}/usr/bin/assistant ' +
    '-no-translations -no-copy-copyright-files ' +
    '-exclude-libs=libqsqlmysql,libqsqlpsql,libqsqlodbc,libqicns,libqico,libqtga,libqtiff,libqwbmp,libqwebp')
  execute(f'./{linuxdeployqt} {REDIST_DIR}/usr/share/applications/{PROJECT_NAME}.desktop ' +
    '-appimage -no-translations -no-copy-copyright-files ' +
    '-extra-plugins=iconengines,imageformats/libqsvg.so ' +
    '-exclude-libs=libqsqlmysql,libqsqlpsql,libqsqlodbc,libqicns,libqico,libqtga,libqtiff,libqwbmp,libqwebp')

  # Seems we can't specify target AppImage name, so find it
  default_appimage_names = glob.glob(PROJECT_NAME + '-x*.AppImage')
  if len(default_appimage_names) != 1:
    print_error_and_exit('Unable to find created AppImage file')

  global package_name
  package_name = '{}-x86_{}.AppImage'.format(package_name, get_exe_bits('../bin/' + PROJECT_EXE))
  remove_files([package_name])
  os.rename(default_appimage_names[0], package_name)


########################################################################
#                              macOS

def make_package_for_macos():
  print_header('Copy application bundle...')
  remove_dir('image')
  image_exe = 'image/' + PROJECT_EXE
  
  # Assitant, QCH and QHC help files are already in the bundle dir
  # This is done during help compilation (see help/make.sh)
  shutil.copytree('../../bin/' + PROJECT_EXE, image_exe)

  print_header('Run macdeployqt...')
  execute('macdeployqt ' + image_exe)

  print_header('Copy project files...')
  shutil.copytree('../../bin/examples', image_exe + '/Contents/MacOS/examples')
  
  print_header('Copy Python files...')
  python_src = '../../vcpkg_installed/x64-osx'
  python_dst = image_exe + '/Contents/MacOS/python'
  python_lib = python_dst + '/Lib'
  shutil.copytree(python_src + '/lib/python3.12', python_lib, ignore=shutil.ignore_patterns(*PYTHON_EXCLUDE_PATTERNS))
  #shutil.make_archive(python_dst + '/python312', 'zip', python_lib)
  #shutil.rmtree(python_lib)

  print_header('Processing Assistant...')
  shutil.copytree(find_qt_dir() + '/Assistant.app', 'Assistant.app')
  execute('macdeployqt Assistant.app -appstore-compliant')
  print_header('Patching Assistant...')
  # `macdeployqt` (at least in Qt 5.10) doesn't add this RPATH to assistant exe, 
  # and as a result when packed to rezonator.app bundle it can't find libraries
  execute('install_name_tool -add_rpath @loader_path/../Frameworks Assistant.app/Contents/MacOS/Assistant')
  execute('install_name_tool -delete_rpath @loader_path/../../../../lib Assistant.app/Contents/MacOS/Assistant')
  print_header('Copy Assistant files to bundle...')
  copy_file('Assistant.app/Contents/MacOS/Assistant', image_exe + '/Contents/MacOS', target_fn='assistant')
  copy_dir('Assistant.app/Contents/Frameworks/QtHelp.framework', image_exe + '/Contents/Frameworks/QtHelp.framework')
  copy_dir('Assistant.app/Contents/Frameworks/QtSql.framework', image_exe + '/Contents/Frameworks/QtSql.framework')
  make_dir(image_exe + '/Contents/PlugIns/sqldrivers')
  copy_file('Assistant.app/Contents/PlugIns/sqldrivers/libqsqlite.dylib', image_exe + '/Contents/PlugIns/sqldrivers')

  print_header('Clean some excessive files...')
  remove_files_in_dir(image_exe + '/Contents/PlugIns/sqldrivers', [
    'libqsqlmysql.dylib', 'libqsqlpsql.dylib'])
  remove_files_in_dir(image_exe + '/Contents/PlugIns/imageformats', [
    'libqico.dylib', 'libqtga.dylib', 'libqtiff.dylib', 'libqwbmp.dylib', 'libqwebp.dylib'])

  print_header('Pack application bundle to dmg...')
  os.chdir('..')
  installer_img = 'installer.dmg'
  installer_volume = f'/Volumes/{PROJECT_NAME}'
  if not os.path.isfile(installer_img):
    print('Installer image does not exist, creating...')
    # Can't use package_name as volname because then it needs to be recreated and adjusted every release
    execute(f'hdiutil create {installer_img} -size 200m -format UDRW -ov -volname {PROJECT_NAME} -fs HFS+ -srcfolder {REDIST_DIR}/image')
    execute(f'hdiutil attach {installer_img}')
    execute('sleep 1')
    execute(f'open {installer_volume}/')
    execute(f'ln -s /Applications {installer_volume}/Applications')
    copy_file(f'../img/install_macos.png', installer_volume, '.bg.png')
    printc('\nInstaller created', Colors.OKGREEN)
    print('It should be already mounted and opened now, configure its visual appearance, then unmount and rerun this command')
    print('Note: use Cmd+J to show a folder setup window')
    print('Note: use Cmd+Shift+. to show hidden files and select a background image')
    print('')
    exit(0)
  else:
    print('Installer image found, updating content...')
  
  execute(f'hdiutil attach {installer_img}')
  execute('sleep 1')
  remove_dir(f'{installer_volume}/{PROJECT_EXE}')
  shutil.copytree(f'{REDIST_DIR}/{image_exe}', f'{installer_volume}/{PROJECT_EXE}')
  execute(f'hdiutil detach {installer_volume}/')
  execute('sleep 1')

  print('Compressing...')
  execute(f'hdiutil convert {installer_img} -ov -format UDZO -o {package_name}.dmg')

########################################################################

if IS_WINDOWS:
  make_package_for_windows()
elif IS_LINUX:
  make_package_for_linux()
elif IS_MACOS:
  make_package_for_macos()

print('\nPackage created: {}'.format(package_name))
printc('Done\n', Colors.OKGREEN)
