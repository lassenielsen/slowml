#!/usr/bin/python3
import struct
import sys
import copy
def readvec(fname):
  fin=open(fname,'rb')
  fcount=struct.unpack('Q',fin.read(8))[0]
  res=[]
  for x in range(fcount):
    f=struct.unpack('d',fin.read(8))[0]
    res.append(f)
  fin.close()
  return res

def writevec(fname,vec):
  fout=open(fname,'wb')
  fout.write(struct.pack('Q',len(vec)))
  for f in vec:
    fout.write(struct.pack('d',f))
  fout.close()
  return

if len(sys.argv)<10:
  print("Syntax: "+sys.argv[0]+" <background vector> <background width> <object vector> <object width> <posx ratio> <posy ratio> <width ratio> <height ratio> <destination vector>")
  quit()

bg=readvec(sys.argv[1])
bgw=int(sys.argv[2])
bgh=int((len(bg)-1)/bgw)

obj=readvec(sys.argv[3])
objw=int(sys.argv[4])
objh=int((len(obj)-1)/objw)

posx=float(sys.argv[5])
posy=float(sys.argv[6])

sizex=float(sys.argv[7])
sizey=float(sys.argv[8])

dst=copy.deepcopy(bg)
for px in range(int(float(bgw)*sizex)):
  for py in range(int(float(bgh)*sizey)):
    x=int(posx*bgw+px)
    y=int(posy*bgh+py)
    objx=int(float(px)*float(objw)/(float(bgw)*sizex))
    objy=int(float(py)*float(objh)/(float(bgh)*sizey))
    dst[1+x+y*bgw]=dst[1+x+y*bgw]+obj[1+objx+objy*objw]/2.0
  if dst[1+x+y*objw]>1.0:
    dst[1+x+y*objw]=dst[1+x+y*objw]%1.0

writevec(sys.argv[9],dst)

print('Done')
