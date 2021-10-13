import time
import sys
import numpy as np
import tfheppy
from tfheppy import Encoder
from tfheppy import Ctxt
from tfheppy import Service

PERMIT_ERROR1 = 1/200 # 0.5% relative error
PERMIT_ERROR2 = 1/100  # 1% relative error
PERMIT_ERROR3 = 1/50  # 2% relative error
PERMIT_ERROR4 = 1/20  # 5% relative error
PERMIT_ERROR5 = 1/10  # 10% relative error
PERMIT_ERROR6 = 1/5  # 20% relative error
PERMIT_LIST = [PERMIT_ERROR1, PERMIT_ERROR2, PERMIT_ERROR3, PERMIT_ERROR4, PERMIT_ERROR5, PERMIT_ERROR6]

dist_max = 10
assert (dist_max > 0)
encoder = Encoder(-1*dist_max, dist_max, 32)

ser = Service(encoder)
ser.gen_keys()

def get_a_b(x):
  tmp_max = abs(np.max(x))
  tmp_min = abs(np.min(x))
  tmp_abs = np.max([tmp_max, tmp_min])
  return -1*tmp_abs-0.1, tmp_abs+0.1


def calc_mse(x, y):
  tmp = x - y
  tmp2 = tmp*tmp
  tmp3 = np.sum(tmp2)/len(tmp)
  return tmp3

def calc_mses(x, y):
  assert len(x) == len(y)
  mses = [calc_mse(x[i], y[i]) for i in range(len(x))]
  return np.average(mses)


def calc_mse_relative(x, y, service):
  mse = calc_mse(x, y)
  return mse/service.get_encoder().get_half_d()

def print_interpret_permit(f, x):
    print(f"accuracy of {f}: {x}")


def sigmoid(a): return 1 / (1 + np.exp(-a))


def assert_with_diff(x, y, permit, dist_max):
    a = abs(x - y)
    return np.all(a/dist_max < permit)

def check_accuracy(x, y, dist_max_=None):
    if dist_max_ is None:
      dist_max_ = dist_max
    for i, permit in enumerate(PERMIT_LIST):
        tmp_res = assert_with_diff(x, y, permit=permit, dist_max=dist_max_)
        if tmp_res:
            return i 
    return -1
    


def test_hom_add():
    x = 2
    y = 3
    c1 = ser.encode_and_encrypt(x)
    c2 = ser.encode_and_encrypt(y)
    c3 = ser.add_hom_fixed_encoder(c1, c2)
    c4 = ser.sub_hom_fixed_encoder(c1, c2)
    d = ser.decrypt_and_decode(c3)
    return check_accuracy(x + y, d)

def test_hom_sub():
    x = 2
    y = 3
    c1 = ser.encode_and_encrypt(x)
    c2 = ser.encode_and_encrypt(y)
    c4 = ser.sub_hom_fixed_encoder(c1, c2)
    d = ser.decrypt_and_decode(c4)
    return check_accuracy(x-y, d)

def test_hom_adds():
    x = np.array([2, 3])
    y = np.array([1, 2])
    c1 = ser.encode_and_encrypt_vector(x)
    c2 = ser.encode_and_encrypt_vector(y)
    c3 = ser.add_hom_fixed_encoder_vector(c1, c2)
    d = ser.decrypt_and_decode_vector(c3)
    return check_accuracy(x + y, d)

def test_hom_subs():
    x = np.array([2, 3])
    y = np.array([1, 2])
    c1 = ser.encode_and_encrypt_vector(x)
    c2 = ser.encode_and_encrypt_vector(y)
    c4 = ser.sub_hom_fixed_encoder_vector(c1, c2)
    d = ser.decrypt_and_decode_vector(c4)
    return check_accuracy(x - y, d)


def test_ser_deser_ctxt_memory():
    x = 2
    c = ser.encode_and_encrypt(x)
    cstr = ser.serialize_ctxt(c)
    c = ser.deserialize_ctxt(cstr)
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x, d)


def test_ser_deser_ctxt_file():
    x = 2
    c = ser.encode_and_encrypt(x)
    ser.serialize_ctxt_to_file(c, "./c.data")
    c = ser.deserialize_ctxt_from_file("./c.data")
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x, d)

def test_ser_deser_sk_memory():
    x = 2
    c = ser.encode_and_encrypt(x)
    skstr = ser.serialize_sk()
    ser.deserialize_sk(skstr)
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x, d)


def test_ser_deser_sk_file():
    x = 2
    c = ser.encode_and_encrypt(x)
    ser.serialize_sk_to_file("./sk.key")
    ser.deserialize_sk_from_file("./sk.key")
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x, d)


def test_ser_deser_gk_memory():
    x = 2
    c = ser.encode_and_encrypt(x)
    gkstr = ser.serialize_gk()
    ser.deserialize_gk(gkstr)
    c = ser.pbs_identity(c)
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x, d)

def test_ser_deser_gk_file():
    x = 2
    c = ser.encode_and_encrypt(x)
    ser.serialize_gk_to_file("./gk.key")
    ser.deserialize_gk_from_file("./gk.key")
    c = ser.pbs_identity(c)
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x, d)


def test_pbs_identity():
    x = 2
    c = ser.encode_and_encrypt(x)
    c = ser.pbs_identity(c)
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x, d)

def test_pbs_mult():
    x = 2
    c = ser.encode_and_encrypt(x)
    m = 1.2
    c = ser.pbs_mult(c, m, 2)
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x*m, d)

