#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
from sys import argv
from sys import exit
import hirakana as hk

if len(argv) < 2:
    print 'error: input file is not specified, exit'
    exit(-1)
infile_name = argv[1]
if not os.path.isfile(infile_name + '.hirakana'):
    print 'error: input file does not exist, exit'
    exit(-1)

max_len = 6
if len(argv) > 2:
    max_len = int(argv[2])
print 'embedding dimension:', max_len

infile = open(infile_name + '.hirakana', 'r')
outfile = open(infile_name + '.encoded', 'w')

for line in infile.readlines():
    word = line[:-1].decode('utf-8')
    if len(word) > max_len:
        print 'warning: %s\'s length(%d) exceeds maximum length(%d)'%(word.encode('utf-8'), len(word), max_len)
    aux = u'补'*(max_len - len(word))
    word = word + aux
    #print word.encode('utf-8')
    vec = hk.jpword2list(word)
    for x in vec:
        outfile.writelines('%d '%(x, ))
    outfile.writelines('\n')

outfile.close()
print '...'
print 'done!'
print 'output file:', infile_name + '.encoded'
