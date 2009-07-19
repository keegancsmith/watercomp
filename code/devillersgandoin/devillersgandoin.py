#!/usr/bin/env python

"""
This is an implementation of Devillers and Gandoin's method of Progressive
Geometric Compression.

This is just an implementation for checking algorithmic correctness. As such
it works on a simple input format, and doesn't use arithmetic coding on the
output. A C implementation will follow that will implement it correctly.

When a file is decompressed, it won't neccessarily list the points in the same
order.
"""

__author__ = "Keegan Carruthers-Smith"


from random import randint


def length_squared(S):
    lengths = (b-a for a,b in S)
    return reduce(lambda a,b : a * b, lengths, 1)


def generate(N=10000, d=3, b=32):
    """
    Generates a random data containing N points. Can contain duplicates. d is
    the dimension of the points and all points are in the range [0,2**32).
    """
    rand_coord = lambda : randint(0, 2**b)
    rand_point = lambda : tuple(rand_coord() for i in xrange(d))
    for i in xrange(N):
        yield rand_point()


def encode(points, d=3, b=32):
    """
    A generator which encodes the points. Direct translation from psuedocode
    from the paper.
    """
    bounding_box = [(0, 2**b)]*d
    points = list(points)

    # L is a queue of segments and the points in that segment. It also
    # contains the current dimension to sort by.
    L = [(bounding_box, points, 0)]
    yield len(points)
    while L:
        S, points, dim = L.pop(0)
        n = len(points)

        mid = (S[dim][0] + S[dim][1]) / 2
        p1 = []
        p2 = []
        for p in points:
            if p[dim] < mid:
                p1.append(p)
            else:
                p2.append(p)

        S1 = list(S)
        S1[dim] = (S[dim][0], mid)
        n1 = len(p1)

        S2 = list(S)
        S2[dim] = (mid, S[dim][1])
        n2 = len(p2)

        yield n1

        dim = (dim + 1) % d

        if n1 > 0 and length_squared(S1) > 1:
            L.append((S1, p1, dim))
        if n2 > 0 and length_squared(S2) > 1:
            L.append((S2, p2, dim))


def decode(data, d=3, b=32):
    """
    A generator which dencodes the encoded points. Direct translation from
    psuedocode from the paper. Points will not be in the same order.
    """
    bounding_box = [(0, 2**b)]*d
    project_1 = lambda x : tuple(a[0] for a in x)
    data = iter(data)
    N = data.next()

    # L is a queue of segments and the points in that segment. It also
    # contains the current dimension to sort by.
    L = [(bounding_box, N, 0)]

    while L:
        S, n, dim = L.pop(0)
        mid = (S[dim][0] + S[dim][1]) / 2

        n1 = data.next()
        n2 = n - n1

        S1 = list(S)
        S1[dim] = (S[dim][0], mid)

        S2 = list(S)
        S2[dim] = (mid, S[dim][1])

        dim = (dim + 1) % d

        if n1 > 0:
            if length_squared(S1) <= 1:
                for i in xrange(n1):
                    yield project_1(S1)
            else:
                L.append((S1, n1, dim))
        if n2 > 0:
            if length_squared(S2) <= 1:
                for i in xrange(n2):
                    yield project_1(S2)
            else:
                L.append((S2, n2, dim))


def test(N=10000, d=3, b=32, show=False):
    if hasattr(N, '__iter__'):
        print "Testing on given data (d=%d, b=%d)" % (d, b)
        data = N
    else:
        print "Testing on random data (N=%d, d=%d, b=%d)" % (N, d, b)
        print "Generating..."
        data = list(generate(N, d, b))

    print "Encoding..."
    enc_data = list(encode(data, d, b))

    print "Decoding..."
    dec_data = list(decode(enc_data, d, b))

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

    # Converts a list of integers to 1d coords
    f = lambda li : [(x,) for x in li]

    run_test(5)
    run_test([(0,0,0)], 3, 32, True)
    run_test(f([0,1,2**32-1]), 1, 32, True)
    run_test(f([0,0,1,1,1,1,2**32-1,2**32-1,2**32-1]), 1, 32, True)
    run_test(f(range(2**4)), 1, 4, True)
    run_test(f(range(2**15)), 1, 15)
    run_test()

    print "Passed %d out of %d" % (run_tests.passed, run_tests.run)


if __name__ == "__main__":
    # 2D Example from paper
    data = [(0,0),(1,1),(0,2),(1,2),(2,2),(1,3),(2,3)]
    enc = list(encode(data, 2, 2))
    dec = decode(enc, 2, 2)
    assert enc == [7, 5, 2, 0, 1, 1, 2, 1, 0, 1, 1, 1]
    assert sorted(dec) == sorted(data)

    run_tests()
