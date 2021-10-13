import numpy as np
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn import decomposition
from sklearn.preprocessing import StandardScaler
from sklearn.utils import shuffle
import sklearn


import keras
from keras.layers import Dense, Dropout, Flatten, Lambda, Activation
from keras import backend as K
from keras.models import Sequential, model_from_json
from keras.layers.core import Dense
from keras.utils import np_utils
import tensorflow as tf

import tfheppy
from tfheppy import Encoder
from tfheppy import Ctxt
from tfheppy import Service

from test import check_accuracy, get_a_b
from test import calc_mse, calc_mses, calc_mse_relative

def train():
  iris = load_iris()
  X = iris.data
  y = iris.target
  X, y = shuffle(X, y, random_state=0)
  scaler = StandardScaler()
  scaler.fit(X)
  X = scaler.transform(X)

  pca = decomposition.PCA(n_components=3)
  pca.fit(X)
  X = pca.transform(X)

  y = np_utils.to_categorical(y)
  #X = sklearn.preprocessing.normalize(X)

  x_train, x_test, y_train, y_test = train_test_split(
      X, y, test_size=0.2)

  model = Sequential()
  model.add(Dense(input_dim=3, units=10, activation='relu'))
  #model.add(Dense(input_dim=20, units=32, activation='relu'))
  model.add(Dense(input_dim=10, units=3))
  model.add(Activation('softmax'))
  model.compile(loss='categorical_crossentropy',
                optimizer=tf.keras.optimizers.Adam(), metrics=['accuracy'])

  history = model.fit(x_train, y_train,
                      batch_size=32, epochs=100,
                      verbose=1,
                      validation_data=(x_test, y_test),
                      )
  model.save("tmp.h5")


if __name__ == "__main__":
  train()
