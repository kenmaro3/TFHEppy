import time
import numpy as np

def cipher_dense(index, c, w, b, ser, expansion=1, is_relu=True, is_print=False):
    print("\n\n=========================================")
    print(f"{index}th dense applying...")
    t1 = time.time()
    c = ser.vector_matrix_mult(c, w, expansion)
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

