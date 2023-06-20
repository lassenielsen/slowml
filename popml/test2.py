#!/usr/bin/python3
import tensorflow as tf
import numpy as np
import vector
import sys

print("TensorFlow version: ", tf.__version__)

def split_vector (vec,length):
  return [vec[i:i+length] for i in range(0, len(vec), length)]

# Load data
data_inputs=[]
data_outputs=[]
collection=vector.ReadCollection(sys.argv[1])
for label in collection:
  for vec in collection[label]:
    arr=split_vector(vec[1:],28)
    data_inputs.append(arr)
    data_outputs.append(int(label[6:]))

data_inputs=np.asarray(data_inputs)
data_outputs=np.asarray(data_outputs)

# Create neural network
model = tf.keras.models.Sequential([
  tf.keras.layers.Flatten(input_shape=(28, 28)),
  tf.keras.layers.Dense(128, activation='relu'),
  tf.keras.layers.Dropout(0.2),
  tf.keras.layers.Dense(10)
])

# Configure neural network training
loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)

model.compile(optimizer='adam',
              loss=loss_fn,
              metrics=['accuracy'])

# Train network
model.fit(data_inputs, data_outputs, epochs=5)

model.save('./models/digits.h5')
