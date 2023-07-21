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
from tfheppy import ServiceDynamic
from tfheppy import CtxtBin




if __name__ == "__main__":
    print("hello, world")
    ser = ServiceDynamic(8)
    ser.deserialize_sk_from_file("keys/sk.txt")
    ser.deserialize_gk_from_file("keys/gk.txt")

    for i in range(10):
        p1 = 20
        c1 = ser.encrypt(p1)
        d1 = ser.decrypt(c1)
        print(d1)
