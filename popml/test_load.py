#!/usr/bin/python3
import tensorflow as tf
import numpy as np

print("TensorFlow version: ", tf.__version__)

# Load data
mnist = tf.keras.datasets.mnist
(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train, x_test = x_train / 255.0, x_test / 255.0

# Create neural network
model=tf.keras.models.load_model('models/digits.h5')

# Test network
model.evaluate(x_test,  y_test, verbose=2)

