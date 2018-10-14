#!/usr/bin/python3
import numpy as np
import matplotlib.pyplot as pyplt
import sys

datas = np.fromfile("mandelbrot_pts.dat",dtype='int32')
datas = datas.reshape((int(sys.argv[1]),-1))%16
pyplt.figure(figsize=(8,6),dpi=150)
#pyplt.jet()
#pyplt.imshow(datas,extent=[-1,1,-1,1])
pyplt.contourf(datas)
pyplt.show()

