#!/usr/bin/env python
#
# Generate rgb arrays from images and output
# them into a C header file suitable to be compiled
# along and loadable with glTexImage2D
# 
import os
import sys
import string
import Image

if len(sys.argv) < 3:
    print 'Usage: generate_textures <header_file> <image> [<image> ...]'
    sys.exit(1)
images = []

basename = os.path.basename(sys.argv[1]).split('.')[0]
h = open(sys.argv[1], 'w')
h.write('#ifndef ' + basename + '_included\n')
h.write('#define ' + basename + '_included\n')
h.write('#include <GL/gl.h>\n')
h.write('GLuint texture[' + str(len(sys.argv[2:])) + '];\n')

images = []
istr = ''
for index, path in enumerate(sys.argv[2:]):
    base = os.path.basename(path).split('.')[0]
    h.write('#define TEX_' + str(base.upper()) + ' ' + str(index) + '\n')
    # print 'index + ' + str(index) + ' file ' + path
    im = Image.open(path).convert('RGB')
    width = im.size[0]
    height = im.size[1]
    data = im.load()
    images += base
    h.write('char tex_' + base + '[' + str(width * height * 3) + ']' +' = {\n')
    for x in range(width):
        for y in range(height):
            h.write('0x%2.2x, 0x%2.2x, 0x%2.2x' % im.getpixel((x, y)))
            if not (x == (width - 1) and y == (height - 1)):
                h.write(',')
            if y % 5 == 0:
                h.write('\n')
            else:
                h.write(' ')
    h.write('};\n')
    istr += 'glGenTextures(1, &texture[' + str(index) + ']);\n'
    istr += 'glBindTexture(GL_TEXTURE_2D, texture[' + str(index) + ']);\n'
    istr += 'glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);\n'
    istr += 'glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);\n'
    istr += 'glTexImage2D(GL_TEXTURE_2D, 0, 3, %s, %s, ' % (width, height)
    istr += '0, GL_RGB, GL_UNSIGNED_BYTE, tex_' + base + ');\n'

h.write('void textures_init(void)\n')
h.write('{\n')
h.write(istr)
h.write('}\n')
h.write('#endif\n')
h.close()

