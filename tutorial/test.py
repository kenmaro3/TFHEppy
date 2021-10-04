import time
import sys
import numpy as np
import tfheppy
from tfheppy import Encoder
from tfheppy import Ctxt
from tfheppy import Service
import pytest

PERMIT_ERROR = 1/100
PERMIT_ERROR2 = 1/3
PERMIT_ERROR3 = 2/3

encoder = Encoder(-10, 10, 32)

ser = Service(encoder)
ser.gen_keys()


def generate_custom_vector():
    a = [1] * 2
    b = [a] * 1204

    return ser.run_custom_test_vector(x=ser, m=b)


def sigmoid(a): return 1 / (1 + np.exp(-a))


def assert_with_diff(x, y, permit=PERMIT_ERROR):
    a = abs(x - y)
    return np.all(a < permit)


def test_hom_add_sub():
    x = 2
    y = 3
    c1 = ser.encode_and_encrypt(x)
    c2 = ser.encode_and_encrypt(y)
    c3 = ser.add_hom_fixed_encoder(c1, c2)
    c4 = ser.sub_hom_fixed_encoder(c1, c2)
    d = ser.decrypt_and_decode(c3)
    assert assert_with_diff(x + y, d)
    d = ser.decrypt_and_decode(c4)
    assert assert_with_diff(x - y, d)

    x = np.array([2, 3])
    y = np.array([1, 2])
    c1 = ser.encode_and_encrypt_vector(x)
    c2 = ser.encode_and_encrypt_vector(y)
    c3 = ser.add_hom_fixed_encoder_vector(c1, c2)
    c4 = ser.sub_hom_fixed_encoder_vector(c1, c2)
    d = ser.decrypt_and_decode_vector(c3)

    assert assert_with_diff(x + y, d)
    d = ser.decrypt_and_decode_vector(c4)
    assert assert_with_diff(x - y, d)


def test_ser_deser():
    x = 2
    c = ser.encode_and_encrypt(x)
    print("ser/deser ctxt via memory")
    cstr = ser.serialize_ctxt(c)
    c = ser.deserialize_ctxt(cstr)
    d = ser.decrypt_and_decode(c)
    assert assert_with_diff(x, d)

    print("ser/deser ctxt via file")
    ser.serialize_ctxt_to_file(c, "./c.data")
    c = ser.deserialize_ctxt_from_file("./c.data")
    d = ser.decrypt_and_decode(c)
    assert assert_with_diff(x, d)

    print("ser/deser sk via memory")
    skstr = ser.serialize_sk()
    ser.deserialize_sk(skstr)
    d = ser.decrypt_and_decode(c)
    assert assert_with_diff(x, d)

    print("ser/deser sk via file")
    ser.serialize_sk_to_file("./sk.key")
    ser.deserialize_sk_from_file("./sk.key")
    d = ser.decrypt_and_decode(c)
    assert assert_with_diff(x, d)

    print("ser/deser gk via memory")
    gkstr = ser.serialize_gk()
    ser.deserialize_gk(gkstr)
    c = ser.pbs_identity(c)
    d = ser.decrypt_and_decode(c)

    assert assert_with_diff(x, d, PERMIT_ERROR2)

    print("ser/deser gk via file")
    ser.serialize_gk_to_file("./gk.key")
    ser.deserialize_gk_from_file("./gk.key")
    c = ser.pbs_identity(c)
    d = ser.decrypt_and_decode(c)
    assert assert_with_diff(x, d, PERMIT_ERROR3)


def test_pbs():
    x = 2
    c = ser.encode_and_encrypt(x)
    print("pbs_identity")
    c = ser.pbs_identity(c)
    d = ser.decrypt_and_decode(c)
    assert assert_with_diff(x, d, PERMIT_ERROR2)

    print("pbs_mult")
    m = 1.2
    c = ser.pbs_mult(c, m, 2)
    d = ser.decrypt_and_decode(c)
    assert assert_with_diff(x*m, d, PERMIT_ERROR3)

    print("pbs_relu")
    c = ser.pbs_relu(c)
    d = ser.decrypt_and_decode(c)
    assert assert_with_diff(x*m, d, PERMIT_ERROR3)
    # print(x, d)

    print("pbs_sigmoid")
    c = ser.pbs_sigmoid(c)
    d = ser.decrypt_and_decode(c)
    assert assert_with_diff(sigmoid(x*m), d, PERMIT_ERROR3)


def test_enc_dec_vector():
    x = [2 for _ in range(10)]
    t1 = time.time()
    c = ser.encode_and_encrypt_vector(x)
    t2 = time.time()
    print(f"time: {t2-t1}")
    t1 = time.time()
    c = ser.encode_and_encrypt_vector(x, False)
    t2 = time.time()
    print(f"time: {t2-t1}")
    d = ser.decrypt_and_decode_vector(c)
    for el in d:
        print(el)

    c = ser.pbs_identity_vector(c)
    d = ser.decrypt_and_decode_vector(c)
    for el in d:
        print(el)


def test_max():
    x1 = 2
    x2 = 3
    c1 = ser.encode_and_encrypt(x1)
    c2 = ser.encode_and_encrypt(x2)
    c3 = ser.max_hom(c1, c2)
    d = ser.decrypt_and_decode(c3)
    assert assert_with_diff(max(x1, x2), d, PERMIT_ERROR2)

    x1 = [2, 3]
    x2 = [1, 2]
    c1 = ser.encode_and_encrypt_vector(x1)
    c2 = ser.encode_and_encrypt_vector(x2)
    c3 = ser.max_hom_vector(c1, c2)
    d = ser.decrypt_and_decode_vector(c3)
    print(d)


# def test_sum_in_col():
#     x = np.array([1, 2, 3])
#     c = ser.encode_and_encrypt_vector(x)
#     c = ser.add_in_col(c, 0, 3)
#     d = ser.decrypt_and_decode(c)
#     print(f"{np.sum(x)} == {d}")


def test_max_in_col():
    x = np.array([1, 2, 3])
    c = ser.encode_and_encrypt_vector(x)
    c = ser.max_in_col(c, 0, 3)
    d = ser.decrypt_and_decode(c)
    print(f"{np.max(x)} == {d}")
    assert assert_with_diff(np.max(x), d, PERMIT_ERROR2)


def test_inner():
    x = np.random.rand(3)
    p = np.random.rand(3)
    c = ser.encode_and_encrypt_vector(x)
    c = ser.inner(c, p, 1)
    d = ser.decrypt_and_decode(c)
    assert assert_with_diff(np.inner(x, p), d, PERMIT_ERROR3)


def test_matrix_vector_mult():
    x = np.random.rand(5)
    m = np.random.rand(3, 5)
    c = ser.encode_and_encrypt_vector(x)
    c = ser.vector_matrix_mult(c, m)
    d = ser.decrypt_and_decode_vector(c)

    print(f"{np.dot(m, x)}")
    print(d)


if __name__ == "__main__":
    # test_hom_add_sub()
    # test_hom_add_sub()
    # test_ser_deser()
    # test_pbs()
    # test_max()
    # # test_sum_in_col()
    # test_max_in_col()
    # test_matrix_vector_mult()

    generate_custom_vector()
