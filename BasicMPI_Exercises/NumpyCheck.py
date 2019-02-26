import numpy as np

def Norm(filename):
    f = open(filename,"r")
    length = int(f.readline().strip())
    array = np.empty(length)
    for i in range(length):
        array[i] = f.readline().strip()
    norm = np.linalg.norm(array)
    return norm


norm = Norm("vec16.txt")
ss = norm**2


print("The norm is %.6f"%norm)
print("The Sum of the Squares is %.6f"%ss)
                                           
