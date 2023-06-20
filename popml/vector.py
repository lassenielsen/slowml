#!/usr/bin/python3
import sys
import os
import struct

def GetLabelDirs(path):
  res=[]
  elts=os.listdir(path)
  for elt in elts:
    if elt[:6]=='label_' and os.path.isdir(path+'/'+elt):
      res.append(elt)

  res.sort()
  return res

def GetVectorFiles(path):
  res=[]
  elts=os.listdir(path)
  for elt in elts:
    if len(elt)>4 and os.path.isfile(path+'/'+elt) and elt[-4:]=='.vec':
      res.append(elt)

  return res

def ReadVector(fname):
  fin=open(fname,'rb')
  fcount=struct.unpack('Q',fin.read(8))[0]
  res=[]
  for x in range(fcount):
    f=struct.unpack('d',fin.read(8))[0]
    res.append(f)
  fin.close()
  return res

def ReadCollection(path):
  res=dict()
  labels=GetLabelDirs(path)
  for label in labels:
    print('Loading label ' + label)
    lres=[]
    files=GetVectorFiles(path+'/'+label)
    for file in files:
      print('  Loading file ' + file)
      vec=ReadVector(path+'/'+label+'/'+file)
      lres.append(vec)
    res[label]=lres
  return res
