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
  execute(f'windeployqt ..\\..\\bin\\{PROJECT_EXE} --dir . --no-translations --no-system-d3d-compiler --no-opengl-sw')
  execute(f'windeployqt {os.path.join(qt_dir, "assistant.exe")} --dir . --no-translations --no-system-d3d-compiler --no-opengl-sw')

  print_header('Clean some excessive files...')
  remove_files(['libEGL.dll', 'libGLESV2.dll'])
  remove_files_in_dir('sqldrivers', ['qsqlmysql.dll', 'qsqlodbc.dll', 'qsqlpsql.dll'])
  remove_files_in_dir('imageformats', ['qicns.dll', 'qtga.dll', 'qtiff.dll', 'qwbmp.dll', 'qwebp.dll'])

  print_header('Copy project files...')
  copy_file('..\\..\\bin\\' + PROJECT_EXE, '.')
  copy_file('..\\..\\bin\\rezonator_test.bat', '.')
  copy_file('..\\..\\bin\\rezonator_test_nogui.bat', '.')
  copy_file('..\\..\\bin\\rezonator.qch', '.')
  copy_file('..\\..\\bin\\rezonator.qhc', '.')
  shutil.copytree('..\\..\\bin\\examples', 'examples')
  shutil.copytree('..\\..\\bin\\test_files', 'test_files')

  # Seems sometimes windeployqt does copy these files,
  # but I definitely had cases when they had not been in the the place...
  print_header('Copy additional files...')
  # In Qt 5.15.2 and 6.5.2 windeployqt is fixed and copies all required dlls
  #copy_files(find_qt_dir(), ['libgcc_s_seh-1.dll', 'libstdc++-6.dll', 'libwinpthread-1.dll'], '.')
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

  copy_file('../../bin/' + PROJECT_EXE, 'usr/bin')
  # There will be error 'Could not determine the path to the executable' otherwise
  execute('chmod +x usr/bin/' + PROJECT_EXE)

  copy_file('../../release/{}.desktop'.format(PROJECT_NAME), 'usr/share/applications')
  shutil.copyfile('../../img/icon/procyon.iconset/icon_256x256.png', 'usr/share/icons/hicolor/256x256/apps/{}.png'.format(PROJECT_NAME))

  print_header('Copy project files...')
  shutil.copytree('../../bin/dicts', 'usr/bin/dicts')

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
  execute((
    './{} {}/usr/share/applications/{}.desktop ' +
    '-appimage -no-translations -no-copy-copyright-files ' +
    '-extra-plugins=iconengines,imageformats/libqsvg.so ' +
    '-exclude-libs=libqsqlmysql,libqsqlpsql,libqsqlodbc,libqicns,libqico,libqtga,libqtiff,libqwbmp,libqwebp'
  ).format(linuxdeployqt, REDIST_DIR, PROJECT_NAME))

  # Seems we can't specify target AppImage name, so find it
  default_appimage_names = glob.glob(PROJECT_NAME + '-x*.AppImage')
  if len(default_appimage_names) != 1:
    print_error_and_exit('Unable to find created AppImage file')

  global package_name
  package_name = '{}-linux-x{}.AppImage'.format(package_name, get_exe_bits('../bin/' + PROJECT_EXE))
  remove_files([package_name])
  os.rename(default_appimage_names[0], package_name)


########################################################################
#                              macOS

def make_package_for_macos():
  print_header('Copy application bundle...')
  remove_dir(PROJECT_EXE)
  shutil.copytree('../../bin/' + PROJECT_EXE, PROJECT_EXE)

  print_header('Run macdeployqt...')
  execute('macdeployqt {}'.format(PROJECT_EXE))

  print_header('Copy project files...')
  shutil.copytree('../../bin/dicts', PROJECT_EXE + '/Contents/MacOS/dicts')

  print_header('Clean some excessive files...')
  remove_files_in_dir(PROJECT_EXE + '/Contents/PlugIns/sqldrivers', [
    'libqsqlmysql.dylib', 'libqsqlpsql.dylib'])
  remove_files_in_dir(PROJECT_EXE + '/Contents/PlugIns/imageformats', [
    'libqico.dylib', 'libqtga.dylib', 'libqtiff.dylib', 'libqwbmp.dylib', 'libqwebp.dylib'])

  print_header('Pack application bundle to dmg...')
  global package_name
  package_name = package_name + '.dmg'
  remove_files(['tmp.dmg', '../' + package_name])
  execute('hdiutil create tmp.dmg -ov -volname {} -fs HFS+ -srcfolder {}'.format(PROJECT_NAME, PROJECT_EXE))
  execute('hdiutil convert tmp.dmg -format UDZO -o ../{}'.format(package_name))

########################################################################

if IS_WINDOWS:
  make_package_for_windows()
elif IS_LINUX:
  make_package_for_linux()
elif IS_MACOS:
  make_package_for_macos()

print('\nPackage created: {}'.format(package_name))
printc('Done\n', Colors.OKGREEN)
