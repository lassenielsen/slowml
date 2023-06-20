#!/usr/bin/python3
import tensorflow as tf
import numpy as np

print("TensorFlow version: ", tf.__version__)

# Load data
mnist = tf.keras.datasets.mnist
(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train, x_test = x_train / 255.0, x_test / 255.0

print(str(x_test))
def load_vector(fname):
  f=open(fname,'rb')
  s=f.read(8)
  count=struct.unpack('Q',s)
  print('Number of values in data: '+str(count))
  data=[]
  for x in range(count):
    s=f.read(8)
    data.append(struct.unpack('d',s))
  f.close()
  return data

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
model.fit(x_train, y_train, epochs=5)

# Test network
model.evaluate(x_test,  y_test, verbose=2)

model.save('./models/digits.h5')
