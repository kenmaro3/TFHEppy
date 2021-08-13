import numpy as np

import tfheppy
from tfheppy import Encoder
from tfheppy import Ctxt
from tfheppy import Service


def sigmoid(a):
  return 1 / (1 + np.exp(-a))


print("setup encoder")
encoder = Encoder(-10, 10, 32, True)


print("setup service with keys")
ser = Service(encoder)
ser.gen_keys()


print("encrypt")
x = 2
c = ser.encode_and_encrypt(x)

print("decrypt")
d = ser.decrypt_and_decode(c)
print(f"{x} == {d}")

print("pbs_identity")
c = ser.pbs_identity(c)
d = ser.decrypt_and_decode(c)
print(f"{x} == {d}")

print("pbs_mult")
m = 1.2
c = ser.pbs_mult(c, m, 2)
d = ser.decrypt_and_decode(c)
print(f"{x*m} == {d}")

print("pbs_relu")
c = ser.pbs_relu(c)
d = ser.decrypt_and_decode(c)
print(f"{x*m} == {d}")

print("pbs_sigmoid")
c = ser.pbs_sigmoid(c)
d = ser.decrypt_and_decode(c)
print(f"{sigmoid(x*m)} == {d}")
