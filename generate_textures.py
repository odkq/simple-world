#!/usr/bin/env python
#
# Simple World. A simple playground. Tool that generates a header
# so texture files are included directly in the executable, along
# with a quick and dirty compression algorithm :>
#
# (C) Copyright 2013 Pablo Martin Medrano <pablo@odkq.com> 
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3
# of the License.
#
__doc__ = """
 generate_textures.py [options] <header> <image> [<image>]'

 build tool for simple world

 Generate rgb arrays from image files and output
 them into a C header file suitable to be compiled
 along and loadable with glTexImage2D

 Optional trivial compression with -c (only for
 textures with less than 256 colors :>)
"""
import os
import sys
import string
import Image
from optparse import OptionParser

# Compress an RGB array that have less than 256 colors;
# First get the values for all different R,G,B triplets
# and store them in a table (called pallete back in the
# mists of time). Then replace the original array with
# indexes to that table. That is 33%# compression 
#
# Then find regions of contiguous pixels with the same
# value and replace them by special sequences storing
# only (length, value) pairs. This is called 'Run Length
# encoding'. If the texture have big solid color areas
# compression achieved with this simple technique is
# surprising.
#
# This is the routine for 'compression', the routine
# for decompression is written in C and appendend to
# the same header file that this file builds
#
# I could have used zlib or PNG, but this is way more fun :>
#
def compress(array):
    palette, pixels = rgb_map(array)
    print 'after rgb_map pallete: ' + str(len(palette))
    print '               pixels: ' + str(len(pixels))
    compressed = rle_compress(pixels)
    print 'after compress pixels: ' + str(len(compressed))
    if compressed == None:
        return None
    return palette, pixels, compressed

# Todo: Do a first pass to get the pallete and
# then remap the array so the marker byte is the
# one with less occurrences
def rgb_map(array):
    colors = {}
    palette = []
    output = []
    index = 0
    t = 0

    while t < len(array):
        r, g, b = array[t], array[t + 1], array[t + 2]
        test = '%2.2x%2.2x%2.2x' % (r, g, b)
        if test in colors:
            output.append(colors[test])
        else:
            colors[test] = index
            palette.append(r)
            palette.append(g)
            palette.append(b)
            output.append(index)
            index += 1
            #if index > 256:
            #    print 'error index ' + str(index)
        t += 3

    print 'colors ' + str(len(colors)) + ' index ' + str(index)
    return palette, output

def rle_compress(array):
    r = []
    i = 0
    while True:
        if i == len(array):
            break
        b = array[i]
        n = 1
        i += 1
        while True:
            if i == len(array):
                # Found the end of file
                break
            if n == 255:
                # We only store 1 byte for length
                # if more occurrences appear, a new
                # 'block' will be allocated.
                break
            c = array[i]
            if c == b:
                n += 1
                i += 1
            else:
                break
        if n > 4:
            # Note that we replace the original array
            # with 3 bytes (mark, length, byte), so
            # no-gain in compressing less than 4 bytes
            # print str(n) + ' occurrences of ' + str(b)
            if n > 255:
                return None
            r.append(0xf1)
            r.append(n)
            r.append(b)
        else:
            if (b == 0xf1):
                # If the original byte is our 'marker'
                # character, we escape it with 0x01
                # (wich is a length we never use)
                for j in range(n):
                    r.append(b)
                    r.append(0x01)
            else:
                for j in range(n):
                    r.append(b)
    return r

def writebuffer(h, buf):
    h.write('{');
    for i, b in enumerate(buf[:-1]):
        h.write('0x%2.2x, ' % b)
        if ((i + 1) % 12) == 0:
            h.write('\n')
    h.write('0x%2.2x};\n' % buf[-1:][0])

parser = OptionParser(__doc__)
parser.add_option('-c', '--compress', action='store_true', dest='compress',
    default=False, help='Compress the textures with a simple algorithm')
parser.add_option('-e', '--executable', action='store_true', dest='executable',
    default=False, help='Generate an executable with main for testing')
(options, args) = parser.parse_args()
if len(args) < 2:
    print 'Specify the output header file and at least one image file. See -h'
    sys.exit(1)

images = []

basename = os.path.basename(args[0]).split('.')[0]
h = open(args[0], 'w')
if not options.executable:
    h.write('#ifndef ' + basename + '_included\n')
    h.write('#define ' + basename + '_included\n')
else:
    h.write('#include <unistd.h>\n')
    h.write('#include <sys/types.h>\n')
    h.write('#include <sys/stat.h>\n')
    h.write('#include <fcntl.h>\n')
    h.write('#include <stdlib.h>\n')
    h.write('#include <stdio.h>\n')
    h.write('#include <string.h>\n')