def test_pbs_relu():
    x = 2
    c = ser.encode_and_encrypt(x)
    m = 1.2
    c = ser.pbs_relu(c)
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x*m, d)
    # print(x, d)

def test_pbs_sigmoid():
    x = 2
    c = ser.encode_and_encrypt(x)
    m = 1.2
    c = ser.pbs_sigmoid(c)
    d = ser.decrypt_and_decode(c)
    return check_accuracy(sigmoid(x*m), d)


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
    return check_accuracy(max(x1, x2), d)


def test_maxs():
    xs = np.array([[2,3], [1,2]])
    c1 = ser.encode_and_encrypt_vector(xs[0])
    c2 = ser.encode_and_encrypt_vector(xs[1])
    c3 = ser.max_hom_vector(c1, c2)
    d = ser.decrypt_and_decode_vector(c3)
    return check_accuracy(np.max(xs, axis=0), d)

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
    return check_accuracy(np.max(x), d)


def test_inner():
    x = np.random.randn(3)
    p = np.random.randn(3)
    c = ser.encode_and_encrypt_vector(x)
    c = ser.inner(c, p, 1)
    d = ser.decrypt_and_decode(c)
    return check_accuracy(np.inner(x, p), d)


def test_matrix_vector_mult():
    x = np.random.randn(5)
    m = np.random.randn(3, 5)
    c = ser.encode_and_encrypt_vector(x)
    c = ser.vector_matrix_mult(c, m)
    d = ser.decrypt_and_decode_vector(c)
    return check_accuracy(np.dot(m, x), d)


def test_run_custom_lut():
    x = 2.0

    custom_lut = ser.get_basic_lut()
    c = ser.encode_and_encrypt(x)
    for i, v in enumerate(custom_lut):
        custom_lut[i] = v / 3

    r = ser.apply_custom_lut(c, custom_lut)
    d = ser.decrypt_and_decode(r)

    return check_accuracy(x / 3, d)

def test_run_custom_lut_self():
    x = 2.0

    c = ser.encode_and_encrypt(x)
    custom_lut = c.get_basic_lut()
    for i, v in enumerate(custom_lut):
        custom_lut[i] = v / 3

    c.apply_custom_lut(custom_lut, ser.get_gk())
    d = ser.decrypt_and_decode(c)

    return check_accuracy(x / 3, d)

def test_rescale():
    x = 2.0
    c = ser.encode_and_encrypt(x)
    encoder_target = Encoder(-0.1*dist_max, 0.1*dist_max, 32)
    c2 = ser.rescale(c, encoder_target)
    d = ser.decrypt_and_decode(c2)
    return check_accuracy(x / 10, d)

def test_rescale_self():
    x = 2.0
    c = ser.encode_and_encrypt(x)
    encoder_target = Encoder(-0.1*dist_max, 0.1*dist_max, 32)
    c.rescale(encoder_target, ser.get_gk())
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x / 10, d)

def test_map():
    x = 2.0
    c = ser.encode_and_encrypt(x)
    encoder_target = Encoder(-2*dist_max, 2*dist_max, 32)
    c2 = ser.map(c, encoder_target)
    d = ser.decrypt_and_decode(c2)
    return check_accuracy(x, d)

def test_map_self():
    x = 2.0
    c = ser.encode_and_encrypt(x)
    encoder_target = Encoder(-2*dist_max, 2*dist_max, 32)
    c.map(encoder_target, ser.get_gk())
    d = ser.decrypt_and_decode(c)
    return check_accuracy(x, d)



if __name__ == "__main__":
    print("hello, world")
    
    #print_interpret_permit(test_hom_add, test_hom_add())
    #print_interpret_permit(test_hom_sub, test_hom_sub())
    #print_interpret_permit(test_hom_adds, test_hom_adds())
    #print_interpret_permit(test_hom_subs, test_hom_subs())
    #print_interpret_permit(test_ser_deser_ctxt_memory, test_ser_deser_ctxt_memory())
    #print_interpret_permit(test_ser_deser_ctxt_file, test_ser_deser_ctxt_file())
    #print_interpret_permit(test_ser_deser_sk_memory, test_ser_deser_sk_memory())
    #print_interpret_permit(test_ser_deser_sk_file, test_ser_deser_sk_file())
    #print_interpret_permit(test_ser_deser_gk_memory, test_ser_deser_gk_memory())
    #print_interpret_permit(test_ser_deser_gk_file, test_ser_deser_gk_file())
    #print_interpret_permit(test_pbs_identity, test_pbs_identity())
    #print_interpret_permit(test_pbs_mult, test_pbs_mult())
    #print_interpret_permit(test_pbs_relu, test_pbs_relu())
    #print_interpret_permit(test_pbs_sigmoid, test_pbs_sigmoid())
    #print_interpret_permit(test_max, test_max())
    #print_interpret_permit(test_maxs, test_maxs())
    #print_interpret_permit(test_max_in_col, test_max_in_col())
    #print_interpret_permit(test_inner, test_inner())
    #print_interpret_permit(test_matrix_vector_mult, test_matrix_vector_mult())
    #print_interpret_permit(test_run_custom_lut, test_run_custom_lut())
    #print_interpret_permit(test_run_custom_lut_self, test_run_custom_lut_self())
    #print_interpret_permit(test_rescale, test_rescale())
    #print_interpret_permit(test_rescale_self, test_rescale_self())
    print_interpret_permit(test_map, test_map())
    print_interpret_permit(test_map_self, test_map_self())
    #test_hom_add_sub()
    #test_max()
    ## test_sum_in_col()
    #test_max_in_col()
    #test_matrix_vector_mult()

    #test_run_custom_test_vector()
