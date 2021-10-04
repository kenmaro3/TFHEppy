#pragma once

#include "ctxt.hpp"
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

  double get_max(vector<double> x)
  {
    double res = abs(x[0]);
    for (int i = 1; i < x.size(); i++)
    {
      if (abs(x[i]) > res)
      {
        res = abs(x[i]);
      }
    }
    return res;
  }

  double get_wider(double x)
  {
    if (x >= 0)
    {
      //return ceil(x);
      return x + 0.2;
    }
    else
    {
      //double tmp = ceil(abs(x));
      double tmp = abs(x) + 0.2;
      return tmp * (-1.0);
    }
  }

  class MultFunction : public FunctionCustomTestVector<TFHEpp::lvl1param>
  {
  public:
    double y;
    MultFunction(double y) { this->y = y; }
    double run(double x) { return x * y; }
  };

  class Service
  {
  private:
    std::shared_ptr<TFHEpp::SecretKey> sk;
    std::shared_ptr<TFHEpp::GateKey> gk;
    Encoder encoder;

  public:
    Service() {}

    Service(Encoder encoder)
    {
      this->encoder = encoder;
    }

    Encoder get_encoder()
    {
      return this->encoder;
    }

    void gen_keys()
    {
      // generate a random key
      std::shared_ptr<TFHEpp::SecretKey> sk =
          std::make_shared<TFHEpp::SecretKey>();
      std::shared_ptr<TFHEpp::GateKey> gk =
          std::make_shared<TFHEpp::GateKey>(*sk, this->encoder);

      this->sk = sk;
      this->gk = gk;
    }

    void set_sk(pybind11::bytes x)
    {
      this->deserialize_sk(x);
    }

    void set_gk(pybind11::bytes x)
    {
      this->deserialize_gk(x);
    }

    Ctxt encode_and_encrypt(double x)
    {
      TLWE<lvl0param> c_tmp = tlweSymEncodeEncrypt<lvl0param>(x, lvl0param::alpha, this->sk->key.lvl0, this->encoder);
      return Ctxt(c_tmp, this->encoder);
      //return c;
    }

    vector<Ctxt> encode_and_encrypt_vector(const std::vector<double> &x, bool is_omp = true)
    {
      if (!is_omp)
      {
        vector<Ctxt> res;
        for (auto it = x.begin(); it != x.end(); ++it)
        {
          TLWE<lvl0param> c_tmp = tlweSymEncodeEncrypt<lvl0param>(*it, lvl0param::alpha, this->sk->key.lvl0, this->encoder);
          res.push_back(Ctxt(c_tmp, this->encoder));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          TLWE<lvl0param> c_tmp = tlweSymEncodeEncrypt<lvl0param>(x[i], lvl0param::alpha, this->sk->key.lvl0, this->encoder);
          res[i] = Ctxt(c_tmp, this->encoder);
        }
        return res;
      }
    }

    double decrypt_and_decode(Ctxt x)
    {
      double tmp = TFHEpp::tlweSymDecryptDecode<lvl0param>(x.get(), this->sk->key.lvl0, x.encoder);
      return tmp;
    }

    vector<double> decrypt_and_decode_vector(const vector<Ctxt> x, bool is_omp = true)
    {
      if (!is_omp)
      {
        vector<double> res;
        //for(auto it=x.begin(); it!=x.end(); ++it){
        for (int i = 0; i < x.size(); i++)
        {
          double tmp = tlweSymDecryptDecode<lvl0param>(x[i].get(), this->sk->key.lvl0, x[i].encoder);
          res.push_back(tmp);
        }
        return res;
      }
      else
      {
        vector<double> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = tlweSymDecryptDecode<lvl0param>(x[i].get(), this->sk->key.lvl0, x[i].encoder);
        }
        return res;
      }
    }

    Ctxt programmable_bootstrapping(Ctxt x)
    {
      TLWE<lvl0param> c_tmp;
      IdentityFunction identity_function = IdentityFunction<lvl1param>();
      TFHEpp::ProgrammableBootstrapping(c_tmp, x.get(), *(this->gk.get()), x.encoder, x.encoder, identity_function);
      return Ctxt(c_tmp, x.encoder);
    }

    vector<Ctxt> programmable_bootstrapping_vector(const vector<Ctxt> x, bool is_omp = true)
    {
      if (!is_omp)
      {
        vector<Ctxt> res;
        for (auto it = x.begin(); it != x.end(); ++it)
        {
          res.push_back(programmable_bootstrapping(*it));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = programmable_bootstrapping(x[i]);
        }
        return res;
      }
    }

    Ctxt programmable_bootstrapping_relu(Ctxt x)
    {
      TLWE<lvl0param> c_tmp;
      ReLUFunction relu_function = ReLUFunction<lvl1param>();
      TFHEpp::ProgrammableBootstrapping(c_tmp, x.get(), *(this->gk.get()), x.encoder, x.encoder, relu_function);
      return Ctxt(c_tmp, x.encoder);
    }

    vector<Ctxt> programmable_bootstrapping_relu_vector(const vector<Ctxt> x, bool is_omp = true)
    {
      if (!is_omp)
      {
        vector<Ctxt> res;
        for (auto it = x.begin(); it != x.end(); ++it)
        {
          res.push_back(programmable_bootstrapping_relu(*it));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = programmable_bootstrapping_relu(x[i]);
        }
        return res;
      }
    }

    Ctxt programmable_bootstrapping_sigmoid(Ctxt x)
    {
      TLWE<lvl0param> c_tmp;
      SigmoidFunction sigmoid_function = SigmoidFunction<lvl1param>();
      TFHEpp::ProgrammableBootstrapping(c_tmp, x.get(), *(this->gk.get()), x.encoder, x.encoder, sigmoid_function);
      return Ctxt(c_tmp, x.encoder);
    }

    vector<Ctxt> programmable_bootstrapping_sigmoid_vector(const vector<Ctxt> x, bool is_omp = true)
    {
      if (!is_omp)
      {
        vector<Ctxt> res;
        for (auto it = x.begin(); it != x.end(); ++it)
        {
          res.push_back(programmable_bootstrapping_sigmoid(*it));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = programmable_bootstrapping_sigmoid(x[i]);
        }
        return res;
      }
    }

    Ctxt programmable_bootstrapping_mult(Ctxt x, double m, double expansion)
    {
      MultFunction mult_function = MultFunction(m);

      Encoder encoder_domain = Encoder::copy(x.encoder);
      Encoder encoder_target = Encoder::copy(x.encoder);

      encoder_target.update(expansion);

      TLWE<lvl0param> c_tmp;
      TFHEpp::ProgrammableBootstrapping(c_tmp, x.get(), *(this->gk.get()), encoder_domain, encoder_target, mult_function);
      //this->encoder.update(expansion);
      return Ctxt(c_tmp, encoder_target);
    }

    vector<Ctxt> programmable_bootstrapping_mult_vector(const vector<Ctxt> x, const vector<double> m, double expansion, bool is_omp = true)
    {
      if (!is_omp)
      {
        vector<Ctxt> res;
        //for(auto it=x.begin(); it!=x.end(); ++it){
        for (int i = 0; i < x.size(); i++)
        {
          res.push_back(programmable_bootstrapping_mult(x[i], m[i], expansion));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = programmable_bootstrapping_mult(x[i], m[i], expansion);
        }
        return res;
      }
    }

    Ctxt run_custom_test_vector(Ctxt x, std::array<std::array<lvl1param::T, lvl1param::n>, 2> m)
    {
      TLWE<lvl0param> c_tmp;

      DirectCustomTestVector test_vector = DirectCustomTestVector<lvl1param>(m);
      TFHEpp::ProgrammableBootstrapping(c_tmp, x.get(), *(this->gk.get()), x.encoder, x.encoder, test_vector);

      return Ctxt(c_tmp, x.encoder);
    }

    pybind11::bytes serialize_ctxt(Ctxt x)
    {
      std::stringstream ss;
      {
        cereal::PortableBinaryOutputArchive ar(ss);
        ar(x.get());
      }
      return py::bytes(ss.str());
    }

    pybind11::bytes serialize_ctxt_vector(vector<Ctxt> x)
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

    void serialize_ctxt_to_file(Ctxt x, string path)
    {
      {
        std::ofstream ofs{path, std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        ar(x.get());
      }
    }

    void serialize_ctxt_to_file_vector(vector<Ctxt> x, string path)
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

    Ctxt deserialize_ctxt(pybind11::bytes x)
    {
      TLWE<lvl0param> ci;
      std::string tmp_str(x);
      std::stringstream ss;
      ss << tmp_str;
      {
        //std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ss);
        ar(ci);
      }
      return Ctxt(ci, this->encoder);
    }

    vector<Ctxt> deserialize_ctxt_vector(pybind11::bytes x, bool is_omp = true)
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
        vector<Ctxt> res;
        for (auto it = ci.begin(); it != ci.end(); ++it)
        {
          res.push_back(Ctxt(*it, this->encoder));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(ci.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < ci.size(); i++)
        {
          res[i] = Ctxt(ci[i], this->encoder);
        }
        return res;
      }
    }

    Ctxt deserialize_ctxt_from_file(string path)
    {
      TLWE<lvl0param> ci;
      {
        std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ar(ci);
      }
      return Ctxt(ci, this->encoder);
    }

    vector<Ctxt> deserialize_ctxt_from_file_vector(string path, bool is_omp = true)
    {
      vector<TLWE<lvl0param>> ci;
      {
        std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ar(ci);
      }
      if (!is_omp)
      {
        vector<Ctxt> res;
        for (auto it = ci.begin(); it != ci.end(); ++it)
        {
          res.push_back(Ctxt(*it, this->encoder));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(ci.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < ci.size(); i++)
        {
          res[i] = Ctxt(ci[i], this->encoder);
        }
        return res;
      }
    }

    pybind11::bytes serialize_sk()
    {
      std::stringstream ss;
      {
        cereal::PortableBinaryOutputArchive ar(ss);
        this->sk.get()->serialize(ar);
      }
      return py::bytes(ss.str());
    }

    void serialize_sk_to_file(string path)
    {
      {
        std::ofstream ofs{path, std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        this->sk.get()->serialize(ar);
      }
    }

    void deserialize_sk(pybind11::bytes x)
    {
      std::string tmp_str(x);
      std::stringstream ss;
      ss << tmp_str;

      std::unique_ptr<TFHEpp::SecretKey> ski = std::make_unique<TFHEpp::SecretKey>();
      {
        cereal::PortableBinaryInputArchive ar(ss);
        ski->serialize(ar);
      }
      this->sk = std::make_shared<TFHEpp::SecretKey>(*ski.get());
    }

    void deserialize_sk_from_file(string path)
    {
      std::unique_ptr<TFHEpp::SecretKey> ski = std::make_unique<TFHEpp::SecretKey>();
      {
        std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ski->serialize(ar);
      }
      this->sk = std::make_shared<TFHEpp::SecretKey>(*ski.get());
      //this->sk = ski;
    }

    pybind11::bytes serialize_gk()
    {
      std::stringstream ss;
      {
        cereal::PortableBinaryOutputArchive ar(ss);
        this->gk.get()->serialize(ar);
      }
      return py::bytes(ss.str());
    }

    void serialize_gk_to_file(string path)
    {
      {
        std::ofstream ofs{path, std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        this->gk.get()->serialize(ar);
      }
    }

    void deserialize_gk(pybind11::bytes x)
    {
      std::unique_ptr<TFHEpp::GateKey> gki = std::make_unique<TFHEpp::GateKey>();
      std::string tmp_str(x);
      std::stringstream ss;
      ss << tmp_str;
      {
        cereal::PortableBinaryInputArchive ar(ss);
        gki->serialize(ar);
      }

      this->gk = std::make_shared<TFHEpp::GateKey>(*gki.get());
    }

    void deserialize_gk_from_file(string path)
    {
      std::unique_ptr<TFHEpp::GateKey> gki = std::make_unique<TFHEpp::GateKey>();
      {
        std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        gki->serialize(ar);
      }
      this->gk = std::make_shared<TFHEpp::GateKey>(*gki.get());
    }

    Ctxt add_hom_fixed_encoder(Ctxt x, Ctxt y)
    {
      TLWE<lvl0param> c_tmp;
      TFHEpp::HomADDFixedEncoder(c_tmp, x.get(), y.get(), x.encoder, y.encoder);
      return Ctxt(c_tmp, x.encoder);
    }

    vector<Ctxt> add_hom_fixed_encoder_vector(vector<Ctxt> x, vector<Ctxt> y, bool is_omp)
    {
      if (!is_omp)
      {
        vector<Ctxt> res;
        for (int i = 0; i < x.size(); i++)
        {
          res.push_back(this->add_hom_fixed_encoder(x[i], y[i]));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = this->add_hom_fixed_encoder(x[i], y[i]);
        }
        return res;
      }
    }

    Ctxt sub_hom_fixed_encoder(Ctxt x, Ctxt y)
    {
      TLWE<lvl0param> c_tmp;
      TFHEpp::HomSUBFixedEncoder(c_tmp, x.get(), y.get(), x.encoder, y.encoder);
      return Ctxt(c_tmp, x.encoder);
    }

    vector<Ctxt> sub_hom_fixed_encoder_vector(vector<Ctxt> x, vector<Ctxt> y, bool is_omp)
    {
      if (!is_omp)
      {
        vector<Ctxt> res;
        for (int i = 0; i < x.size(); i++)
        {
          res.push_back(this->sub_hom_fixed_encoder(x[i], y[i]));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = this->sub_hom_fixed_encoder(x[i], y[i]);
        }
        return res;
      }
    }

    Ctxt add_const(Ctxt x, double m)
    {
      TLWE<lvl0param> c_tmp;
      TFHEpp::HomADDCONST(c_tmp, x.get(), m, x.encoder);
      return Ctxt(c_tmp, x.encoder);
    }

    vector<Ctxt> add_const_vector(vector<Ctxt> x, vector<double> m, bool is_omp)
    {
      if (!is_omp)
      {
        vector<Ctxt> res;
        for (int i = 0; i < x.size(); i++)
        {
          res.push_back(this->add_const(x[i], m[i]));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = this->add_const(x[i], m[i]);
        }
        return res;
      }
    }

    Ctxt max_hom(Ctxt x, Ctxt y)
    {
      TLWE<lvl0param> c_tmp;
      TFHEpp::HomMAX(c_tmp, x.get(), y.get(), x.encoder, y.encoder, x.encoder, *(this->gk.get()));
      return Ctxt(c_tmp, x.encoder);
    }

    vector<Ctxt> max_hom_vector(vector<Ctxt> x, vector<Ctxt> y, bool is_omp)
    {
      if (!is_omp)
      {
        vector<Ctxt> res;
        for (int i = 0; i < x.size(); i++)
        {
          res.push_back(this->max_hom(x[i], y[i]));
        }
        return res;
      }
      else
      {
        vector<Ctxt> res(x.size());
        int i;
#pragma omp parallel for private(i)
        for (i = 0; i < x.size(); i++)
        {
          res[i] = this->max_hom(x[i], y[i]);
        }
        return res;
      }
    }

    Ctxt sum_in_col(vector<Ctxt> x, int start_idx, int how_many)
    {
      assert(start_idx >= 0);
      assert(how_many >= 0);
      assert((start_idx + how_many) <= x.size());
      Ctxt tmp = x[start_idx];
      for (int i = start_idx + 1; i < (start_idx + how_many); i++)
      {
        tmp = this->add_hom_fixed_encoder(tmp, x[i]);
      }
      return tmp;
    }

    Ctxt max_in_col(vector<Ctxt> x, int start_idx, int how_many)
    {
      assert(start_idx >= 0);
      assert(how_many >= 0);
      assert((start_idx + how_many) <= x.size());
      Ctxt tmp = x[start_idx];
      for (int i = start_idx + 1; i < (start_idx + how_many); i++)
      {
        tmp = this->max_hom(tmp, x[i]);
      }
      return tmp;
    }

    Ctxt inner(vector<Ctxt> x, const vector<double> m, double expansion, bool is_omp = true)
    {
      vector<Ctxt> tmp = this->programmable_bootstrapping_mult_vector(x, m, expansion, is_omp);
      Ctxt res = this->sum_in_col(tmp, 0, tmp.size());
      return res;
    }

    vector<Ctxt> vector_matrix_mult(vector<Ctxt> x, const vector<vector<double>> m, double expansion = 1.0, bool is_omp = true)
    {
      assert(x.size() == m[0].size());
      double max_ds = 0.;
      for (int i = 0; i < m.size(); i++)
      {
        max_ds = max(max_ds, get_max(m[i]));
      }
      max_ds = get_wider(max_ds);
      if (max_ds < 1.0)
        max_ds = 1.0;

      max_ds *= expansion;

      vector<Ctxt> res(m.size());
      for (int i = 0; i < m.size(); i++)
      {
        res[i] = this->inner(x, m[i], max_ds, is_omp);
      }
      return res;
    }
  };

}
