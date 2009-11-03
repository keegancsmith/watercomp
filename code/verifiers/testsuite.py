#!/usr/bin/env python

import cPickle as pickle
import glob
import itertools
import os
import os.path
import pprint
import sys
import time

RESULTS      = os.path.expanduser('~/dcd_results')
QUANTISATION = '8 12 16'.split()
PERMUTATIONS = 'null best naive interframe delta'.split()
CMD_PATH     = os.path.abspath(sys.argv[0])
WORKING_DIR  = os.path.dirname(os.path.dirname(CMD_PATH))
INPUT_DIR    = os.path.expanduser('~/dcd_input')
DCDS         = [os.path.splitext(os.path.basename(a))[0]
                for a in glob.glob(INPUT_DIR + '/*.dcd')]

TESTS = {
    'dg': {
        'bin'    : './devillersgandoin/dgcomp',
        'perms'  : PERMUTATIONS,
        'author' : 'keegan',
    },

    'gumhold': {
        'bin'    : './gumhold/gumhold',
        'perms'  : PERMUTATIONS,
        'author' : 'keegan',
    },

    # 'omeltchenko': {
    #     'bin'    : './omeltchenko/driver',
    #     'author' : 'julian',
    # },

    'quant': {
        'bin'    : './quantcomp/quantcomp',
        'author' : 'keegan',
    },

    'gzip': {
        'bin'    : './quantcomp/gzipcomp',
        'author' : 'keegan',
    },

    'watercomp': {
        'bin'    : './watercomp/watercomp',
        'perms'  : PERMUTATIONS,
        'author' : 'keegan',
    },
}


# If a test supports permutations, add all versions of the test with different
# permutations
TESTS_ALL = {}
for k, v in TESTS.items():
    TESTS_ALL[k] = v
    if 'perms' in v:
        v2 = dict(v)
        del v2['perms']
        for p in v['perms']:
            test = {
                'bin'    : 'PERMUTATION="%s" %s' % (p, v['bin']),
                'author' : v['author'],
            }
            TESTS_ALL['%s_%s' % (k,p)] = test


def get_results():
    f = open(RESULTS + '.dat')
    r = pickle.load(f)
    f.close()
    return r

def write_results(r):
    f = open(RESULTS + '.dat', 'w')
    pickle.dump(r, f)
    f.close()
    f = open(RESULTS + '.txt', 'w')
    pprint.pprint(r, f, 4)
    f.close()

def write_result(dcd, quant, comp, size, _time):
    r = get_results()
    r[(dcd, quant, comp)] = (size, _time)
    write_results(r)

if not os.path.exists(RESULTS + '.dat'):
    write_results({})

if __name__ == '__main__':
    valid_tests = sorted(TESTS_ALL.keys() + ['all', 'untested'])
    tests = sys.argv[1:]

    # Parse args
    if 'verify' in tests:
        verify = True
        tests.remove('verify')
    else:
        verify = False

    if 'untested' in tests:
        untested = True
        tests.remove('untested')
    else:
        untested = False

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
    dcds = sorted(dcds)

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

    os.chdir(WORKING_DIR)
    tested = set(get_results().keys())
    verify_failed = []

    # Run each test
    for name, dcd, quant in itertools.product(tests_run, dcds, QUANTISATION):
        test = TESTS_ALL[name]

        if test['author'] is 'keegan':
            cflag, dflag = '-c', '-d'
        else:
            cflag, dflag = 'c', 'x'

        if untested and (dcd, quant, name) in tested:
            continue

        tag = ' '.join([dcd, quant, name])
        print '\n', '*' * len(tag)
        print tag
        print '*' * len(tag), '\n'

        pdb_path = '%s/%s.pdb' % (INPUT_DIR, dcd)
        dcd_path = '%s/%s.dcd' % (INPUT_DIR, dcd)
        cmp_path = '/tmp/foo.cmp'
        dec_path = '/tmp/foo.dcd'

        flags = ['-q ' + quant]
        if os.path.exists(pdb_path):
            flags.append('--pdb ' + pdb_path)
        flags = ' '.join(flags)

        cmp_cmd = '%s %s %s %s %s' % (test['bin'], cflag, flags,
                                      dcd_path, cmp_path)

        t = time.time()
        os.system(quiet(cmp_cmd))
        t = time.time() - t
        s = os.stat(cmp_path).st_size

        # Give 5 seconds for the user to cancel
        try:
            time.sleep(5)
        except:
            sys.exit(0)

        write_result(dcd, quant, name, s, t)

        if not verify:
            continue

        dec_cmd = '%s %s %s %s %s' % (test['bin'], dflag, flags,
                                      cmp_path, dec_path)
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
