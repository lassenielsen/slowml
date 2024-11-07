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

def ReadCollection(path,fnr):
  res=dict()
  labels=GetLabelDirs(path)
  for label in labels:
    #print('Loading label ' + label)
    lvec=[]
    files=GetVectorFiles(path+'/'+label)
    for file in files:
      #print('  Loading file ' + file)
      fvec=ReadVector(path+'/'+label+'/'+file)
      fres=[]
      if fnr>0:
        parts=file[0:-4].split('_')
        for r in range(fnr):
          if r>=len(parts):
            #print('Error - missing results not in filename')
            fres.append(0.0)
          else:
            fres.append(float(parts[r-fnr]))
      lvec.append((fvec,fres))
    res[label]=lvec
  return res

def ClassVector(classstr,classes):
  res=[]
  for c in classes:
    if c==classstr:
      res.append(1.0)
    else:
      res.append(0.0)
  return res

def ClassUnVector(classvec,classes):
  if len(classvec)<len(classes) or len(classes)<1:
    raise "ClassUnVector: Requires vector of length at least 1"
  res=classes[0]
  minconf=classvec[0]
  for i in range(1,len(classes)):
    if classvec[i]>minconf:
      res=classes[i]
  return res

def SplitVector(vec,length):
  return [ vec[i:i+length] for i in range(0, len(vec), length) ]

def GroupVector(vec):
  res=[]
  for x in range(len(vec)):
    res.append([])
    for y in range(len(vec[x])):
      res[x].append([vec[x][y]])
  return res

