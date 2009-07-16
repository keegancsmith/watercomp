#!/usr/bin/env python

"""
This is an implementation of Devillers and Gandoin's method of Progressive
Geometric Compression.

This is just an implementation for checking algorithmic correctness. As such
it works on a simple input format, and doesn't use arithmetic coding on the
output. A C implementation will follow that will implement it correctly.

The input format is as follows: The first line contains N and b, where N is
the number of points. The following N lines each contain 3 space seperated
integers numbers, representing a point's (x, y, z) co-ordinate. Each
co-ordinate is in the range [0, 2^b)

When a file is decompressed, it won't neccessarily list the points in the same
order.
"""

__author__ = "Keegan Carruthers-Smith"


from random import randint
from bisect import bisect_left


def generate(N=10000, b=32):
    """
    Generates a random data containing N points. Can contain duplicates.
    """
    rand_coord = lambda : randint(0, 2**b)
    #rand_point = lambda : tuple(randint() for i in xrange(3))
    rand_point = rand_coord
    for i in xrange(N):
        yield rand_point()


def encode(points, b):
    """
    A generator which encodes the points. Direct translation from psuedocode
    from the paper.
    """
    points = sorted(points)
    length = lambda S : S[1] - S[0]

    # L is a queue of segments and the index range of points in that
    # segment. The initial segment is [0, 2**b) and contains all the points in
    # index range [0, len(points))
    L = [((0, 2**b), (0, len(points)))]
    yield len(points)
    while L:
        S, S_i = L.pop(0)
        n = S_i[1] - S_i[0]

        mid = (S[0] + S[1]) / 2
        i = bisect_left(points, mid, S_i[0], S_i[1])

        S1 = (S[0], mid)
        n1 = i - S_i[0]

        S2 = (mid, S[1])
        n2 = S_i[1] - i

        yield n1

        if n1 > 0 and length(S1) > 1:
            L.append((S1, (S_i[0], i)))
        if n2 > 0 and length(S2) > 1:
            L.append((S2, (i, S_i[1])))


def decode(data, b):
    """
    A generator which dencodes the encoded points. Direct translation from
    psuedocode from the paper. Points will not be in the same order.
    """
    data = iter(data)
    N = data.next()
    length = lambda S : S[1] - S[0]

    # L is a queue of segments and the index range of points in that
    # segment. The initial segment is [0, 2**b) and contains all the points in
    # index range [0, N)
    L = [((0, 2**b), N)]

    while L:
        S, n = L.pop(0)
        mid = (S[0] + S[1]) / 2

        n1 = data.next()
        n2 = n - n1

        if n1 > 0:
            S1 = (S[0], mid)
            if length(S1) <= 1:
                for i in xrange(n1):
                    yield S[0]
            else:
                L.append((S1, n1))
        if n2 > 0:
            S2 = (mid, S[1])
            if length(S2) <= 1:
                for i in xrange(n2):
                    yield mid
            else:
                L.append((S2, n2))


def test(N=10000, b=32, show=False):
    if hasattr(N, '__iter__'):
        print "Testing on given data (b=%d)" % b
        data = N
    else:
        print "Testing on random data (N=%d, b=%d)" % (N, b)
        print "Generating..."
        data = list(generate(N, b))

    print "Encoding..."
    enc_data = list(encode(data, b))

    print "Decoding..."
    dec_data = list(decode(enc_data, b))

    if show:
        f = lambda li : ' '.join(map(str, sorted(li)))
        print f(data)
        print f(dec_data)

    if sorted(data) == sorted(dec_data):
        print "PASS"
        return True
    else:
        print "FAIL"
        return False


def run_tests():
    run_tests.passed = 0
    run_tests.run = 0
    def run_test(*args, **kwargs):
        result = test(*args, **kwargs)
        print
        if result:
            run_tests.passed += 1
        run_tests.run += 1
        
    run_test(5, show=True)
    run_test([0], 32, True)
    run_test([0,1,2**32-1], 32, True)
    run_test([0,0,1,1,1,1,2**32-1,2**32-1,2**32-1], 32, True)
    run_test(xrange(2**4), 4, True)
    run_test(xrange(2**15), 15)
    run_test()

    print "Passed %d out of %d" % (run_tests.passed, run_tests.run)


if __name__ == "__main__":
    run_tests()
