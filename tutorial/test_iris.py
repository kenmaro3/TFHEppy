import keras
from keras.layers import Dense, Dropout, Flatten, Lambda, Activation
from keras import backend as K
from keras.models import Sequential, model_from_json
from keras.layers.core import Dense
from keras.utils import np_utils
from keras.callbacks import CSVLogger
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
import sklearn

import time
import sys
import numpy as np

import tfheppy
from tfheppy import Encoder
from tfheppy import Ctxt
from tfheppy import Service

from more_itertools import chunked

from test import check_accuracy, get_a_b
from test import calc_mse, calc_mses, calc_mse_relative

import lib_dense as ldense
import lib_keras as lkeras


def dispense_data():
    iris = load_iris()
    data_x = iris.data
    data_y = iris.target
    data_y = np_utils.to_categorical(data_y)
    data_x = sklearn.preprocessing.normalize(data_x)
    x_train, x_test, y_train, y_test = train_test_split(
        data_x, data_y, test_size=0.2)
    return x_train, y_train, x_test, y_test


def gen_ser():
    print("setup encoder")
    encoder = Encoder(-3, 3, 32)
    print("setup service with keys")
    ser = Service(encoder)
    ser.gen_keys()
    return ser


if __name__ == "__main__":
    print("\n=========================================")
    print("hello, world")
    model = keras.models.load_model("./nn_iris.h5")

    x_train, y_train, x_test, y_test = dispense_data()
    a, b = get_a_b(x_train)

    print("setup encoder")
    encoder = Encoder(a, b, 32)
    print("setup service with keys")
    ser = Service(encoder)
    ser.gen_keys()



    print("\n\n=========================================")
    ser = gen_ser()
    ws, bs = lkeras.get_w_b(model)

    t1 = time.time()
    c = ser.encode_and_encrypt_vector(x_test[0])
    t2 = time.time()
    print(f"time enc: {t2-t1}")

    t1 = time.time()

    c1 = ldense.cipher_dense(0, c, ws[0], bs[0], ser, True, False)
    c2 = ldense.cipher_dense(1, c1, ws[1], bs[1], ser, True, False)
    c3 = ldense.cipher_dense(2, c2, ws[2], bs[2], ser, False, False)
    t2 = time.time()
    print("\n\n=========================================")
    print(f"time total: {t2-t1}")

    p1 = ldense.raw_dense(x_test[0], ws[0], bs[0], True, False)
    p2 = ldense.raw_dense(p1, ws[1], bs[1], True, False)
    p3 = ldense.raw_dense(p2, ws[2], bs[2], False, False)

    dec1 = ser.decrypt_and_decode_vector(c1)
    dec2 = ser.decrypt_and_decode_vector(c2)
    dec3 = ser.decrypt_and_decode_vector(c3)

    print("\n\n=========================================")
    print(f"showing mse:")
    mse = calc_mse(dec1, p1)
    print(mse)
    mse = calc_mse(dec2, p2)
    print(mse)
    mse = calc_mse(dec3, p3)
    print(mse)

    print(f"label(cipher): {np.argmax(dec3)}\nlabel(raw): {np.argmax(p3)}")
    
    quit()
    print("\n\n=========================================")
    print(f"showing resluts:")
    print(dec3)
    print(p3)
    quit()

    c2 = ldense.cipher_max_pooling_1d(c, ser, 2)
    p2 = ser.decrypt_and_decode_vector(c2)

    p3 = ldense.raw_max_pooling_1d(x_test[0])

    print(p2, p3)

    print("\n\n=========================================")
    print(f"done...")
