#!/usr/bin/env python
#
# Script searches for the latest git tag starting with "version",
# supposing this tags has format "version-M.N.K-codename",
# and calculates fourth version digit as a number of commits since tag date.
# So full current version will be "M.N.K.L-codename".
#
# Script updates version information in version.pri and version.rc,
# and creates version.txt file contaning version string "M.N.K.L-codename"
# that can be read by another scripts.
#

import locale
import os
import re
import subprocess

def execute(cmdline, join_stdout = True):
    stdout = [];
    os_encoding = locale.getpreferredencoding()
    p = subprocess.Popen(cmdline, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    p.wait()
    stdout = [line.decode(os_encoding) for line in p.stdout.readlines()]
    if join_stdout:
        return ''.join(stdout).strip()
    else:
        return stdout

def get_latest_version_tag():
    latest_moment = 0
    latest_tag = ''
    latest_sha = ''
    version_tags = execute('git tag -l "version-*"', False)
    for tag in version_tags:
        tag_info = execute('git cat-file tag ' + tag, False)
        tag_sha = ''
        for line in tag_info:
            if line.startswith('object'):
                tag_sha = line.split(' ')[1]
            elif line.startswith('tagger'):
                moment = int(line.split(' ')[-2])
                if moment > latest_moment:
                    latest_moment = moment
                    latest_tag = tag
                    latest_sha = tag_sha
    return latest_tag.strip(), latest_sha.strip()

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

    # Get latest commit id
    latest_sha = execute('git rev-list --max-count=1 HEAD')
    print('Latest commit: %s' % latest_sha)

    # Get latest tag name and its commit id
    tag, tag_sha = get_latest_version_tag()
    if not tag or not tag_sha:
        print('ERROR: No version tags found')
        exit
    print('Latest version tag: %s' % tag)
    print('Tag commit: %s' % tag_sha)

    # Split tag name into three version numbers
    tag_parts = tag.split('-')
    version = tag_parts[1]
    codename = tag_parts[2]
    
    version_parts = version.split('.')
    if len(version_parts) != 3:
        print('ERROR: Invalid version number' % version)
        exit
    version_major = version_parts[0]
    version_minor = version_parts[1]
    version_micro = version_parts[2]

    # Count commits between latest SHA and tag, it will be fourth version number 
    commits_after_tag = execute('git rev-list ' + latest_sha + ' ^' + tag_sha + ' --count')
    if not commits_after_tag:
        print('ERROR: Empty commits number, must be 0 or more')
        exit
    
    # We have version now
    version_str = '%s.%s.%s.%s-%s' % (version_major, version_minor, version_micro, commits_after_tag, codename)
    print('Version: %s' % version_str)
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
    replace('APP_VER_MICRO', version_micro)
    replace('APP_VER_COMMITS', commits_after_tag)
    replace('APP_VER_CODENAME', codename)
    replace('APP_VER_SHA', latest_sha)
    set_file_text(file_name, text)
    
    # Update version.rc
    print('Updating version.rc')
    text = get_file_text('release/version.rc.template')
    text = text.replace('{v1}', version_major)
    text = text.replace('{v2}', version_minor)
    text = text.replace('{v3}', version_micro)
    text = text.replace('{v4}', commits_after_tag)
    text = text.replace('{codename}', codename)
    text = text.replace('{sha}', latest_sha)
    set_file_text('release/version.rc', text)

    print('OK')
