#!/usr/bin/env python

import itertools
import math
import os
import os.path
import pprint
import cPickle as pickle

from testsuite import *

translate = {
    '1A43_autopsf': 'hiv',
    '1A8O_autopsf_modified': None,
    '2IXZ_fullbox': None,
    '3DHR_fullbox': None,
    'hiv': None,
    'hot': 'hotwater',
    'mscl': 'mscl',
    'rabies_full': 'rabies',
    'smallwater': 'smallwater',
    'water_large': None,
    'water_roomtemperature': 'water',
    'gala_pmf': None,
    '1A8O_autopsf_modified_large': None,
    '2IXZ_fullbox_large': None,
    '2IXZ_fullbox_really_large': None,
    'full_solvate_rabies': None,
}

def print_times(quant=None):
    if quant is None:
        print '12 bit:'
        print_times(12)
        print '\n8 bit:'
        print_times(8)
        return

    r = get_results()
    a = sorted((k[0], v[1], k) for k, v in r.items() if k[1] == str(quant))
    for i in a:
        print ('%.2f' % i[1]).ljust(10),
        print i[2][0].ljust(13),
        print i[2][2]


def print_size(quant=None):
    if quant is None:
        print '12 bit:'
        print_size(12)
        print '\n8 bit:'
        print_size(8)
        return

    r = get_results()
    a = sorted((k[0], v[0], k) for k, v in r.items() if k[1] == str(quant))
    for i in a:
        print ('%d' % (i[1]/1024)).ljust(10),
        print i[2][0].ljust(13),
        print i[2][2]


#(dcd, quant, comp), (size, _time)
def perm_csv(scheme, quant):
    perms = PERMUTATIONS[:]
    perms.remove('null')

    r = get_results()

    size = lambda dcd, p : r[(dcd, quant, '%s_%s' % (scheme, p))][0]

    print ','.join(['dcd'] + perms)
    for dcd in DCDS:
        null = size(dcd, 'null')
        row = [dcd] + [size(dcd, p) - null for p in perms]
        print ','.join(map(str, row))


def time_csv(quant):
    quant = str(quant)
    r = get_results()

    def time(dcd, scheme):
        if 'perms' in TESTS[scheme]:
            scheme = scheme + '_null'
        return r[(dcd, quant, scheme)][1]

    schemes = 'quant gzip omeltchenko gumhold dg watercomp'.split()
    dcds = 'smallwater water hotwater rabies hiv mscl'.split()

    print ','.join(['dcd'] + schemes)
    for dcd in dcds:
        row = [dcd] + [int(math.ceil(time(dcd, s))) for s in schemes]
        print ','.join(map(str, row))


def res_table(quant):
    quant = str(quant)
    r = get_results()

    size   = lambda d, s : r[(d, quant, s)][0]
    size_p = lambda d, s, p : size(d, '%s_%s' % (s, p))

    perms = PERMUTATIONS[:]
    perms.remove('null')
    schemes = ['quant', 'gzip', 'omeltchenko', 'gumhold', 'dg', 'watercomp']

    print '\\begin{tabular}{|lr|rrr|rr|rr|rrr|}'
    print '  \\hline'

    emph = lambda li : ['\\textbf{%s}' % s for s in li]
    col_head =  ['DCD', 'atoms'] + schemes[:2] + ['omel']
    col_head =  ['\\multirow{2}{*}{%s}' % s for s in emph(col_head)]
    col_head += ['\\multicolumn{2}{|c|}{%s}' % s for s in emph(schemes[3:-1])]
    col_head += ['\\multicolumn{3}{|c|}{%s}' % emph([schemes[-1]])[0]]
    col_head2 = ['']*5 + ['\\emph{null}', '\\emph{best}']*3 + ['\\emph{gain}']
    for i in col_head, col_head2:
        print '  %s \\\\' % (' & '.join(i))
        print
    print '  \\hline\n'

    dcds = 'smallwater water hotwater rabies hiv mscl'.split()
    atoms = '699 96\\,603 96\\,603 464\\,099 16\\,470 111\,016'.split()
    for dcd, atom in zip(dcds, atoms):
        dcd_path = '%s/%s.dcd' % (INPUT_DIR, dcd)
        s = os.stat(dcd_path).st_size

        row  = [dcd, '$%s$' % atom]
        vals = []
        for scheme in schemes:
            if 'perms' in TESTS[scheme]:
                a = size_p(dcd, scheme, 'null')
                b = min(size_p(dcd, scheme, p) for p in perms)
            else:
                a = size(dcd, scheme)
                b = None
            a = a * 100.0 / s
            vals.append(a)
            if b:
                b = b * 100.0 / s
                vals.append(b)
        row += ['$%.2f$' % a for a in vals]
        valid = [2,3,4,6,8,10]
        row_valid = [row[i] for i in valid]
        vals_cmp  = [vals[i-2] for i in valid[:-1]]
        m1 = min(row[2:])
        m2 = min(row_valid)
        bolded = []
        for c in row:
            if c == m2:
                c = '$\\textbf{%s}$' % c[1:-1]
            if c == m1:
                c = '$\\emph{%s}$' % c[1:-1]
            bolded.append(c)
        bolded.append('$%.2f$' % (min(vals_cmp) - vals[-1]))
        print '  ' + ' & '.join(bolded), '\\\\'
        print

    print '  \\hline'
    print '\\end{tabular}'


if __name__ == '__main__':
    #perm_csv('watercomp', '12')
    #print_size(12)
    res_table(12)
    #time_csv(8)
