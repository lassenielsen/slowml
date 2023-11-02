#!/usr/bin/python3
import tensorflow as tf
import numpy as np
import vector
import sys

print("TensorFlow version: ", tf.__version__)

def split_vector (vec,length):
  return [vec[i:i+length] for i in range(0, len(vec), length)]

def group_vector(vec):
  for x in range(len(vec)):
    for y in range(len(vec[x])):
      vec[x][y]=[vec[x][y]]
  return vec

# Load data
x_test=[]
y_test=[]
collection=vector.ReadCollection(sys.argv[1],0)
for label in collection:
  for (vec,r) in collection[label]:
    arr=group_vector(split_vector(vec[1:],100))
    x_test.append(arr)
    y_test.append(int(label[6:]))

x_test=np.asarray(x_test)
y_test=np.asarray(y_test)
#print(str(x_test))
#print(str(y_test))

# Create neural network
model=tf.keras.models.load_model('models/digits.h5')

# Test network
model.evaluate(x_test, y_test, verbose=2)
print(str(model(x_test[0]))+' compared to '+str(y_test[0]))

