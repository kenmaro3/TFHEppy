import time
import sys
import numpy as np
import pprint
import tfheppy
from tfheppy import Encoder
from tfheppy import Ctxt
from tfheppy import Service
from tfheppy import ServiceBin
from tfheppy import CtxtBin

def load_ser():
    dist_max = 10
    assert (dist_max > 0)
    encoder = Encoder(-1*dist_max, dist_max, 32)
    ser = Service(encoder)
    ser.deserialize_sk_from_file("./sk.key")
    ser.deserialize_gk_from_file("./gk.key")
    return ser


def test1():
    ser = load_ser()

    x = 2
    y = 3
    c1 = ser.encode_and_encrypt(x)
    c2 = ser.encode_and_encrypt(y)
    c3 = ser.add_hom_fixed_encoder(c1, c2)
    c4 = ser.sub_hom_fixed_encoder(c1, c2)
    d3 = ser.decrypt_and_decode(c3)
    d4 = ser.decrypt_and_decode(c4)
    print(d3)
    print(d4)

def get_dict(x, y):
    assert pow(2, len(x)) == len(y)
    binary_dict = dict()
    for i in range(pow(2, len(x))):
        binary_dict[i] = y[i]
    return binary_dict

def search_index_for_dict(x):
    index = 0
    for i in range(len(x)):
        index += pow(2, i) * x[len(x)-i-1]
    return index

if __name__ == "__main__":
    print("hello, world")

    p1 = 0
    p2 = 1
    t1 = time.time()
    #ser = ServiceBin()
    #ser.gen_keys()
    #ser.serialize_sk_to_file("keys/sk.txt")
    #ser.serialize_gk_to_file("keys/gk.txt")
    ser = ServiceBin()
    ser.deserialize_sk_from_file("keys/sk.txt")
    ser.deserialize_gk_from_file("keys/gk.txt")
    print("load done")
    c1 = ser.encrypt_level1(p1)
    c2 = ser.encrypt_level1(p2)

    c1_ring = ser.inverse_sample_extract_index(c1, 0)
    c2_ring = ser.inverse_sample_extract_index(c2, 0)

    p_flag = np.zeros(1<<11, dtype=np.int32)
    p_flag[0] = 1
    c_flag_rgsw = ser.encrypt_rgsw(p_flag)

    t1 = time.time()
    cmux_res = ser.cmux_fft(c_flag_rgsw, c1_ring, c2_ring)
    t2 = time.time()
    print(f"time cmux: {t2-t1}")

    d_cmux_res = ser.decrypt_ring_level1(cmux_res)
    print(d_cmux_res[0])



    #x = [0, 1]
    #y = [1, 1, 1, 0]
    
    #table_dict = get_dict(x, y)
    #pprint.pprint(table_dict)

    #search_input = [1,1]
    #search_index = search_index_for_dict(search_input)
    #search_value = table_dict[search_index]

    #print(search_value)



        
    



