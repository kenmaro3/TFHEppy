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

  class Gk
  {
  public:
    std::shared_ptr<TFHEpp::GateKey> gk;

    Gk(){};
    Gk(std::shared_ptr<TFHEpp::GateKey> gk){this->gk=gk;}

    void set_gk(std::shared_ptr<TFHEpp::GateKey> gk){this->gk = gk;}

    void set_gk(pybind11::bytes x)
    {
      this->deserialize_gk(x);
    }

    std::shared_ptr<TFHEpp::GateKey> get_gk(){return this->gk;}

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




  };

}//
