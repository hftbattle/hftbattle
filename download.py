#!/usr/bin/env python

import sys
import zipfile

try:
    from urllib.request import urlretrieve
except ImportError:
    from urllib import urlretrieve


def progress_bar(count, block_size, total_size):
    percent = int(100 * count * block_size / total_size)
    half = percent // 2
    sys.stdout.write('\r[%s%s] %s %%' % ('=' * half, ' ' * (50 - half), percent))
    sys.stdout.flush()


def download(url, filename):
    print('Downloading %s from %s:' % (filename, url))
    urlretrieve(url, filename, reporthook=progress_bar)
    print('\nDone!')


def unarchive(filename):
    print('Unarchiving %s:' % filename)
    zf = zipfile.ZipFile(filename, allowZip64=True)
    total_size = sum((file.file_size for file in zf.infolist()))
    cur_size = 0
    for file in zf.infolist():
        cur_size += file.file_size
        percent = cur_size * 100 / total_size
        sys.stdout.write('\r%2d%%' % percent)
        sys.stdout.flush()
        zf.extract(file)
    print('\nDone!')


URL = 'https://www.dropbox.com/s/ko2zgwt004yejts/data.zip?dl=1'
FILENAME = 'data.zip'

download(URL, FILENAME)
unarchive(FILENAME)
