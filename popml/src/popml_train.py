#!/usr/bin/python3
import tensorflow as tf
import numpy as np
import popml_vector
import sys

print("TensorFlow version: ", tf.__version__)

def split_vector (vec,length):
  return [vec[i:i+length] for i in range(0, len(vec), length)]

# Settings
datapath='./data'
width=100
modelfile='./model.smartml'
epochs=5

if len(sys.argv)==1 or len(sys.argv)==2 and sys.argv[1]=='--help':
  print('Syntax is '+sys.argv[0]+' [--epochs|-e <int=5>] [--width|-w <width=100>] [--output|-o <modelpath=./model.smartml>] <datapath=./data>')
  exit

i=1
while i<len(sys.argv):
  if i+1<len(sys.argv) and (sys.argv[i]=='--epochs' or sys.argv[i]=='-e'):
    epochs=int(sys.argv[i+1])
    i=i+2
  elif i+1<len(sys.argv) and (sys.argv[i]=='--width' or sys.argv[i]=='-w'):
    width=int(sys.argv[i+1])
    i=i+2
  elif i+1<len(sys.argv) and (sys.argv[i]=='--output' or sys.argv[i]=='-o'):
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
print('Config epochs='+str(epochs))
# Load data
data_inputs=[]
data_outputs=[]
collection=popml_vector.ReadCollection(datapath)
for label in collection:
  for vec in collection[label]:
    arr=split_vector(vec[1:],width)
    data_inputs.append(arr)
    data_outputs.append(int(label[6:]))

data_inputs=np.asarray(data_inputs)
data_outputs=np.asarray(data_outputs)

# Create neural network
model = tf.keras.models.Sequential([
  tf.keras.layers.Flatten(input_shape=(width, len(data_inputs[0]))),
  tf.keras.layers.Dense(128, activation='relu'),
  tf.keras.layers.Dropout(0.2),
  tf.keras.layers.Dense(len(collection))
])

# Configure neural network training
loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)

model.compile(optimizer='adam',
              loss=loss_fn,
              metrics=['accuracy'])

# Train network
model.fit(data_inputs, data_outputs, epochs=epochs)

model.save(modelfile)
