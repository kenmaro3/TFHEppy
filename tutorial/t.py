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


def get_w_b():
    ws = []
    bs = []
    for i, layer in enumerate(model.layers):
        if type(layer) == keras.layers.core.Dense:
            w = layer.get_weights()[0].T
            b = layer.get_weights()[1]
            ws.append(w)
            bs.append(b)
    return ws, bs


def cipher_dense(index, c, w, b, ser, is_relu=True, is_print=False):
    print("\n\n=========================================")
    print(f"{index}th dense applying...")
    t1 = time.time()
    c = ser.vector_matrix_mult(c, w)
    c = ser.add_const_vector(c, b)
    t2 = time.time()
    print(f"time linear: {t2-t1}")

    if is_relu:
        t1 = time.time()
        c = ser.pbs_relu_vector(c)
        t2 = time.time()
        print(f"time relu: {t2-t1}")

    if is_print:
        t1 = time.time()
        d = ser.decrypt_and_decode_vector(c)
        t2 = time.time()
        print("=========================================")
        print("cipher data printing...")
        print(np.array(d))
        print(f"time dec: {t2-t1}")

    return c


def raw_dense(x, w, b, is_relu=True, is_print=False):
    tmp = np.dot(w, x) + b
    if is_relu:
        tmp = np.maximum(tmp, 0)
    if is_print:
        print("=========================================")
        print("raw data printing...")
        print(tmp)

    return tmp


def raw_max_pooling_1d(x, pool_size=2):
    splited = x.reshape((pool_size, -1))
    print("splited: ", splited)

    res = splited.max(axis=1)
    print("res: ", res)

    return res


def cipher_max_pooling_1d(x, ser, pool_size=2):
    length = len(x) // pool_size
    splited = list(chunked(x, 2))

    res = []

    for s in splited:
        print(s)

        r = ser.max_in_col(s, 0, length)
        res.append(r)

    return res


if __name__ == "__main__":
    print("\n=========================================")
    print("hello, world")
    model = keras.models.load_model("./nn_iris.h5")

    x_train, y_train, x_test, y_test = dispense_data()
    print("\n\n=========================================")
    ser = gen_ser()
    ws, bs = get_w_b()

    t1 = time.time()
    c = ser.encode_and_encrypt_vector(x_test[0])
    t2 = time.time()
    print(f"time enc: {t2-t1}")

    t1 = time.time()

    c1 = cipher_dense(0, c, ws[0], bs[0], ser, True, True)
    c1 = cipher_dense(1, c1, ws[1], bs[1], ser, True, True)
    c1 = cipher_dense(2, c1, ws[2], bs[2], ser, False, True)
    t2 = time.time()
    print("\n\n=========================================")
    print(f"time total: {t2-t1}")

    p1 = raw_dense(x_test[0], ws[0], bs[0], True, True)
    p1 = raw_dense(p1, ws[1], bs[1], True, True)
    p1 = raw_dense(p1, ws[2], bs[2], False, True)
    quit()

    c2 = cipher_max_pooling_1d(c, ser, 2)
    p2 = ser.decrypt_and_decode_vector(c2)

    p3 = raw_max_pooling_1d(x_test[0])

    print(p2, p3)

    print("\n\n=========================================")
    print(f"done...")
