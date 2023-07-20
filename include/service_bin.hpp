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

  class ServiceBin
  {
  private:
    // std::shared_ptr<TFHEpp::SecretKey> sk;
    Sk sk;
    // std::shared_ptr<TFHEpp::GateKey> gk;
    Gk gk;

    int level;

  public:
    ServiceBin()
    {
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

    CtxtBin encrypt(bool x)
    {
      if (x == 0)
      {
        TLWE<lvl0param> c_tmp = tlweSymEncrypt<lvl0param>(-lvl0param::mu, lvl0param::alpha, this->sk.get_sk()->key.lvl0);
        return CtxtBin(c_tmp);
      }
      else
      {
        TLWE<lvl0param> c_tmp = tlweSymEncrypt<lvl0param>(lvl0param::mu, lvl0param::alpha, this->sk.get_sk()->key.lvl0);
        return CtxtBin(c_tmp);
      }
    }

    CtxtBinLevel1 encrypt_level1(bool x)
    {
      if (x == 0)
      {
        TLWE<lvl1param> c_tmp = tlweSymEncrypt<lvl1param>(-lvl1param::mu, lvl1param::alpha, this->sk.get_sk()->key.lvl1);
        return CtxtBinLevel1(c_tmp);
      }
      else
      {
        TLWE<lvl1param> c_tmp = tlweSymEncrypt<lvl1param>(lvl1param::mu, lvl1param::alpha, this->sk.get_sk()->key.lvl1);
        return CtxtBinLevel1(c_tmp);
      }
    }

    // CtxtRing encrypt_ring(array<uint32_t, TFHEpp::lvl0param::n> x)
    // {
    //   TRLWE<TFHEpp::lvl0param> c1 = trlweSymEncrypt<TFHEpp::lvl0param>(
    //       x, TFHEpp::lvl0param::alpha, sk.get_sk()->key.lvl0);

    //   return CtxtRing(c1);
    // }

    CtxtRingLevel1 encrypt_ring_level1(array<uint32_t, TFHEpp::lvl1param::n> x)
    {
      array<uint32_t, TFHEpp::lvl1param::n> pmu;
      for (int i = 0; i < TFHEpp::lvl1param::n; i++)
        pmu[i] = x[i] ? TFHEpp::lvl1param::mu : -TFHEpp::lvl1param::mu;
      TRLWE<TFHEpp::lvl1param> c1 = trlweSymEncrypt<TFHEpp::lvl1param>(
          pmu, TFHEpp::lvl1param::alpha, sk.get_sk()->key.lvl1);

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

    array<bool, TFHEpp::lvl1param::n> decrypt_ring_level1(CtxtRingLevel1 x)
    {
      array<bool, TFHEpp::lvl1param::n> res;
      res = trlweSymDecrypt<TFHEpp::lvl1param>(x.ctxt, sk.get_sk()->key.lvl1);
      return res;
    }

    vector<CtxtBin>
    encrypt_vector(const std::vector<bool> &x, bool is_omp = true)
    {
      if (!is_omp)
      {
        vector<CtxtBin> res;
        for (auto it = x.begin(); it != x.end(); ++it)
        {
          TLWE<lvl0param> c_tmp = tlweSymEncrypt<lvl0param>(*it, lvl0param::alpha, this->sk.get_sk()->key.lvl0);
          res.push_back(CtxtBin(c_tmp));
        }
        return res;
      }
      else
      {
        vector<CtxtBin> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          TLWE<lvl0param> c_tmp = tlweSymEncrypt<lvl0param>(x[i], lvl0param::alpha, this->sk.get_sk()->key.lvl0);
          res[i] = CtxtBin(c_tmp);
        }
        return res;
      }
    }

    vector<CtxtBinLevel1> encrypt_vector_level1(const std::vector<bool> &x, bool is_omp = true)
    {
      if (!is_omp)
      {
        vector<CtxtBinLevel1> res;
        for (auto it = x.begin(); it != x.end(); ++it)
        {
          TLWE<lvl1param> c_tmp = tlweSymEncrypt<lvl1param>(*it, lvl1param::alpha, this->sk.get_sk()->key.lvl1);
          res.push_back(CtxtBinLevel1(c_tmp));
        }
        return res;
      }
      else
      {
        vector<CtxtBinLevel1> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          TLWE<lvl1param> c_tmp = tlweSymEncrypt<lvl1param>(x[i], lvl1param::alpha, this->sk.get_sk()->key.lvl1);
          res[i] = CtxtBinLevel1(c_tmp);
        }
        return res;
      }
    }

    bool decrypt(CtxtBin x)
    {

      bool tmp = TFHEpp::tlweSymDecrypt<lvl0param>(x.get(), this->sk.get_sk()->key.lvl0);
      return tmp;
    }

    bool decrypt_level1(CtxtBinLevel1 x)
    {

      bool tmp = TFHEpp::tlweSymDecrypt<lvl1param>(x.get(), this->sk.get_sk()->key.lvl1);
      return tmp;
    }

    vector<bool> decrypt_vector(const vector<CtxtBin> x, bool is_omp = true)
    {
      if (!is_omp)
      {
        vector<bool> res;
        // for(auto it=x.begin(); it!=x.end(); ++it){
        for (int i = 0; i < x.size(); i++)
        {
          bool tmp = tlweSymDecrypt<lvl0param>(x[i].get(), this->sk.get_sk()->key.lvl0);
          res.push_back(tmp);
        }
        return res;
      }
      else
      {
        vector<bool> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = tlweSymDecrypt<lvl0param>(x[i].get(), this->sk.get_sk()->key.lvl0);
        }
        return res;
      }
    }

    vector<bool> decrypt_vector_level1(const vector<CtxtBinLevel1> x, bool is_omp = true)
    {
      if (!is_omp)
      {
        vector<bool> res;
        // for(auto it=x.begin(); it!=x.end(); ++it){
        for (int i = 0; i < x.size(); i++)
        {
          bool tmp = tlweSymDecrypt<lvl1param>(x[i].get(), this->sk.get_sk()->key.lvl1);
          res.push_back(tmp);
        }
        return res;
      }
      else
      {
        vector<bool> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = tlweSymDecrypt<lvl1param>(x[i].get(), this->sk.get_sk()->key.lvl1);
        }
        return res;
      }
    }

    pybind11::bytes serialize_ctxt(CtxtBin x)
    {
      std::stringstream ss;
      {
        cereal::PortableBinaryOutputArchive ar(ss);
        ar(x.get());
      }
      return py::bytes(ss.str());
    }

    pybind11::bytes serialize_ctxt_vector(vector<CtxtBin> x)
    {
      std::stringstream ss;
      vector<TLWE<lvl0param>> tmp_cts;
      for (auto it = x.begin(); it != x.end(); ++it)
      {
        tmp_cts.push_back((*it).get());
      }
      {
        cereal::PortableBinaryOutputArchive ar(ss);
        ar(tmp_cts);
      }
      return py::bytes(ss.str());
    }

    void serialize_ctxt_to_file(CtxtBin x, string path)
    {
      {
        std::ofstream ofs{path, std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        ar(x.get());
      }
    }

    void serialize_ctxt_to_file_vector(vector<CtxtBin> x, string path)
    {
      vector<TLWE<lvl0param>> tmp_cts;
      for (auto it = x.begin(); it != x.end(); ++it)
      {
        tmp_cts.push_back((*it).get());
      }
      {
        std::ofstream ofs{path, std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        ar(tmp_cts);
      }
    }

    CtxtBin deserialize_ctxt(pybind11::bytes x)
    {
      TLWE<lvl0param> ci;
      std::string tmp_str(x);
      std::stringstream ss;
      ss << tmp_str;
      {
        // std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ss);
        ar(ci);
      }
      return CtxtBin(ci);
    }

    vector<CtxtBin> deserialize_ctxt_vector(pybind11::bytes x, bool is_omp = true)
    {
      vector<TLWE<lvl0param>> ci;
      std::string tmp_str(x);
      std::stringstream ss;
      ss << tmp_str;
      {
        cereal::PortableBinaryInputArchive ar(ss);
        ar(ci);
      }

      if (!is_omp)
      {
        vector<CtxtBin> res;
        for (auto it = ci.begin(); it != ci.end(); ++it)
        {
          res.push_back(CtxtBin(*it));
        }
        return res;
      }
      else
      {
        vector<CtxtBin> res(ci.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < ci.size(); i++)
        {
          res[i] = CtxtBin(ci[i]);
        }
        return res;
      }
    }

    CtxtBin deserialize_ctxt_from_file(string path)
    {
      TLWE<lvl0param> ci;
      {
        std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ar(ci);
      }
      return CtxtBin(ci);
    }

    vector<CtxtBin> deserialize_ctxt_from_file_vector(string path, bool is_omp = true)
    {
      vector<TLWE<lvl0param>> ci;
      {
        std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ar(ci);
      }
      if (!is_omp)
      {
        vector<CtxtBin> res;
        for (auto it = ci.begin(); it != ci.end(); ++it)
        {
          res.push_back(CtxtBin(*it));
        }
        return res;
      }
      else
      {
        vector<CtxtBin> res(ci.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < ci.size(); i++)
        {
          res[i] = CtxtBin(ci[i]);
        }
        return res;
      }
    }

    CtxtRingLevel1 inverse_sample_extract_index(CtxtBinLevel1 x, int index = 0)
    {
      TRLWE<TFHEpp::lvl1param> c0;
      TFHEpp::InverseSampleExtractIndex<TFHEpp::lvl1param>(c0, x.ctxt, index);
      return CtxtRingLevel1(c0);
    }

    CtxtRingLevel1 cmux_fft(CtxtRGSWLevel1 c_flag, CtxtRingLevel1 c_true, CtxtRingLevel1 c_false)
    {
      TRLWE<TFHEpp::lvl1param> cres;
      CMUXFFT<lvl1param>(cres, c_flag.ctxt, c_true.ctxt, c_false.ctxt);
      return CtxtRingLevel1(cres);
    }
  };
}
