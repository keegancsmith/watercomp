#!/bin/bash

make test_point_compression

./test_point_compression gen $1 points.raw

time ./test_point_compression enc points.raw points.enc
time ./test_point_compression dec points.enc points.dec

echo
./test_point_compression same points.raw points.dec
