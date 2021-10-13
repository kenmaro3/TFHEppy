import numpy as np
import pickle
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn import decomposition
from sklearn.preprocessing import MinMaxScaler
from sklearn.utils import shuffle
import sklearn

import tfheppy
from tfheppy import Encoder, Ctxt, Service

from test import check_accuracy, get_a_b
from test import calc_mse, calc_mses, calc_mse_relative

import lib_iris as liris

def get_luts(mins, maxs, service, set_min=0, set_max=1):
  assert len(mins) == len(maxs)
  luts = []
  min_list = []
  max_list = []

  for i in range(mins.shape[0]):
    ctv = service.get_basic_lut()

    tfd_ctv = np.zeros_like(ctv)
    for j, v in enumerate(ctv):
        tmp = (v - mins[i]) / (maxs[i] - mins[i])
        res = tmp * (set_max - set_min) + set_min
        tfd_ctv[j] = res
    
    min_list.append(np.min(tfd_ctv)-0.1)
    max_list.append(np.max(tfd_ctv)+0.1)

    luts.append(tfd_ctv)

  a, b = get_a_b(min_list+max_list)
  tfd_encoder = Encoder(a, b, 32)

  return luts, tfd_encoder

def run(luts, cs, tfd_encoder, ser):
  assert len(luts) == len(cs)
  res = []
  luts = np.array(luts)
  for i in range(len(cs)):
    r = ser.apply_custom_lut(x=cs[i], custom_test_vector=luts[i], encoder_target=tfd_encoder)
    res.append(r)
  return res

def run_raw(mins, maxs, xs, set_min=0, set_max=1):
  assert len(mins) == len(maxs)
  assert len(mins) == len(xs)
    
  res_list = []
  for i in range(len(xs)):
    tmp = (xs[i] - mins[i])/(maxs[i] - mins[i])
    res = tmp * (set_max - set_min) + set_min
    res_list.append(res)

  return res_list

def train(X, set_min=0, set_max=1):
  scaler = MinMaxScaler(feature_range=(set_min,set_max))
  scaler.fit(X)

  return scaler.data_min_, scaler.data_max_


if __name__ == "__main__":
  print("hello, world")

  set_min = -1
  set_max = 1

  X, y = liris.load_data()
  mins, maxs = train(X, set_min=set_min, set_max=set_max)
  test_x = X[:10]
  test_y = X[:10]

  a, b = get_a_b(X)

  encoder1= Encoder(a, b, 32)
  #encoder1.print()

  ser1= Service(encoder1)
  ser1.gen_keys()
  ser2= ser1

  luts, tfd_encoder = get_luts(mins, maxs, ser1, set_min=set_min, set_max=set_max)

  cs2d = [ser1.encode_and_encrypt_vector(el) for el in test_x]

  res_list = [run(luts, el, tfd_encoder, ser1) for el in cs2d]

  ser2.set_encoder(tfd_encoder)

  dec2d = np.array([np.array(ser2.decrypt_and_decode_vector(el)) for el in res_list])

  res_list_raw = np.array([np.array(run_raw(mins, maxs, el, set_min=set_min, set_max=set_max)) for el in test_x])

  mses = calc_mses(dec2d, res_list_raw)
  print(f"mses: {mses}")

  print(dec2d)
  print(res_list_raw)

  accs = []
  for i in range(len(test_x)):
    acc = check_accuracy(np.array(dec2d[i]), np.array(res_list_raw[i]), ser2.get_encoder().get_half_d())
    accs.append(acc)
  print(f"accs: {accs}")
  print(res_list[0][0].get_encoder().print())

