#!/usr/bin/env python

'''
    Script update version information in version.pri and version.rc
    It searches for the latest git tag starting with "version",
    supposing this tags has format "version-M.N.K-codename".
'''

import os
import subprocess

def execute(cmdline, join_stdout = True):
    stdout = [];
    p = subprocess.Popen(cmdline, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    stdout = p.stdout.readlines()
    p.wait()
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
                

if __name__ == '__main__':
    # Navigate to repository dir
    curdir = os.path.dirname(os.path.realpath(__file__))
    os.chdir(os.path.join(curdir, '..'))

    # Get latest commit id
    latest_sha = execute('git rev-list --max-count=1 HEAD')
    print 'Latest commit:', latest_sha

    # Get latest tag name and its commit id
    tag, tag_sha = get_latest_version_tag()
    if not tag or not tag_sha:
        print 'ERROR: No version tags found'
        exit
    print 'Latest version tag:', tag, tag_sha

    # Split tag name into version numbers
    tag_parts = tag.split('-')
    version = tag_parts[1]
    codename = tag_parts[2]
    
    version_parts = version.split('.')
    if len(version_parts) != 3:
        print 'ERROR: Invalid version number', version
        exit
    version_major = version_parts[0]
    version_minor = version_parts[1]
    version_build = version_parts[2]

    # Count commits between latest and tag
    commits_after_tag = execute('git rev-list ' + latest_sha + ' ^' + tag_sha + ' --count')
    if not commits_after_tag:
        print 'ERROR: Empty commits number, must be 0 or more'
        exit
    
    # We have version now
    print 'Version:', version_major, version_minor, version_build, commits_after_tag, codename

    # TODO Update version.pri
    # TODO Update version.rc
