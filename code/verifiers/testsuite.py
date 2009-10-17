#!/usr/bin/env python

import os
import os.path
import sys

PERMUTATIONS = 'null best naive interframe delta'.split()
DCDS = 'mscl smallwater mscl1000atoms mscl5frames'.split()

CMD_PATH = os.path.abspath(sys.argv[0])
WORKING_DIR = os.path.dirname(os.path.dirname(CMD_PATH))
OUTPUT_DIR = os.path.expanduser('~/dcd_output')

TESTS = {
    'common_interframe': {
        'bin'    : './common_interframe/driver',
        'dcds'   : DCDS,
        'author' : 'julian',
    },

    'dg': {
        'bin'    : './devillersgandoin/dgcomp',
        'perms'  : PERMUTATIONS,
        'dcds'   : DCDS,
        'author' : 'keegan',
    },

    'interframe': {
        'bin'    : './interframe/driver',
        'dcds'   : DCDS,
        'author' : 'julian',
    },

    'nearest_interframe': {
        'bin'    : './nearest_interframe/driver',
        'dcds'   : DCDS,
        'author' : 'julian',
    },

    'nn_interframe': {
        'bin'    : './nn_interframe/driver',
        'dcds'   : DCDS,
        'author' : 'julian',
    },

    'spline_interframe': {
        'bin'    : './spline_interframe/driver',
        'dcds'   : DCDS,
        'author' : 'julian',
    },

    'gumhold': {
        'bin'    : './gumhold/gumhold',
        'perms'  : PERMUTATIONS,
        'dcds'   : DCDS,
        'author' : 'keegan',
    },

    'omeltchenko': {
        'bin'    : './omeltchenko/driver',
        'dcds'   : DCDS,
        'author' : 'julian',
    },

    'quant': {
        'bin'    : './quantcomp/quantcomp',
        'dcds'   : DCDS,
        'author' : 'keegan',
    },

    'bzip2': {
        'bin'    : './quantcomp/bzip2comp',
        'dcds'   : DCDS,
        'author' : 'keegan',
    },
}


TESTS_ALL = {}
for k, v in TESTS.items():
    TESTS_ALL[k] = v
    if 'perms' in v:
        v2 = dict(v)
        del v2['perms']
        for p in v['perms']:
            test = {
                'bin'    : 'PERMUTATION="%s" %s' % (p, v['bin']),
                'dcds'   : v['dcds'],
                'author' : v['author'],
            }
            TESTS_ALL['%s_%s' % (k,p)] = test


if __name__ == '__main__':
    valid_tests = sorted(TESTS_ALL.keys() + ['all'])
    tests = sys.argv[1:]

    # Parse args
    if 'verify' in tests:
        verify = True
        tests.remove('verify')
    else:
        verify = False

    if 'quiet' in tests:
        quiet = lambda x : x + ' > /dev/null'
        tests.remove('quiet')
    else:
        quiet = lambda x : x

    # Limit DCDs tested
    if '--' in tests:
        i = tests.index('--')
        dcds = tests[i+1:]
        tests = tests[:i]
    else:
        dcds = DCDS

    if not tests or not set(tests).issubset(valid_tests):
        print "Valid tests are:\n * " + '\n * '.join(valid_tests)
        sys.exit(1)

    if 'all' in tests:
        tests = TESTS.keys()

    # All tests with perms must be expanded
    tests_run = []
    for test in tests:
        if 'perms' not in TESTS_ALL[test]:
            tests_run.append(test)
            continue
        for p in TESTS_ALL[test]['perms']:
            tests_run.append('%s_%s' % (test, p))
    tests_run = sorted(set(tests_run))

    # Setup Dirs
    os.chdir(WORKING_DIR)
    if not os.path.exists(OUTPUT_DIR):
        os.mkdir(OUTPUT_DIR)

    verify_failed = []

    # Run each test
    for name in tests_run:
        test = TESTS_ALL[name]

        if test['author'] is 'keegan':
            cflag, dflag = '-c', '-d'
        else:
            cflag, dflag = 'c', 'x'

        print '='*len(name)
        print name
        print '='*len(name)

        for dcd in test['dcds']:
            if dcd not in dcds:
                continue

            print '\n', dcd

            dcd_path = '../dcd/%s.dcd' % dcd
            cmp_path = os.path.join(OUTPUT_DIR, '%s_%s.cmp' % (dcd, name))
            dec_path = os.path.join(OUTPUT_DIR, '%s_%s.dcd' % (dcd, name))

            cmp_cmd = '%s %s %s %s' % (test['bin'], cflag, dcd_path, cmp_path)
            os.system(quiet(cmp_cmd))

            if not verify:
                continue

            dec_cmd = '%s %s %s %s' % (test['bin'], dflag, cmp_path, dec_path)
            os.system(quiet(dec_cmd))

            check_cmd = './verifiers/checkdcd %s %s' % (dcd_path, dec_path)
            verified = os.system(check_cmd)

            if verified != 0:
                verify_failed.append('%s_%s' % (dcd, name))
        print

    if verify:
        if verify_failed:
            print "The following tests failed:"
            for test in verify_failed:
                print ' * ' + test
        else:
            print "Every test successfully verified."
