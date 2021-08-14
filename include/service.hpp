#pragma once

#include "ctxt.hpp"
#include <tfhe++.hpp>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace TFHEpp {
using namespace std;

class Service
{
  private:
    std::shared_ptr<TFHEpp::SecretKey> sk;
    std::shared_ptr<TFHEpp::GateKey> gk;
    Encoder encoder;

  public:
    Service(){}

    Service(Encoder encoder){
      this->encoder = encoder;
    }

    Encoder get_encoder(){
      return this->encoder;
    }

    void gen_keys(){
      // generate a random key
      std::shared_ptr<TFHEpp::SecretKey> sk =
          std::make_shared<TFHEpp::SecretKey>();
      std::shared_ptr<TFHEpp::GateKey> gk =
          std::make_shared<TFHEpp::GateKey>(*sk, this->encoder);

      this->sk = sk;
      this->gk = gk;
    }

    void set_sk(pybind11::bytes x){
      this->deserialize_sk(x);
    }

    void set_gk(pybind11::bytes x){
      this->deserialize_gk(x);
    }

    Ctxt encode_and_encrypt(double x){
      TLWE<lvl0param> c_tmp = tlweSymEncodeEncrypt<lvl0param>(x, lvl0param::alpha, this->sk->key.lvl0, this->encoder);
      return Ctxt(c_tmp);
      //return c;
    }

    double decrypt_and_decode(Ctxt x){
      double tmp = TFHEpp::tlweSymDecryptDecode<lvl0param>(x.get(), this->sk->key.lvl0, this->encoder);
      return tmp;

    }


    Ctxt programmable_bootstrapping(Ctxt x){
      TLWE<lvl0param> c_tmp;
      TFHEpp::ProgrammableBootstrapping(c_tmp, x.get(), *(this->gk.get()), this->encoder, this->encoder, my_identity_function);
      return Ctxt(c_tmp);
    }

    Ctxt programmable_bootstrapping_relu(Ctxt x){
      TLWE<lvl0param> c_tmp;
      TFHEpp::ProgrammableBootstrapping(c_tmp, x.get(), *(this->gk.get()), this->encoder, this->encoder, my_relu_function);
      return Ctxt(c_tmp);
    }

    Ctxt programmable_bootstrapping_sigmoid(Ctxt x){
      TLWE<lvl0param> c_tmp;
      TFHEpp::ProgrammableBootstrapping(c_tmp, x.get(), *(this->gk.get()), this->encoder, this->encoder, my_sigmoid_function);
      return Ctxt(c_tmp);
    }

    Ctxt programmable_bootstrapping_mult(Ctxt x, double m, double expansion){

      Encoder encoder_domain = Encoder::copy(this->encoder);
      Encoder encoder_target = Encoder::copy(this->encoder);

      encoder_target.update(expansion);

      TLWE<lvl0param> c_tmp;
      TFHEpp::ProgrammableBootstrapping(c_tmp, x.get(), *(this->gk.get()), encoder_domain, encoder_target, my_mult_function, m);
      this->encoder.update(expansion);
      return Ctxt(c_tmp);
    }


    pybind11::bytes serialize_ctxt(Ctxt x){
      std::stringstream ss;
      {
        cereal::PortableBinaryOutputArchive ar(ss);
        ar(x.get());
      }
      return py::bytes(ss.str());
    }

    void serialize_ctxt_to_file(Ctxt x, string path){
      {
        std::ofstream ofs{path, std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        ar(x.get());
      }
    }


    Ctxt deserialize_ctxt(pybind11::bytes x){
      TLWE<lvl0param> ci;
      std::string tmp_str(x);
      std::stringstream ss;
      ss << tmp_str;
      {
        //std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ss);
        ar(ci);
      }
      return Ctxt(ci);
    }

    Ctxt deserialize_ctxt_from_file(string path){
      TLWE<lvl0param> ci;
      {
        std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ar(ci);
      }
      return Ctxt(ci);
    }

    pybind11::bytes serialize_sk(){
      std::stringstream ss;
      {
        cereal::PortableBinaryOutputArchive ar(ss);
        this->sk.get()->serialize(ar);
      }
      return py::bytes(ss.str());
    }

    void serialize_sk_to_file(string path){
      {
        std::ofstream ofs{path, std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        this->sk.get()->serialize(ar);
      }
    }

    void deserialize_sk(pybind11::bytes x){
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


    void deserialize_sk_from_file(string path){
      std::unique_ptr<TFHEpp::SecretKey> ski = std::make_unique<TFHEpp::SecretKey>();
      {
        std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ski->serialize(ar);
      }
      this->sk = std::make_shared<TFHEpp::SecretKey>(*ski.get());
      //this->sk = ski;
    }


    pybind11::bytes serialize_gk(){
      std::stringstream ss;
      {
        cereal::PortableBinaryOutputArchive ar(ss);
        this->gk.get()->serialize(ar);
      }
      return py::bytes(ss.str());
    }


    void serialize_gk_to_file(string path){
      {
        std::ofstream ofs{path, std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        this->gk.get()->serialize(ar);
      }
    }

    void deserialize_gk(pybind11::bytes x){
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

    void deserialize_gk_from_file(string path){
      std::unique_ptr<TFHEpp::GateKey> gki = std::make_unique<TFHEpp::GateKey>();
      {
        std::ifstream ifs{path, std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        gki->serialize(ar);
      }
      this->gk = std::make_shared<TFHEpp::GateKey>(*gki.get());
    }
    


};


}


