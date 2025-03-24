import random

header = '''#Cornell Box
size 640 480
camera 0 0 1 0 0 -1 0 1 0 45
output scene6.png

maxdepth 5



maxverts 12
#planar face
vertex -1 +1 0
vertex -1 -1 0
vertex +1 -1 0
vertex +1 +1 0

#cube
vertex -1 +1 +1
vertex +1 +1 +1
vertex -1 -1 +1
vertex +1 -1 +1

vertex -1 +1 -1
vertex +1 +1 -1
vertex -1 -1 -1
vertex +1 -1 -1


ambient 0 0 0
specular 0 0 0
shininess 1
emission 0 0 0
diffuse 0 0 0

attenuation 1 0.1 0.05

point 0 0.44 -1.5 0.8 0.8 0.8
point 0 0 1.5 0.8 0.8 0.8
directional 0 1 -1 0.2 0.2 0.2

diffuse 0 0 1
#sphere 0 0.8 -1.5 0.1


pushTransform

#red
pushTransform
translate 0 0 -3
rotate 0 1 0 60
scale 10 10 1
diffuse 1 0 0
tri 0 1 2
tri 0 2 3
popTransform

#green
pushTransform
translate 0 0 -3
rotate 0 1 0 -60
scale 10 10 1
diffuse 0 1 0
tri 0 1 2
tri 0 2 3
popTransform

#back
pushTransform
scale 10 10 1
translate 0 0 -2
diffuse 1 1 1
tri 0 1 2
tri 0 2 3
popTransform

#top
pushTransform
translate 0 0.5 0
rotate 1 0 0 60
translate 0 0 -1
scale 10 10 1
diffuse 1 1 1
tri 0 1 2
tri 0 2 3
popTransform

#bottom
pushTransform
translate 0 -0.5 0
rotate 1 0 0 -60
translate 0 0 -1
scale 10 10 1
diffuse 1 1 1
tri 0 1 2
tri 0 2 3
popTransform

'''

cube_starter = '''
diffuse 0.5 0.7 0.2
specular 0.2 0.2 0.2
pushTransform
'''

cube_ender = '''
diffuse 1 1 1

tri 4 6 5
tri 6 7 5
tri 4 5 8
tri 5 9 8
tri 7 9 5
tri 7 11 9
tri 4 8 10
tri 4 10 6
tri 6 10 11
tri 6 11 7
tri 10 8 9
tri 10 9 11
popTransform

'''
file_str = header
for f in range(3):
    for i in range(10**(f+1)):
        t = [random.uniform(-0.5, 0.5), random.uniform(-0.5, 0), random.uniform(0, -2)]
        s = [random.uniform(0.01, 0.2), random.uniform(0.01, 0.2), random.uniform(0.01, 0.2)]
        r = random.randint(0, 360)
        file_str += cube_starter + '''translate {} {} {}
rotate 0 1 0 {}
scale {} {} {}'''.format(
            t[0], t[1], t[2],
            r,
            s[0], s[1], s[2]
        ) + cube_ender

    with open("./tests/randomCornell{}.test".format(10**(f+1)), "w") as file:
        file.write(file_str)