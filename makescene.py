import random

triamt = 1000

header = '''size 640 480
camera 0 0 1 0 0 -1 0 1 0 45

maxdepth 5

ambient 0 0 0
specular 0 0 0
shininess 1
emission 0 0 0
diffuse 0 0 0

attenuation 1 0.1 0.05

point 0 0.44 -1.5 0.8 0.8 0.8
directional 0 1 -1 0.2 0.2 0.2

'''

vertex = ""
face = ""
vert_amt = 1
for tri in range(triamt):
    baseline = [random.uniform(-10, 10), random.uniform(-10, 10), random.uniform(-10, -9)]
    for i in range(3):
        vertex += "vertex {} {} {}\n".format(baseline[0] + random.uniform(-0.01, 0.01), baseline[1] + random.uniform(-0.01, 0.01), baseline[2] + random.uniform(-0.01, 0.01))
    vert_amt += 3
    selected_numbers = random.sample(range(0, vert_amt), 3)
    face += "ambient {} {} {}\n".format(random.uniform(0, 1), random.uniform(0, 1), random.uniform(0, 1))
    face+= "tri {} {} {}\n".format(selected_numbers[0], selected_numbers[1], selected_numbers[2])

file_str = header + vertex + face

with open("./tests/random.test", "w") as file:
    file.write(file_str)