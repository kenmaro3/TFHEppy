#pragma once

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
  class Sk
  {
  public:
    std::shared_ptr<TFHEpp::SecretKey> sk;

    Sk(){}
    Sk(std::shared_ptr<TFHEpp::SecretKey> sk){this->sk=sk;}

    void set_sk(std::shared_ptr<TFHEpp::SecretKey> sk){this->sk = sk;}

    void set_sk(pybind11::bytes x)
    {
      this->deserialize_sk(x);
    }

    std::shared_ptr<TFHEpp::SecretKey> get_sk(){return this->sk;}

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


  };
}//