h.write('#include <GL/gl.h>\n')
h.write('GLuint texture[' + str(len(sys.argv[2:])) + '];\n')
images = []
istr = ''
for index, path in enumerate(args[1:]):
    base = os.path.basename(path).split('.')[0]
    h.write('#define TEX_' + str(base.upper()) + ' ' + str(index) + '\n')
    # print 'index + ' + str(index) + ' file ' + path
    im = Image.open(path).convert('RGB')
    width = im.size[0]
    height = im.size[1]
    data = im.load()
    images += base

    buf = []
    for x in range(width):
        for y in range(height):
            r, g, b = im.getpixel((x, y))
            buf.append(r)
            buf.append(g)
            buf.append(b)

    length = width * height * 3
    if options.compress:
        palette, pixels, compressed = compress(buf)
        h.write('unsigned char pixelbuffer_' + str(index))
        h.write('[' + str(len(compressed)) + '] =\n')
        writebuffer(h, compressed)
        h.write('unsigned char palette_' + str(index))
        h.write('[' + str(len(palette)) + '] =\n')
        writebuffer(h, palette)
        h.write('unsigned char *tex_' + base + ';\n');
    else:
        h.write('unsigned char tex_' + base + '[' + str(length) + \
                ']' + ' = \n')
        writebuffer(h, buf)

    if options.compress:
        istr += '{\n'
        istr += ' int length;\n'
        istr += ' unsigned char *tmp;\n'
        istr += 'length = rle_decompress(&tmp'
        istr += ', ' + str(len(pixels))
        istr += ', pixelbuffer_' + str(index) 
        istr += ', ' + str(len(compressed))
        # &tex_' + base
        # istr += ', pallete_' + str(index)
        # istr += ', ' + str(len(palette))
        istr += ');\n'
        istr += 'length = rgb_unmap(&tex_' + base + ', ' + str(len(buf)) + ', '
        istr += 'tmp, ' + str(len(pixels)) + ', palette_' + str(index) + ','
        istr += str(len(palette)) +');\n'
        istr += 'free(tmp);\n';
        istr += '}\n'

# int rgb_unmap(unsigned char **ret, unsigned long retlen, char *buffer,
# unsigned long buflen, unsigned char *palette, unsigned long pallen)

    if options.executable:
        istr += '{int d; d = open("' + base
        if options.compress:
            istr += '.compress'
        istr += '", O_WRONLY | O_CREAT, '
        istr += 'S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);\n'
        istr += '  write(d, (const void *)tex_' + base + ', '
        istr += str(length) + ');\n'
        istr += '  close(d);}\n'
    istr += 'glGenTextures(1, &texture[' + str(index) + ']);\n'
    istr += 'glBindTexture(GL_TEXTURE_2D, texture[' + str(index) + ']);\n'
    istr += 'glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);\n'
    istr += 'glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);\n'
    istr += 'glTexImage2D(GL_TEXTURE_2D, 0, 3, %s, %s, ' % (width, height)
    istr += '0, GL_RGB, GL_UNSIGNED_BYTE, tex_' + base + ');\n'

if options.compress:
    h.write('''
/*
 * RLE decompressing routine for textures included with generate_textures.py
 *
 * Basically, replace any occurrence of a sequence of bytes with a special
 * character followed by the length. Arbitrarily, the markers choosed
 * are 0xf1, for chunks with size fittable in 1 byte, 0xf2 for two bytes
 * and 0xf3 for three bytes. To allow for those values to appear on input,
 * they are 'escaped' with 0x01.
 *
 * Given the input 0x02 0x01 0x01 0x01 0x01 0x01 0x01 0x04, the compressor
 * will output:    0x02 0xf1 0x06 0x01 0x04
 *
 * Given the input 0x02 0xf1 0xf2 0xf4 0x04, the compressor will output:
 * 0x02 0xf1 0x01 0xf2 0xf4 0x04
 */
int rle_decompress(unsigned char **ret, unsigned long ret_length,
unsigned char *array, unsigned long length)
{
    int i, j, l;
    unsigned char c, b, s, *r;

    r = (unsigned char *)malloc(ret_length);
    l = 0;
    for(i = 0; i < length; i++) {
        c = array[i];
        if (c != 0xf1) {
            r[l] = c;
            l += 1;
            continue;
        }
        if (i == (length - 1)) {
            free(r);
            return -1;
        }
        s = array[i + 1];
        if (s == 0x01) {
            r[l] = c;   /* 0xf1 */
            l++;
            i++;
            continue;
        }
        b = array[i + 2];
        for (j = 0; j < s; j++) {
            r[l] = b;
            l++;
        }
        i += 2;
    }
    *ret = r;
    return l;
}
/*
 * rgb_unmap
 */
int rgb_unmap(unsigned char **ret, unsigned long retlen, char *buffer,
unsigned long buflen, unsigned char *palette, unsigned long pallen)
{
    unsigned char *r, *d, c;
    unsigned long i;

    if ((r = (unsigned char *)malloc(retlen)) == NULL) {
        return -1;
    }
    d = r;
    for (i = 0; i < buflen; i++) {
        c = buffer[i];
        memcpy((void *)d, (void *)(palette + (c * 3)), 3);
        d += 3;
    }
    *ret = r;
    return (int)(d - r);
}
''')
h.write('void textures_init(void)\n')
h.write('{\n')
h.write(istr)
h.write('}\n')
if options.executable:
    h.write('int main(void)\n')
    h.write('{\n')
    h.write('   textures_init();\n')
    h.write('   return 0;\n')
    h.write('}\n')
else:
    h.write('#endif\n')
h.close()

