#include <pybind11/pybind11.h>
#include <tfhe++.hpp>
#include "include/ctxt.hpp"
#include "include/service.hpp"

using namespace std;
using namespace TFHEpp;
namespace py = pybind11;

#include <chrono>
using namespace std::chrono;
inline double get_time_sec(void){
    return static_cast<double>(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count())/1000000000;
}

int add(int i, int j) {
    return i + j;
}

void test(){
  double a = -10.0;
  double b = -10.0;
  int bp = 32;
  Encoder encoder(a,b,bp);

}

PYBIND11_MODULE(tfheppy, m) {
    m.doc() = "pybind11 tfheppy plugin"; // optional module docstring
    m.def("add", &add, "A function which adds two numbers");

    py::class_<Encoder>(m,"Encoder")
      .def(py::init<>())
      .def(py::init<double, double, int, bool>())
      .def("encode_0_1", &Encoder::encode_0_1, py::arg("x"));

    py::class_<Ctxt>(m, "Ctxt")
      .def(py::init<>())
      .def(py::init<array<lvl0param::T, lvl0param::n+1>>());

    py::class_<Service>(m, "Service")
      .def(py::init<>())
      .def(py::init<Encoder>())
      .def("gen_keys", &Service::gen_keys) 
      .def("get_encoder", &Service::get_encoder)
      .def("encode_and_encrypt", &Service::encode_and_encrypt, py::arg("x"))
      .def("decrypt_and_decode", &Service::decrypt_and_decode, py::arg("x"))
      .def("pbs_identity", &Service::programmable_bootstrapping, py::arg("x"))
      .def("pbs_relu", &Service::programmable_bootstrapping_relu, py::arg("x"))
      .def("pbs_sigmoid", &Service::programmable_bootstrapping_sigmoid, py::arg("x"))
      .def("pbs_mult", &Service::programmable_bootstrapping_mult, py::arg("x"), py::arg("m"), py::arg("expansion"))
      .def("serialize_ctxt", &Service::serialize_ctxt, py::arg("x"))
      .def("serialize_ctxt_to_file", &Service::serialize_ctxt_to_file, py::arg("x"), py::arg("path"))
      .def("deserialize_ctxt", &Service::deserialize_ctxt, py::arg("x"))
      .def("deserialize_ctxt_from_file", &Service::deserialize_ctxt_from_file, py::arg("path"))
      .def("serialize_sk", &Service::serialize_sk)
      .def("serialize_sk_to_file", &Service::serialize_sk_to_file, py::arg("path"))
      .def("deserialize_sk", &Service::deserialize_sk, py::arg("x"))
      .def("deserialize_sk_from_file", &Service::deserialize_sk_from_file, py::arg("path"))
      .def("serialize_gk", &Service::serialize_gk)
      .def("serialize_gk_to_file", &Service::serialize_gk_to_file, py::arg("path"))
      .def("deserialize_gk", &Service::deserialize_gk, py::arg("x"))
      .def("deserialize_gk_from_file", &Service::deserialize_gk_from_file, py::arg("path"))
      .def("set_sk", &Service::set_sk, py::arg("x"))
      .def("set_gk", &Service::set_gk, py::arg("x"));



}

