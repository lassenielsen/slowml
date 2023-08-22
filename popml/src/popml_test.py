#!/usr/bin/python3
import tensorflow as tf
import numpy as np
import popml_vector
import sys

print("TensorFlow version: ", tf.__version__)

def split_vector (vec,length):
  return [vec[i:i+length] for i in range(0, len(vec), length)]

datapath='./data'
width=100
modelfile='./model.smartml'

if len(sys.argv)==1 or len(sys.argv)==2 and sys.argv[1]=='--help':
  print('Syntax is '+sys.argv[0]+' [--width|-w <width=100>] [--model|-m <modelpath=./model.smartml>] <datapath=./data>')
  exit

i=1
while i<len(sys.argv):
  if i+1<len(sys.argv) and (sys.argv[i]=='--width' or sys.argv[i]=='-w'):
    width=int(sys.argv[i+1])
    i=i+2
  elif i+1<len(sys.argv) and (sys.argv[i]=='--model' or sys.argv[i]=='-m'):
    modelfile=sys.argv[i+1]
    i=i+2
  elif i+1==len(sys.argv):
    datapath=sys.argv[i]
    i=i+1
  else:
    print('Unknown argument: '+sys.argv[i])
    i=i+1

print('Config datapath='+datapath)
print('Config width='+str(width))
print('Config modelfile='+modelfile)

# Load data
x_test=[]
y_test=[]
collection=popml_vector.ReadCollection(datapath)
for label in collection:
  for vec in collection[label]:
    arr=split_vector(vec[1:],width)
    x_test.append(arr)
    y_test.append(int(label[6:]))

x_test=np.asarray(x_test)
y_test=np.asarray(y_test)
#print(str(x_test))
#print(str(y_test))

# Create neural network
model=tf.keras.models.load_model(modelfile)

# Test network
model.evaluate(x_test, y_test, verbose=2)

