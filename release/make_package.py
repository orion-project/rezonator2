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


########################################################################
#                             Windows

def make_package_for_windows():
  qt_dir = find_qt_dir()

  print_header('Run windeployqt...')
  execute(f'windeployqt ..\\..\\bin\\{PROJECT_EXE} --dir . ' +
          '--no-translations --no-system-d3d-compiler --no-opengl-sw')
  execute(f'windeployqt {os.path.join(qt_dir, "assistant.exe")} --dir . ' +
          '--no-translations --no-system-d3d-compiler --no-opengl-sw')

  print_header('Clean some excessive files...')
  remove_files(['libEGL.dll', 'libGLESV2.dll'])
  remove_files_in_dir('sqldrivers', ['qsqlmysql.dll', 'qsqlodbc.dll', 'qsqlpsql.dll'])
  remove_files_in_dir('imageformats', ['qicns.dll', 'qtga.dll', 'qtiff.dll', 'qwbmp.dll', 'qwebp.dll'])

  print_header('Copy project files...')
  copy_file('..\\..\\bin\\' + PROJECT_EXE, '.')
  copy_file('..\\..\\bin\\rezonator.qch', '.')
  copy_file('..\\..\\bin\\rezonator.qhc', '.')
  shutil.copytree('..\\..\\bin\\examples', 'examples')
  copy_files(qt_dir, ['assistant.exe'], '.')

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
  copy_file(os.path.join(find_qt_dir(), 'assistant'), 'usr/bin')
  copy_file('../../bin/rezonator.qch', 'usr/bin')
  copy_file('../../bin/rezonator.qhc', 'usr/bin')
  shutil.copytree('../../bin/examples', 'usr/bin/examples')
  copy_file(f'../../release/{PROJECT_NAME}.desktop', 'usr/share/applications')
  shutil.copyfile('../../img/icon/main_2_256.png', f'usr/share/icons/hicolor/256x256/apps/{PROJECT_NAME}.png')

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
  remove_dir(PROJECT_EXE)
  # Assitant, QCH and QHC help files are already in the bundle dir
  # This is done during help compilation (see help/make.sh)
  shutil.copytree('../../bin/' + PROJECT_EXE, PROJECT_EXE)

  print_header('Run macdeployqt...')
  execute('macdeployqt ' + PROJECT_EXE)

  print_header('Copy project files...')
  shutil.copytree('../../bin/examples', PROJECT_EXE + '/Contents/MacOS/examples')

  print_header('Processing Assistant...')
  shutil.copytree(os.path.join(find_qt_dir(), 'Assistant.app'), 'Assistant.app')
  execute('macdeployqt Assistant.app -appstore-compliant')
  print_header('Patching Assistant...')
  # `macdeployqt` (at least in Qt 5.10) doesn't add this RPATH to assistant exe, 
  # and as a result when packed to rezonator.app bundle it can't find libraries
  execute('install_name_tool -add_rpath @loader_path/../Frameworks Assistant.app/Contents/MacOS/Assistant')
  execute('install_name_tool -delete_rpath @loader_path/../../../../lib Assistant.app/Contents/MacOS/Assistant')
  print_header('Copy Assistant files to bundle...')
  copy_file('Assistant.app/Contents/MacOS/Assistant', PROJECT_EXE + '/Contents/MacOS', target_fn='assistant')
  shutil.copytree('Assistant.app/Contents/Frameworks/QtHelp.framework', PROJECT_EXE + '/Contents/Frameworks/QtHelp.framework')
  shutil.copytree('Assistant.app/Contents/Frameworks/QtSql.framework', PROJECT_EXE + '/Contents/Frameworks/QtSql.framework')
  os.mkdir(PROJECT_EXE + '/Contents/PlugIns/sqldrivers')
  copy_file('Assistant.app/Contents/PlugIns/sqldrivers/libqsqlite.dylib', PROJECT_EXE + '/Contents/PlugIns/sqldrivers')

  print_header('Clean some excessive files...')
  remove_files_in_dir(PROJECT_EXE + '/Contents/PlugIns/sqldrivers', [
    'libqsqlmysql.dylib', 'libqsqlpsql.dylib'])
  remove_files_in_dir(PROJECT_EXE + '/Contents/PlugIns/imageformats', [
    'libqico.dylib', 'libqtga.dylib', 'libqtiff.dylib', 'libqwbmp.dylib', 'libqwebp.dylib'])

  print_header('Pack application bundle to dmg...')
  global package_name
  package_name = package_name + '.dmg'
  remove_files(['tmp.dmg', '../' + package_name])
  execute(f'hdiutil create tmp.dmg -ov -volname {PROJECT_NAME} -fs HFS+ -srcfolder {PROJECT_EXE}')
  execute(f'hdiutil convert tmp.dmg -format UDZO -o ../{package_name}')

########################################################################

if IS_WINDOWS:
  make_package_for_windows()
elif IS_LINUX:
  make_package_for_linux()
elif IS_MACOS:
  make_package_for_macos()

print('\nPackage created: {}'.format(package_name))
printc('Done\n', Colors.OKGREEN)
