import time
import sys
import numpy as np
import pprint
import tfheppy
import random

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

def get_dict(bit_length, y):
    assert pow(2, bit_length) == len(y)
    binary_dict = dict()
    for i in range(pow(2, bit_length)):
        binary_dict[i] = y[i]
    return binary_dict

def search_index_for_dict(x):
    index = 0
    for i in range(len(x)):
        index += pow(2, i) * x[len(x)-i-1]
    return index

def get_random_binary():
    tmp = random.random()
    if tmp < 0.5:
        return 0
    else:
        return 1
    

def test_cmux(ser):
    t_list = []
    t_wall_list = []
    for _ in range(10):
        t1_wall = time.time()
        p1 = get_random_binary()
        p2 = get_random_binary()
        p_flag = np.zeros(1<<11, dtype=np.int32)
        p_flag_element = get_random_binary()
        p_flag[0] = p_flag_element

        c1 = ser.encrypt_level1(p1)
        c2 = ser.encrypt_level1(p2)

        c1_ring = ser.inverse_sample_extract_index(c1, 0)
        c2_ring = ser.inverse_sample_extract_index(c2, 0)

        c_flag_rgsw = ser.encrypt_rgsw(p_flag)

        t1 = time.time()
        cmux_res = ser.cmux_fft(c_flag_rgsw, c1_ring, c2_ring)
        t2 = time.time()
        t_list.append(t2-t1)

        d_cmux_res = ser.decrypt_ring_level1(cmux_res)
        if p_flag_element == 0:
            assert d_cmux_res[0] == p2
        else:
            assert d_cmux_res[0] == p1 
        t2_wall = time.time()

        t_wall_list.append(t2_wall-t1_wall)


    print(len(t_list))
    print(f"time cmux: {np.average(t_list)}")
    print(f"time wall: {np.average(t_wall_list)}")

def load_key():
    t1 = time.time()
    #ser = ServiceBin()
    #ser.gen_keys()
    #ser.serialize_sk_to_file("keys/sk.txt")
    #ser.serialize_gk_to_file("keys/gk.txt")
    ser = ServiceBin()
    ser.deserialize_sk_from_file("keys/sk.txt")
    ser.deserialize_gk_from_file("keys/gk.txt")
    print("load done")
    t2 = time.time()
    print(f"key_load_time: {t2-t1}")
    return ser


def encrypt_table(xs, ser):
    res_list = []
    for i in range(len(xs)):
        c1 = ser.encrypt_level1(xs[i])
        c1_ring = ser.inverse_sample_extract_index(c1, 0)
        res_list.append(c1_ring)
    return res_list

def encrypt_input(xs, ser):
    res_list = []
    for i in range(len(xs)):
        p_flag = np.zeros(1<<11, dtype=np.int32)
        p_flag[0] = xs[i]
        c_flag_rgsw = ser.encrypt_rgsw(p_flag)
        res_list.append(c_flag_rgsw)
    return res_list


def cmux_tree_test(ser):
    n = 2
    digits_length = 2
    test_number = 10


    y = np.array([0, 1, 1, 0, 1, 0, 1, 0])
    y = y.reshape([digits_length, 1<<n])
    print(y)

    table_dict_list = []
    for i in range(digits_length):
        
        table_dict = get_dict(n, y[i])
        table_dict_list.append(table_dict)
    pprint.pprint(table_dict_list)


    for test_index in range(test_number):
        print(f"test_index: {test_index}")
        search_input = []
        for n_index in range(n):
            search_input.append(get_random_binary())

        enc_input = encrypt_input(search_input, ser)
        enc_table = encrypt_table(y[0], ser)

        search_index = search_index_for_dict(search_input)

        tmp_list = []
        for index in range(n):
            if index == 0:
                for i in range(1<<(n-1)):
                    tmp = ser.cmux_fft(enc_input[index], enc_table[2*i], enc_table[2*i+1]) 
                    tmp_list.append(tmp)
            else:
                new_tmp_list = []
                for i in range(1<<(n-2)):
                    tmp = ser.cmux_fft(enc_input[index], tmp_list[2*i], tmp_list[2*i+1]) 
                    new_tmp_list.append(tmp)
                    tmp_list = new_tmp_list
        

        d_cmux_res = ser.decrypt_ring_level1(tmp_list[0])

        assert table_dict_list[0][search_index] == d_cmux_res[0]



if __name__ == "__main__":
    print("hello, world")


    #ser = load_key()
    #test_cmux(ser)

    ser = load_key()
    cmux_tree_test(ser)










        
    



