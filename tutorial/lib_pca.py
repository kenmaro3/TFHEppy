import numpy as np
import pickle
from sklearn import decomposition
from sklearn.preprocessing import StandardScaler
import sklearn

import tfheppy
from tfheppy import Encoder, Ctxt, Service

from test import check_accuracy, get_a_b
from test import calc_mse, calc_mses, calc_mse_relative

import lib_iris as liris

if __name__ == "__main__":
  print("hello, world")

  X, y = liris.load_data()
  scaler = StandardScaler()
  scaler.fit(X)
  X = scaler.transform(X)

  a, b = get_a_b(X)

  
  encoder1= Encoder(a, b, 32)
  ser1= Service(encoder1)
  ser1.gen_keys()


  pca = decomposition.PCA(n_components=3)
  pca.fit(X)
  with open("pca.pkl", "wb") as f:
    pickle.dump(pca, f)

  with open("pca.pkl", "rb") as f:
    pca = pickle.load(f)

  X1 = pca.transform(X)


  test_x = X[:10]
  res_list_raw = np.array([np.dot(pca.components_, el) for el in test_x])

  cs2d = [ser1.encode_and_encrypt_vector(el) for el in test_x]

  res_list = [ser1.vector_matrix_mult(el, pca.components_) for el in cs2d]
  dec2d = [ser1.decrypt_and_decode_vector(el) for el in res_list]

  
  mses = calc_mses(dec2d, res_list_raw)
  print(f"mses: {mses}")




