import os
import glob
count = 0
for filename in glob.glob(r'X:\\HTL\\htl_*'):
	myfile = open(filename)
	length = len(myfile.readlines())
	print(filename,length,sep='---')
	count += length

print("sum", count, "lines code")

x = input("press anykey to continue...")
