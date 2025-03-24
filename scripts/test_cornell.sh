#!/bin/bash

echo "RUNNING 10"
./raytracing.exe ../tests/randomcornell10.test ../output/randomcornell10_iter_box_depth ../stats/randomcornell10_iter_runtime.txt 0
./raytracing.exe ../tests/randomcornell10.test ../output/randomcornell10_BVH_box_depth ../stats/randomcornell10_BVH_runtime.txt 1
./raytracing.exe ../tests/randomcornell10.test ../output/randomcornell10_LBVH_box_depth ../stats/randomcornell10_LBVH_runtime.txt 2

echo "RUNNING 100"
./raytracing.exe ../tests/randomcornell100.test ../output/randomcornell100_iter_box_depth ../stats/randomcornell100_iter_runtime.txt 0
./raytracing.exe ../tests/randomcornell100.test ../output/randomcornell100_BVH_box_depth ../stats/randomcornell100_BVH_runtime.txt 1
./raytracing.exe ../tests/randomcornell100.test ../output/randomcornell100_LBVH_box_depth ../stats/randomcornell100_LBVH_runtime.txt 2

echo "RUNNING 1000"
./raytracing.exe ../tests/randomcornell1000.test ../output/randomcornell1000_iter_box_depth ../stats/randomcornell1000_iter_runtime.txt 0
./raytracing.exe ../tests/randomcornell1000.test ../output/randomcornell1000_BVH_box_depth ../stats/randomcornell1000_BVH_runtime.txt 1
./raytracing.exe ../tests/randomcornell1000.test ../output/randomcornell1000_LBVH_box_depth ../stats/randomcornell1000_LBVH_runtime.txt 2
