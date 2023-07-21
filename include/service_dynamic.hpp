#pragma once

#include "ctxt.hpp"
#include "ctxt_bin.hpp"
#include "ctxt_ring.hpp"
#include "ctxt_rgsw.hpp"
#include "sk.hpp"
#include "gk.hpp"
#include <tfhe++.hpp>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <omp.h>
#include <cassert>
#include <cmath>

namespace py = pybind11;

namespace TFHEpp
{
  using namespace std;

  class ServiceDynamic
  {
  private:
    uint8_t bit;
    // std::shared_ptr<TFHEpp::SecretKey> sk;
    Sk sk;
    // std::shared_ptr<TFHEpp::GateKey> gk;
    Gk gk;

    int level;

  public:
    ServiceDynamic(uint8_t bit)
    {
      this->bit = bit;
    }

    Sk get_sk()
    {
      return this->sk;
    }
    Gk get_gk() { return this->gk; }

    void set_sk(Sk sk) { this->sk = sk; }
    void set_gk(Gk gk) { this->gk = gk; }

    void gen_keys()
    {
      // generate a random key
      std::shared_ptr<TFHEpp::SecretKey> sk =
          std::make_shared<TFHEpp::SecretKey>();
      std::shared_ptr<TFHEpp::GateKey> gk =
          std::make_shared<TFHEpp::GateKey>(*sk);

      this->sk = Sk(sk);
      this->gk = Gk(gk);
    }

    void set_sk(pybind11::bytes x)
    {
      Sk sk_tmp = Sk();
      sk_tmp.deserialize_sk(x);
      this->sk = sk_tmp;
    }

    pybind11::bytes serialize_sk()
    {
      return this->sk.serialize_sk();
    }

    void serialize_sk_to_file(string path)
    {
      this->sk.serialize_sk_to_file(path);
    }

    void deserialize_sk(pybind11::bytes x)
    {
      Sk sk_tmp = Sk();
      sk_tmp.deserialize_sk(x);
      this->sk = sk_tmp;
    }

    void deserialize_sk_from_file(string path)
    {
      Sk sk_tmp = Sk();
      sk_tmp.deserialize_sk_from_file(path);
      this->sk = sk_tmp;
    }

    void set_gk(pybind11::bytes x)
    {
      Gk gk_tmp = Gk();
      gk_tmp.deserialize_gk(x);
      this->gk = gk_tmp;
    }

    pybind11::bytes serialize_gk()
    {
      return this->gk.serialize_gk();
    }

    void serialize_gk_to_file(string path)
    {
      this->gk.serialize_gk_to_file(path);
    }

    void deserialize_gk(pybind11::bytes x)
    {
      Gk gk_tmp = Gk();
      gk_tmp.deserialize_gk(x);
      this->gk = gk_tmp;
    }

    void deserialize_gk_from_file(string path)
    {
      Gk gk_tmp = Gk();
      gk_tmp.deserialize_gk_from_file(path);
      this->gk = gk_tmp;
    }

    CtxtBin encrypt(uint32_t x)
    {
      if (x > (1 << this->bit))
      {
        throw "value exceeds limit";
      }

      uint32_t max = 1 << 32 - 1;
      uint32_t tmp1 = max >> this->bit;
      uint32_t encoded_value = tmp1 * x;

      TLWE<lvl0param> c_tmp = tlweSymEncrypt<lvl0param>(encoded_value, lvl0param::alpha, this->sk.get_sk()->key.lvl0);
      return CtxtBin(c_tmp);
    }

    CtxtBinLevel1 encrypt_level1(uint32_t x)
    {
      if (x > (1 << this->bit))
      {
        throw "value exceeds limit";
      }

      uint32_t max = 1 << 32 - 1;
      uint32_t tmp1 = max >> this->bit;
      uint32_t encoded_value = tmp1 * x;

      TLWE<lvl1param> c_tmp = tlweSymEncrypt<lvl1param>(encoded_value, lvl1param::alpha, this->sk.get_sk()->key.lvl1);
      return CtxtBinLevel1(c_tmp);
    }

    // CtxtRing encrypt_ring(array<uint32_t, TFHEpp::lvl0param::n> x)
    // {
    //   TRLWE<TFHEpp::lvl0param> c1 = trlweSymEncrypt<TFHEpp::lvl0param>(
    //       x, TFHEpp::lvl0param::alpha, sk.get_sk()->key.lvl0);

    //   return CtxtRing(c1);
    // }

    CtxtRingLevel1 encrypt_ring_level1(array<uint32_t, TFHEpp::lvl1param::n> x)
    {
      uint32_t max = 1 << 32 - 1;
      uint32_t tmp1 = max >> this->bit;

      array<uint32_t, TFHEpp::lvl1param::n> encoded_array;
      for (int i = 0; i < TFHEpp::lvl1param::n; i++)
      {
        uint32_t encoded_value = tmp1 * x[i];
        encoded_array[i] = encoded_value;
      }
      TRLWE<TFHEpp::lvl1param> c1 = trlweSymEncrypt<TFHEpp::lvl1param>(
          encoded_array, TFHEpp::lvl1param::alpha, sk.get_sk()->key.lvl1);

      return CtxtRingLevel1(c1);
    }

    CtxtRGSWLevel1 encrypt_rgsw(array<lvl1param::T, lvl1param::n> x)
    {
      Polynomial<TFHEpp::lvl1param> plainpoly = {};
      for (int i = 0; i < lvl1param::n; i++)
      {
        plainpoly[i] = x[i];
      }
      TRGSWFFT<TFHEpp::lvl1param> c = trgswfftSymEncrypt<TFHEpp::lvl1param>(
          plainpoly, TFHEpp::lvl1param::alpha, sk.get_sk()->key.lvl1);

      return CtxtRGSWLevel1(c);
    }

    // array<bool, TFHEpp::lvl0param::n> decrypt_ring(CtxtRing x)
    // {
    //   array<bool, TFHEpp::lvl0param::n> res;
    //   res = trlweSymDecrypt<TFHEpp::lvl0param>(x.ctxt, sk.get_sk()->key.lvl0);
    //   return res;
    // }

    array<uint32_t, TFHEpp::lvl1param::n> decrypt_ring_level1(CtxtRingLevel1 x)
    {
      array<uint32_t, TFHEpp::lvl1param::n> res;
      res = trlweSymDecryptReturnPhase<TFHEpp::lvl1param>(x.ctxt, sk.get_sk()->key.lvl1);

      uint32_t divisor = (1UL << 32 - 1) >> this->bit;
      for (int i = 0; i < TFHEpp::lvl1param::n; i++)
      {
        uint32_t tmp = (uint32_t)round(double(res[i]) / double(divisor));
        res[i] = tmp;
        // res[i] = res[i] / divisor;
      }
      return res;
    }

    uint32_t decrypt(CtxtBin x)
    {

      uint32_t tmp = TFHEpp::tlweSymDecryptReturnPhase<lvl0param>(x.get(), this->sk.get_sk()->key.lvl0);
      uint32_t divisor = (1UL << 32 - 1) >> this->bit;

      return (uint32_t)round(double(tmp) / double(divisor));
    }

    uint32_t decrypt_level1(CtxtBinLevel1 x)
    {

      uint32_t tmp = TFHEpp::tlweSymDecryptReturnPhase<lvl1param>(x.get(), this->sk.get_sk()->key.lvl1);
      uint32_t divisor = (1UL << 32 - 1) >> this->bit;

      return (uint32_t)round(double(tmp) / double(divisor));
    }
  };
}
