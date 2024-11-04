#!/usr/bin/python3
import sys

if len(sys.argv)<3:
  print('Usage: '+sys.argv[0]+' <training data folder> <output model>')
  exit(1)

import tensorflow as tf
import numpy as np
import vector

print("Using TensorFlow version: ", tf.__version__)

epochs=10
model=[]
dest='popml.mod'

args=sys.argv[1:-1]
while len(args)>0:
  arg=args[0]
  args=args[1:]
  if arg in ['-e','--epochs'] and len(args)>0:
    epochs=int(args[0])
    args=args[1:]
  elif arg in ['-m','--model'] and len(args)>0:
    model=tf.keras.models.load_model(args[0])
    args=args[1:]
  elif arg in ['-n','--network'] and len(args)>0:
    network=[]
    layers=args[0].split(',')
    args=args[1:]
    for layer in layers:
      if layer[0:6]=='Dense(':
        nodes=int(layer[6:-1])
        network.append(tf.keras.layers.Dense(nodes,activation='relu'))
      else:
        print('Need support for layer: '+str(layer))
    model = tf.keras.models.Sequential(network)
  elif arg in ['-o','--output'] and len(args)>0:
    dest=args[0]
    args=args[1:]
  else:
    print('Unknown option '+str(arg))


# Load data
data_inputs=[]
data_outputs=[]
collection=vector.ReadCollection(sys.argv[-1],0)
for label in collection:
  for v,r in collection[label]:
    data_inputs.append(v[1:])
    res=vector.ClassVector(label[6:],['0','1','2','3','4','5','6','7','8','9'])
    res.extend(r)
    data_outputs.append(res)

# Configure neural network training
loss_fn = tf.keras.losses.MeanSquaredError() #SquaredHinge() #SparseCategoricalCrossentropy(from_logits=True)

model.compile(optimizer='adam',
              loss=loss_fn,
              metrics=['accuracy'])

# Train network
model.fit(data_inputs, data_outputs, epochs=epochs)
model.save(dest)
