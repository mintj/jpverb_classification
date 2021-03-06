#!/usr/bin/python
# -*- coding: utf-8 -*-

import random

hirakana_dict = {\
        'あ':   0, 'い':   1, 'う':   2, 'え':   3, 'お':   4,\
        'か':  10, 'き':  11, 'く':  12, 'け':  13, 'こ':  14,\
        'が':  20, 'ぎ':  21, 'ぐ':  22, 'げ':  23, 'ご':  24,\
        'さ':  30, 'し':  31, 'す':  32, 'せ':  33, 'そ':  34,\
        'ざ':  40, 'じ':  41, 'ず':  42, 'ぜ':  43, 'ぞ':  44,\
        'た':  50, 'ち':  51, 'つ':  52, 'て':  53, 'と':  54,\
        'だ':  60, 'ぢ':  61, 'づ':  62, 'で':  63, 'ど':  64,\
        'な':  70, 'に':  71, 'ぬ':  72, 'ね':  73, 'の':  74,\
        'は':  80, 'ひ':  81, 'ふ':  82, 'へ':  83, 'ほ':  84,\
        'ば':  80, 'び':  91, 'ぶ':  92, 'べ':  93, 'ぼ':  94,\
        'ぱ': 100, 'ぴ': 101, 'ぷ': 102, 'ぺ': 103, 'ぽ': 104,\
        'ま': 110, 'み': 111, 'む': 112, 'め': 113, 'も': 114,\
        'や': 120,            'ゆ': 122,            'よ': 124,\
        'ら': 130, 'り': 131, 'る': 132, 'れ': 133, 'ろ': 134,\
        'わ': 140,                                  'を': 144,\
        'ん': 156,                                            \
        'ぁ': 160, 'ぃ': 161, 'ぅ': 162, 'ぇ': 163, 'ぉ': 164,\
        'っ': 172,                                            \
        'ゃ': 180,            'ゅ': 182,            'ょ': 184,\
        }

hirakana_in_dan = {\
        'a': ['あ', 'か', 'が', 'さ', 'ざ', 'た', 'だ', 'な', 'は', 'ば', 'ぱ', 'ま', 'や', 'ら', 'わ', 'ぁ',       'ゃ',],\
        'i': ['い', 'き', 'ぎ', 'し', 'じ', 'ち', 'づ', 'に', 'ひ', 'び', 'ぴ', 'み',       'り',       'ぃ',            ],\
        'u': ['う', 'く', 'ぐ', 'す', 'ず', 'つ', 'づ', 'ぬ', 'ふ', 'ぶ', 'ぷ', 'む', 'ゆ', 'る',       'ぅ', 'っ', 'ゅ',],\
        'e': ['え', 'け', 'げ', 'せ', 'ぜ', 'て', 'で', 'ね', 'へ', 'べ', 'ぺ', 'め',       'れ',       'ぇ',            ],\
        'o': ['お', 'こ', 'ご', 'そ', 'ぞ', 'と', 'ど', 'の', 'ほ', 'ぼ', 'ぽ', 'も', 'よ', 'ろ', 'を', 'ぉ',       'ょ',],\
        'none': ['ん',],\
        'u_verb': ['う', 'く', 'ぐ', 'す', 'つ', 'ぬ', 'ぶ', 'む', 'る',],\
        }

def jpword2list(jp_word):
    numlist = []
    for kana in jp_word:
        kana = kana.encode('utf-8')
        if kana in hirakana_dict.keys():
            numlist.append(hirakana_dict[kana])
        else:
            numlist.append(999)

    return numlist

def get_random_kana(dan='all', tail_of_verb=False):
    if dan not in ['a', 'i', 'u', 'e', 'o', 'none', 'all']:
        return '空'

    if dan == 'all':
        hirakana_pool = hirakana_dict.keys()
    elif dan == 'u' and tail_of_verb:
        hirakana_pool = hirakana_in_dan['u_verb']
    else:
        hirakana_pool = hirakana_in_dan[dan]
        
    rand = random.randint(0, len(hirakana_pool)-1)
    return hirakana_pool[rand]
