#!/usr/bin/python3
import sys

if len(sys.argv)<3:
  print('Usage: '+sys.argv[0]+' <model> <testdata folder>')
  exit(1)

import tensorflow as tf
import numpy as np
import vector
import random

print("TensorFlow version: ", tf.__version__)

# Load data
x_test=[]
y_test=[]
collection=vector.ReadCollection(sys.argv[2],0)
for label in collection:
  for v,r in collection[label]:
    #img=vector.SplitVector(v[1:],28)
    #arr=vector.GroupVector(img)
    #x_test.append(arr)
    x_test.append(v[1:])
    res=vector.ClassVector(label[6:],['0','1','2','3','4','5','6','7','8','9'])
    res.extend(r)
    y_test.append(res)

# Create neural network
model=tf.keras.models.load_model(sys.argv[1])

# Test network
#model.evaluate(x_test, y_test, verbose=2)
res_success=0
res_error=0
result=model.predict(x_test)
for i in range(len(result)):
  if vector.ClassUnVector(result[i],range(10)) == vector.ClassUnVector(y_test[i],range(10)):
    print('Success on file ... with result '+str(vector.ClassUnVector(result[i],range(10))))
    res_success+=1;
  else:
    print('Error on file ... with result '+str(vector.ClassUnVector(result[i],range(10))))
    res_error+=1;
print('Total '+str(res_success)+' succeeded ('+str(100.0*res_success/(res_success+res_error))+'%)')
print('Total '+str(res_error)+' failed ('+str(100.0*res_error/(res_success+res_error))+'%)')
