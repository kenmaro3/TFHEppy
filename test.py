import time
import sys
import numpy as np
import pprint
import tfheppy
import random
from tqdm import tqdm

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
    return random.getrandbits(1)
    # tmp = random.random()
    # if tmp < 0.5:
    #     return 0
    # else:
    #     return 1

def get_random_binary_list(size):
    res = []
    for i in range(size):
        res.append(get_random_binary())
    return res

def get_random_binary_list_2d(size1, size2):
    res = []
    for i in range(size1):
        res.append(get_random_binary_list(size2))
    return res
    

def test_cmux(ser):
    t_list = []
    t_wall_list = []
    for _ in tqdm(range(100)):
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
    #print("load done")
    t2 = time.time()
    #print(f"key_load_time: {t2-t1}")
    return ser


def encrypt_table(xs, ser):
    res_list = []
    for i in range(len(xs)):
        c1 = ser.encrypt_level1(xs[i])
        c1_ring = ser.inverse_sample_extract_index(c1, 0)
        res_list.append(c1_ring)
    return res_list

def encrypt_table_for_horizontal_packing(xs, ser):
    res_list = []
    for i in range(len(xs)):
        tmp_fix_array = np.zeros(1<<11, dtype=np.int32)
        for j in range(len(xs[i])):
            # print("here")
            # print(xs[i][j])
            # input()
            tmp_fix_array[j] = xs[i][j]
        c1_ring = ser.encrypt_ring_level1(tmp_fix_array)
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
    # input bit length
    n = 2

    # output bit length
    digits_length = 1

    # number of test trial
    test_number = 10

    # table for binary search
    y = [0,1,0,1]
    y = [1, 0, 1, 0]
    #y = [1,1, 1, 1]
    # for i in range(digits_length * 1<<n):
    #     y.append(get_random_binary())
    y = np.array(y)

    # y.shape == (output_bit_length, input_bit_length)
    y = y.reshape([digits_length, 1<<n])
    print(y)

    # table_dict_list is needed for validation
    # later used for assertion with cmux result
    table_dict_list = []
    for i in range(digits_length):
        table_dict = get_dict(n, y[i])
        table_dict_list.append(table_dict)
    pprint.pprint(table_dict_list)


    # test main
    # loop for test_number
    for test_index in tqdm(range(test_number)):
        #print(f"test_index: {test_index}")

        # input bits created by random
        search_input = []
        for n_index in range(n):
            search_input.append(get_random_binary())
        

        ## loop for output bit length
        #for output_index in range(1):

        output_index = 0

        # encrypt input bits by trgsw
        enc_input = encrypt_input(search_input, ser)
        # encrypt tables each bit with trlwe
        enc_table = encrypt_table(list(reversed(y[output_index])), ser)

        # search_index is for validation
        search_index = search_index_for_dict(search_input)

        # cmux main
        tmp_list = []
        for index in range(n):
            # separate out the first layer only
            if index == 0:
                for i in range(1<<(n-1)):
                    # cmux_fft(flag_bit_with_trgsw, return_bit_if_true_in_trlwf, return_bit_if_false_in_trlwe)
                    tmp = ser.cmux_fft(enc_input[index], enc_table[2*i], enc_table[2*i+1]) 
                    tmp_list.append(tmp)
            else:
                new_tmp_list = []
                for i in range(1<<(n-2)):
                    tmp = ser.cmux_fft(enc_input[index], tmp_list[2*i], tmp_list[2*i+1]) 
                    new_tmp_list.append(tmp)
                    tmp_list = new_tmp_list
        

        assert len(tmp_list) == 1
        # tmp_list = [trlwe([result, 0, 0, 0...])]
        # d_cmux_res = [result, 0, 0, 0....]
        d_cmux_res = ser.decrypt_ring_level1(tmp_list[0])
        print()
        print(f"search_input: {search_input}")
        print(f"search_index: {search_index}")
        print(table_dict_list)
        print(d_cmux_res[0])

        assert table_dict_list[output_index][search_index] == d_cmux_res[0]


def cmux_raw(flag, x1, x0):
    if flag:
        return x1
    else:
        return x0

