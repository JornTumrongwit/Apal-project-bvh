#!/bin/bash

echo "RUNNING DRAGON"
./raytracing.exe ../tests/dragon.test ../output/dragon_iter_box_depth ../stats/dragon_iter_runtime.txt 0
./raytracing.exe ../tests/dragon.test ../output/dragon_BVH_box_depth ../stats/dragon_BVH_runtime.txt 1
./raytracing.exe ../tests/dragon.test ../output/dragon_LBVH_box_depth ../stats/dragon_LBVH_runtime.txt 2
