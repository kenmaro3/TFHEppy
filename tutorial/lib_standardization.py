import numpy as np
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn import decomposition
from sklearn.preprocessing import StandardScaler
from sklearn.utils import shuffle
import sklearn

import tfheppy
from tfheppy import Encoder, Ctxt, Service

from test import check_accuracy, get_a_b
from test import calc_mse, calc_mses, calc_mse_relative

import lib_iris as liris

def get_luts(mean, std, service):
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

  a, b = get_a_b(mins+maxs)
  #tfd_encoder = Encoder(np.min(mins), np.max(maxs), 32)
  tfd_encoder = Encoder(a, b, 32)

  return luts, tfd_encoder


def run(luts, cs, tfd_encoder, ser):
  assert len(luts) == len(cs)
  res = []
  luts = np.array(luts)
  for i in range(len(cs)):
    r = ser.run_custom_test_vector(x=cs[i], custom_test_vector=luts[i], encoder_target=tfd_encoder)
    res.append(r)
  return res



def run_raw(mean, std, xs):
  assert len(mean) == len(std)
  assert len(mean) == len(xs)
    
  res_list = []
  for i in range(len(xs)):
    res = (xs[i] - mean[i])/std[i]
    res_list.append(res)

  return res_list


def train(X):
  scaler = StandardScaler()
  scaler.fit(X)

  return scaler.mean_, np.sqrt(scaler.var_)



if __name__ == "__main__":
  print("hello, world")

  X, y = liris.load_data()
  mean, std = train(X)

  test_x = X[:10]
  test_y = X[:10]

  a, b = get_a_b(X)

  encoder1= Encoder(a, b, 32)
  #encoder1.print()

  ser1= Service(encoder1)
  ser1.gen_keys()
  ser2= ser1

  luts, tfd_encoder = get_luts(mean, std, ser1)

  cs2d = [ser1.encode_and_encrypt_vector(el) for el in test_x]

  res_list = [run(luts, el, tfd_encoder, ser1) for el in cs2d]

  ser2.set_encoder(tfd_encoder)

  dec2d = np.array([np.array(ser2.decrypt_and_decode_vector(el)) for el in res_list])

  res_list_raw = np.array([np.array(run_raw(mean, std, el)) for el in test_x])

  mses = calc_mses(dec2d, res_list_raw)
  print(f"mses: {mses}")

  accs = []
  for i in range(len(test_x)):
    acc = check_accuracy(np.array(dec2d[i]), np.array(res_list_raw[i]), ser2.get_encoder().get_half_d())
    accs.append(acc)
  print(f"accs: {accs}")

