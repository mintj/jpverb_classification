#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import hirakana as hk
import random

def gen_verb(length, katsuyou_type, noisy=False):
    if katsuyou_type == 1:
        return gen_verb_type1(length, noisy)
    elif katsuyou_type == 2:
        return gen_verb_type2(length)
    else:
        return gen_verb_type3(length)

def gen_verb_type1(length, noisy):
    tail = hk.get_random_kana('u', True)
    root = ''
    for i in range(length-1):
        root += hk.get_random_kana('all')
    
    # noisy, allow overlap with type 2
    if noisy:
        kana = hk.get_random_kana('all')
        root += kana
    # exclude overlap with type2
    else:
        if tail == 'る':
            kana = 'す'
            while kana == 'す' or kana in hk.hirakana_in_dan['i'] or kana in hk.hirakana_in_dan['e']:
                kana = hk.get_random_kana('all')
            root += kana
        else:
            root += hk.get_random_kana('all')
            
    return root + tail

def gen_verb_type2(length):
    tail = 'る'
    root = ''
    for i in range(length-1):
        root += hk.get_random_kana('all')

    # exclude overlap with type3
    if random.randint(1, 2)%2:
        kana = 'す'
        while kana == 'す':
            kana = hk.get_random_kana('i')
        root += kana
    else:
        root += hk.get_random_kana('e')

    return root + tail

def gen_verb_type3(length):
    tail = 'する'
    root = ''
    for i in range(length-1):
        root += hk.get_random_kana('all')

    return root + tail

opf1a = open('type1_clean.encoded', 'w')
opf1b = open('type1_noisy.encoded', 'w')
opf2  = open('type2.encoded', 'w')
opf3  = open('type3.encoded', 'w')

for i in range(100000):
    root_length = random.randint(1, 5)
    opf1a.writelines(gen_verb(root_length, 1)       + '\n')
    opf1b.writelines(gen_verb(root_length, 1, True) + '\n')
    opf2. writelines(gen_verb(root_length, 2)       + '\n')
    opf3. writelines(gen_verb(root_length, 3)       + '\n')

opf1a.close()
opf1b.close()
opf2. close()
opf3. close()
