import numpy as np

import tfheppy
from tfheppy import Encoder
from tfheppy import Ctxt
from tfheppy import Service


def sigmoid(a):
  return 1 / (1 + np.exp(-a))


if __name__ == "__main__":
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

  #print("pbs_mult")
  #m = 1.2
  #c = ser.pbs_mult(c, m, 2)
  #d = ser.decrypt_and_decode(c)
  #print(f"{x*m} == {d}")

  #print("pbs_relu")
  #c = ser.pbs_relu(c)
  #d = ser.decrypt_and_decode(c)
  #print(f"{x*m} == {d}")

  #print("pbs_sigmoid")
  #c = ser.pbs_sigmoid(c)
  #d = ser.decrypt_and_decode(c)
  #print(f"{sigmoid(x*m)} == {d}")

  print("ser/deser ctxt via memory")
  cstr = ser.serialize_ctxt(c)
  c = ser.deserialize_ctxt(cstr)
  d = ser.decrypt_and_decode(c)
  print(f"{x} == {d}")

  print("ser/deser ctxt via file")
  ser.serialize_ctxt_to_file(c, "./c.data")
  c = ser.deserialize_ctxt_from_file("./c.data")
  d = ser.decrypt_and_decode(c)
  print(f"{x} == {d}")

  print("ser/deser sk via memory")
  skstr = ser.serialize_sk()
  ser.deserialize_sk(skstr)
  d = ser.decrypt_and_decode(c)
  print(f"{x} == {d}")

  print("ser/deser sk via file")
  ser.serialize_sk_to_file("./sk.key")
  ser.deserialize_sk_from_file("./sk.key")
  d = ser.decrypt_and_decode(c)
  print(f"{x} == {d}")

  print("ser/deser gk via memory")
  gkstr = ser.serialize_gk()
  ser.deserialize_gk(gkstr)
  c = ser.pbs_identity(c)
  d = ser.decrypt_and_decode(c)
  print(f"{x} == {d}")

  print("ser/deser gk via file")
  ser.serialize_gk_to_file("./gk.key")
  ser.deserialize_gk_from_file("./gk.key")
  c = ser.pbs_identity(c)
  d = ser.decrypt_and_decode(c)
  print(f"{x} == {d}")


  quit()
