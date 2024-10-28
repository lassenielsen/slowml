#!/usr/bin/python3
import tensorflow as tf
import numpy as np
import vector
import sys

print("TensorFlow version: ", tf.__version__)

def split_vector(vec,length):
  return [ vec[i:i+length] for i in range(0, len(vec), length) ]

def group_vector(vec):
  res=[]
  for x in range(len(vec)):
    res.append([])
    for y in range(len(vec[x])):
      res[x].append([vec[x][y]])
  return res

# Load data
x_test=[]
y_test=[]
collection=vector.ReadCollection(sys.argv[1],0)
for label in collection:
  for v,r in collection[label]:
    img=split_vector(v[1:],28)
    #arr=group_vector(img)
    x_test.append(img)
    res=vector.ClassVector(label[6:],['0','1','2','3','4','5','6','7','8','9'])
    res.extend(r)
    y_test.append(res)

x_test=np.asarray(x_test)
y_test=np.asarray(y_test)
#print(str(x_test))
#print(str(y_test))

# Create neural network
model=tf.keras.models.load_model('models/digits.h5')

# Test network
model.evaluate(x_test, y_test, verbose=2)
print(str(model(x_test[0]))+' compared to '+str(y_test[0]))