def test_tree_cipher(input_bit_length, output_bit_length, ser):
    # table
    #table = [1, 0, 1, 0]
    table = get_random_binary_list(size=pow(2, input_bit_length))

    # input query
    #x = [0, 0]
    x = get_random_binary_list(size=input_bit_length)

    # print(f"x    : {x}")
    # print(f"table: {table}")

    # encrypt
    # encrypt input bits by trgsw
    enc_input = encrypt_input(x, ser)
    # encrypt tables each bit with trlwe
    enc_table = encrypt_table(table, ser)
    #print(enc_input)

    tmp_list = []

    for j in range(pow(2, input_bit_length-1)):
        tmp = ser.cmux_fft(enc_input[-1], enc_table[2*j+1], enc_table[2*j])
        tmp_list.append(tmp)
    
    # d_tmp_list = [ser.decrypt_ring_level1(el) for el in tmp_list]
    # for el in d_tmp_list:
    #    print(el[0])
    # quit()

    for i in range(1, input_bit_length):
        tmp_list_new = []
        for j in range(pow(2, input_bit_length-i-1)):
            tmp = ser.cmux_fft(enc_input[input_bit_length-i-1], tmp_list[2*j+1], tmp_list[2*j])
            tmp_list_new.append(tmp)
        tmp_list = tmp_list_new
    

    d_cmux_res = ser.decrypt_ring_level1(tmp_list[0])
    res = d_cmux_res[0]

    search_index = 0
    for i in range(input_bit_length):
        search_index += pow(2, input_bit_length - i -1) * x[i]
    #print(f"search_index: {search_index}")

    assert res == table[search_index], f"{res}, {table[search_index]}"

def test_tree_raw(input_bit_length, output_bit_length):
    # table
    #table = [1, 0, 1, 0]
    table = get_random_binary_list(size=pow(2, input_bit_length))

    # input query
    #x = [0, 0]
    x = get_random_binary_list(size=input_bit_length)

    # print(f"x    : {x}")
    # print(f"table: {table}")

    tmp_list = []

    for j in range(pow(2, input_bit_length-1)):
        tmp = cmux_raw(x[-1], table[2*j+1], table[2*j])
        tmp_list.append(tmp)
    
    for i in range(1, input_bit_length):
        tmp_list_new = []
        for j in range(pow(2, input_bit_length-i-1)):
            tmp = cmux_raw(x[input_bit_length-i-1], tmp_list[2*j+1], tmp_list[2*j])
            tmp_list_new.append(tmp)
        tmp_list = tmp_list_new
    

    assert len(tmp_list) == 1

    search_index = 0
    for i in range(input_bit_length):
        search_index += pow(2, input_bit_length - i -1) * x[i]
    #print(f"search_index: {search_index}")

    res = tmp_list[0]

    assert res == table[search_index], f"{res}, {table[search_index]}"


def main_cmux_tree():
    # input bit length
    input_bit_length = 5
    # output bit length
    output_bit_length = 1


    test_num = 100
    ser = load_key()
    for i in tqdm(range(test_num)):
        test_tree_raw(input_bit_length, output_bit_length)
        test_tree_cipher(input_bit_length, output_bit_length, ser)

def test_horizontal_packing_cipher(input_bit_length, output_bit_length, ser):
    table = get_random_binary_list_2d(output_bit_length, pow(2, input_bit_length))
    table = np.array(table)
    table_transpose = np.transpose(table)

    # input query
    x = get_random_binary_list(size=input_bit_length)

    # encrypt input bits by trgsw
    enc_input = encrypt_input(x, ser)
    # encrypt tables each bit with trlwe, for horizontal packing
    enc_table = encrypt_table_for_horizontal_packing(table_transpose, ser)

    tmp_list = []

    for j in range(pow(2, input_bit_length-1)):
        tmp = ser.cmux_fft(enc_input[-1], enc_table[2*j+1], enc_table[2*j])
        tmp_list.append(tmp)
    

    for i in range(1, input_bit_length):
        tmp_list_new = []
        for j in range(pow(2, input_bit_length-i-1)):
            tmp = ser.cmux_fft(enc_input[input_bit_length-i-1], tmp_list[2*j+1], tmp_list[2*j])
            tmp_list_new.append(tmp)
        tmp_list = tmp_list_new
    

    d_cmux_res = ser.decrypt_ring_level1(tmp_list[0])

    for i in range(output_bit_length):
        res = d_cmux_res[i]
        search_index = 0
        for j in range(input_bit_length):
            search_index += pow(2, input_bit_length - j -1) * x[j]
        assert res == table[i][search_index], f"{res}, {table[i][search_index]}, at {i}"


def main_horizontal_packing():
    # input bit length
    input_bit_length = 5
    # output bit length
    output_bit_length = 1

    test_num = 10
    ser = load_key()
    for i in tqdm(range(test_num)):
        test_horizontal_packing_cipher(input_bit_length, output_bit_length, ser)


if __name__ == "__main__":
    print("hello, world")
    main_horizontal_packing()