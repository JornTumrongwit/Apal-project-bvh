#!/bin/bash

for i in $(seq 0 8); do
    echo "RUNNING ${i}"
    ./raytracing.exe ../tests/random100000.test ../output/random_l${i} ../stats/random_l${i}_iter_runtime.txt 2 ${i}
done
