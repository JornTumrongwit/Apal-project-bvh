#!/bin/bash

echo "RUNNING 10"
./raytracing.exe ../tests/random10.test ../output/random10_iter_box_depth ../stats/random10_iter_runtime.txt 0
./raytracing.exe ../tests/random10.test ../output/random10_bvh_box_depth ../stats/random10_bvh_runtime.txt 1
./raytracing.exe ../tests/random10.test ../output/random10_lbvh_box_depth ../stats/random10_lbvh_runtime.txt 2

echo "RUNNING 100"
./raytracing.exe ../tests/random100.test ../output/random100_iter_box_depth ../stats/random100_iter_runtime.txt 0
./raytracing.exe ../tests/random100.test ../output/random100_bvh_box_depth ../stats/random100_bvh_runtime.txt 1
./raytracing.exe ../tests/random100.test ../output/random100_lbvh_box_depth ../stats/random100_lbvh_runtime.txt 2

echo "RUNNING 1000"
./raytracing.exe ../tests/random1000.test ../output/random1000_iter_box_depth ../stats/random1000_iter_runtime.txt 0
./raytracing.exe ../tests/random1000.test ../output/random1000_bvh_box_depth ../stats/random1000_bvh_runtime.txt 1
./raytracing.exe ../tests/random1000.test ../output/random1000_lbvh_box_depth ../stats/random1000_lbvh_runtime.txt 2

echo "RUNNING 10000"
./raytracing.exe ../tests/random10000.test ../output/random10000_iter_box_depth ../stats/random10000_iter_runtime.txt 0
./raytracing.exe ../tests/random10000.test ../output/random10000_bvh_box_depth ../stats/random10000_bvh_runtime.txt 1
./raytracing.exe ../tests/random10000.test ../output/random10000_lbvh_box_depth ../stats/random10000_lbvh_runtime.txt 2

echo "RUNNING 100000"
./raytracing.exe ../tests/random100000.test ../output/random100000_lbvh_box_depth ../stats/random100000_bvh_runtime.txt 1
./raytracing.exe ../tests/random100000.test ../output/random100000_lbvh_box_depth ../stats/random100000_lbvh_runtime.txt 2
