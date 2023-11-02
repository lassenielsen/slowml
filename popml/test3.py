#!/usr/bin/python3
import tensorflow as tf
import numpy as np
import vector
import sys

print("TensorFlow version: ", tf.__version__)

def split_vector(vec,length):
  return [vec[i:i+length] for i in range(0, len(vec), length)]

def group_vector(vec):
  for x in range(len(vec)):
    for y in range(len(vec[x])):
      vec[x][y]=[vec[x][y]]
  return vec

# Load data
data_inputs=[]
data_outputs=[]
collection=vector.ReadCollection(sys.argv[1],0)
for label in collection:
  for (vec,r) in collection[label]:
    arr=group_vector(split_vector(vec[1:],100))
    data_inputs.append(arr)
    data_outputs.append(int(label[6:]))

data_inputs=np.asarray(data_inputs)
data_outputs=np.asarray(data_outputs)

# Create neural network
model = tf.keras.models.Sequential([
  #tf.keras.layers.Flatten(input_shape=(100, 100)),
  tf.keras.layers.Conv2D(100,(5,5), activation='relu',input_shape=(100,100,1),kernel_regularizer=tf.keras.regularizers.L2(0.001)), # 
  #tf.keras.layers.MaxPooling2D((2,2)),
  #tf.keras.layers.Conv2D(100,(5,5), activation='relu'),
  tf.keras.layers.Flatten(),
  tf.keras.layers.Dense(200,activation='relu',kernel_regularizer=tf.keras.regularizers.L2(0.001)),
  #tf.keras.layers.Dropout(0.2),
  tf.keras.layers.Dense(10)
])

# Configure neural network training
loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)

model.compile(optimizer='adam',
              loss=loss_fn,
              metrics=['accuracy'])

# Train network
model.fit(data_inputs, data_outputs, epochs=100)

model.save('./models/digits.h5')
