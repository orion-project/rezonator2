#!/usr/bin/env python
#
# Script updates version information in version.pri and version.rc,
# and creates version.txt file contaning version string "X.Y.Z-codename"
# that can be read by another scripts.
#
# Target version should be given via script parameter:
#    ./make_version.py 2.0.2-alpha2
#

import os
import re
import sys
import datetime

def get_file_text(file_name):
    with open(file_name, 'r') as f:
        return f.read()

def set_file_text(file_name, text):
    with open(file_name, 'w') as f:
        f.write(text)

if __name__ == '__main__':
    # Navigate to repository dir
    curdir = os.path.dirname(os.path.realpath(__file__))
    os.chdir(os.path.join(curdir, '..'))

    # Get version string from command line
    if len(sys.argv) < 2:
        print('ERROR: No version string is given')
        exit()

    version_parts = sys.argv[1].split('.')
    if len(version_parts) != 3:
        print('ERROR: Invalid version string')
        exit()

    version_major = version_parts[0]
    version_minor = version_parts[1]
    version_patch = version_parts[2]
    codename = ''

    patch_parts = version_patch.split('-')
    if len(patch_parts) > 1:
        version_patch = patch_parts[0]
        codename = patch_parts[1]

    # We have version now
    version_str = '%s.%s.%s-%s' % (version_major, version_minor, version_patch, codename)
    print('Version: %s' % version_str)
    
    year = str(datetime.datetime.now().year)
    
    # Update version.txt
    print('Updating version.txt')
    set_file_text('release/version.txt', version_str)

    # Update version.pri
    print('Updating version.pri')
    file_name = 'release/version.pri'
    text = get_file_text(file_name)

    def replace(key, value):
        global text
        text = re.sub('^' + key + '=.+$', key + '=' + value, text, 1, re.MULTILINE)

    replace('APP_VER_MAJOR', version_major)
    replace('APP_VER_MINOR', version_minor)
    replace('APP_VER_PATCH', version_patch)
    replace('APP_VER_CODENAME', codename)
    replace('APP_VER_YEAR', year)
    set_file_text(file_name, text)
    
    # Update version.rc
    print('Updating version.rc')
    text = get_file_text('release/version.rc.template')
    text = text.replace('{v1}', version_major)
    text = text.replace('{v2}', version_minor)
    text = text.replace('{v3}', version_patch)
    text = text.replace('{v4}', '0')
    text = text.replace('{year}', year)
    text = text.replace('{codename}', codename)
    set_file_text('release/version.rc', text)

    print('OK')
