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

from test import check_accuracy

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
  model.add(Dense(input_dim=3, units=100, activation='relu'))
  model.add(Dense(input_dim=100, units=32, activation='relu'))
  model.add(Dense(input_dim=32, units=3))
  model.add(Activation('softmax'))
  model.compile(loss='categorical_crossentropy',
                optimizer=tf.keras.optimizers.Adam(), metrics=['accuracy'])

  history = model.fit(x_train, y_train,
                      batch_size=32, epochs=40,
                      verbose=1,
                      validation_data=(x_test, y_test),
                      )
  model.save("tmp.h5")


def gen_luts(mean, std, service):
  assert len(mean) == len(std)
  luts = []
  mins = []
  maxs = []

  for i in range(mean.shape[0]):
    ctv = service.custom_test_vector_args()

    tfd_ctv = np.zeros_like(ctv)
    for j, v in enumerate(ctv):
        tfd_ctv[j] = (v - mean[i])/std[i]
    
    #tfd_encoder = Encoder(np.min(tfd_ctv)-0.1, np.max(tfd_ctv)+0.1, 32)
    mins.append(np.min(tfd_ctv)-0.1)
    maxs.append(np.max(tfd_ctv)+0.1)

    luts.append(tfd_ctv)
  tfd_encoder = Encoder(np.min(mins), np.max(maxs), 32)

  return luts, tfd_encoder


def run_standard_cipher(luts, cs, tfd_encoder, ser):
  assert len(luts) == len(cs)
  res = []
  luts = np.array(luts)
  for i in range(len(cs)):
    r = ser.run_custom_test_vector(x=cs[i], custom_test_vector=luts[i], encoder_target=tfd_encoder)
    res.append(r)
  return res



def run_standard_raw(mean, std, xs):
  assert len(mean) == len(std)
  assert len(mean) == len(xs)
    
  res_list = []
  for i in range(len(xs)):
    res = (xs[i] - mean[i])/std[i]
    res_list.append(res)

  return res_list

def train_standard(X):
  scaler = StandardScaler()
  scaler.fit(X)

  return scaler.mean_, np.sqrt(scaler.var_)



if __name__ == "__main__":
  print("hello, world")

  iris = load_iris()
  X,y = iris.data, iris.target
  X, y = shuffle(X, y, random_state=0)
  mean, std = train_standard(X)

  test_x = X[:10]
  test_y = X[:10]

  encoder1= Encoder(np.min(X)-0.1, np.max(X)+0.1, 32)
  #encoder1.print()

  ser1= Service(encoder1)
  ser1.gen_keys()
  ser2= ser1

  luts, tfd_encoder = gen_luts(mean, std, ser1)

  cs2d = [ser1.encode_and_encrypt_vector(el) for el in test_x]

  res_list = [run_standard_cipher(luts, el, tfd_encoder, ser1) for el in cs2d]

  ser2.set_encoder(tfd_encoder)

  dec2d = [ser2.decrypt_and_decode_vector(el) for el in res_list]

  res_raw_list = [run_standard_raw(mean, std, el) for el in test_x]

  accs = []
  for i in range(len(test_x)):
    acc = check_accuracy(np.array(dec2d[i]), np.array(res_raw_list[i]), ser2.get_encoder().get_half_d())
    accs.append(acc)
  print(f"accs: {accs}")

