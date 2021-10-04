#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <tfhe++.hpp>
#include "include/ctxt.hpp"
#include "include/service.hpp"

using namespace std;
using namespace TFHEpp;
namespace py = pybind11;

#include <chrono>
using namespace std::chrono;
inline double get_time_sec(void)
{
  return static_cast<double>(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count()) / 1000000000;
}

int add(int i, int j)
{
  return i + j;
}

void test()
{
  double a = -10.0;
  double b = -10.0;
  int bp = 32;
  Encoder encoder(a, b, bp);
}

PYBIND11_MODULE(tfheppy, m)
{
  m.doc() = "pybind11 tfheppy plugin"; // optional module docstring
  m.def("add", &add, "A function which adds two numbers");

  py::class_<Encoder>(m, "Encoder")
      .def(py::init<>())
      .def(py::init<double, double, int>())
      .def("encode_0_1", &Encoder::encode_0_1, py::arg("x"))
      .def("print", &Encoder::print);

  py::class_<Ctxt>(m, "Ctxt")
      .def(py::init<>())
      .def(py::init<array<lvl0param::T, lvl0param::n + 1>, Encoder>())
      .def("get", &Ctxt::get)
      .def("get_encoder", &Ctxt::get_encoder);

  py::class_<Service>(m, "Service")
      .def(py::init<>())
      .def(py::init<Encoder>())
      .def("gen_keys", &Service::gen_keys)
      .def("get_encoder", &Service::get_encoder)
      .def("encode_and_encrypt", &Service::encode_and_encrypt, py::arg("x"))
      .def("encode_and_encrypt_vector", &Service::encode_and_encrypt_vector, py::arg("x"), py::arg("is_omp") = bool(true))
      .def("decrypt_and_decode", &Service::decrypt_and_decode, py::arg("x"))
      .def("decrypt_and_decode_vector", &Service::decrypt_and_decode_vector, py::arg("x"), py::arg("is_omp") = bool(true))
      .def("pbs_identity", &Service::programmable_bootstrapping, py::arg("x"))
      .def("pbs_identity_vector", &Service::programmable_bootstrapping_vector, py::arg("x"), py::arg("is_omp") = bool(true))
      .def("pbs_relu", &Service::programmable_bootstrapping_relu, py::arg("x"))
      .def("pbs_relu_vector", &Service::programmable_bootstrapping_relu_vector, py::arg("x"), py::arg("is_omp") = bool(true))
      .def("pbs_sigmoid", &Service::programmable_bootstrapping_sigmoid, py::arg("x"))
      .def("pbs_sigmoid_vector", &Service::programmable_bootstrapping_sigmoid_vector, py::arg("x"), py::arg("is_omp") = bool(true))
      .def("pbs_mult", &Service::programmable_bootstrapping_mult, py::arg("x"), py::arg("m"), py::arg("expansion"))
      .def("pbs_mult_vector", &Service::programmable_bootstrapping_mult_vector, py::arg("x"), py::arg("m"), py::arg("expansion"), py::arg("is_omp") = bool(true))
      .def("serialize_ctxt", &Service::serialize_ctxt, py::arg("x"))
      .def("serialize_ctxt_vector", &Service::serialize_ctxt_vector, py::arg("x"))
      .def("serialize_ctxt_to_file", &Service::serialize_ctxt_to_file, py::arg("x"), py::arg("path"))
      .def("serialize_ctxt_to_file_vector", &Service::serialize_ctxt_to_file_vector, py::arg("x"), py::arg("path"))
      .def("deserialize_ctxt", &Service::deserialize_ctxt, py::arg("x"))
      .def("deserialize_ctxt_vector", &Service::deserialize_ctxt_vector, py::arg("x"), py::arg("is_omp") = bool(true))
      .def("deserialize_ctxt_from_file", &Service::deserialize_ctxt_from_file, py::arg("path"))
      .def("deserialize_ctxt_from_file_vector", &Service::deserialize_ctxt_from_file_vector, py::arg("path"), py::arg("is_omp") = bool(true))
      .def("serialize_sk", &Service::serialize_sk)
      .def("serialize_sk_to_file", &Service::serialize_sk_to_file, py::arg("path"))
      .def("deserialize_sk", &Service::deserialize_sk, py::arg("x"))
      .def("deserialize_sk_from_file", &Service::deserialize_sk_from_file, py::arg("path"))
      .def("serialize_gk", &Service::serialize_gk)
      .def("serialize_gk_to_file", &Service::serialize_gk_to_file, py::arg("path"))
      .def("deserialize_gk", &Service::deserialize_gk, py::arg("x"))
      .def("deserialize_gk_from_file", &Service::deserialize_gk_from_file, py::arg("path"))
      .def("set_sk", &Service::set_sk, py::arg("x"))
      .def("set_gk", &Service::set_gk, py::arg("x"))
      .def("add_const", &Service::add_const, py::arg("x"), py::arg("m"))
      .def("add_const_vector", &Service::add_const_vector, py::arg("x"), py::arg("m"), py::arg("is_omp") = bool(true))
      .def("add_hom_fixed_encoder", &Service::add_hom_fixed_encoder, py::arg("x"), py::arg("y"))
      .def("add_hom_fixed_encoder_vector", &Service::add_hom_fixed_encoder_vector, py::arg("x"), py::arg("y"), py::arg("is_omp") = bool(true))
      .def("sub_hom_fixed_encoder", &Service::sub_hom_fixed_encoder, py::arg("x"), py::arg("y"))
      .def("sub_hom_fixed_encoder_vector", &Service::sub_hom_fixed_encoder_vector, py::arg("x"), py::arg("y"), py::arg("is_omp") = bool(true))
      .def("max_hom", &Service::max_hom, py::arg("x"), py::arg("y"))
      .def("max_hom_vector", &Service::max_hom_vector, py::arg("x"), py::arg("y"), py::arg("is_omp") = bool(true))
      .def("sum_in_col", &Service::sum_in_col, py::arg("x"), py::arg("start_idx"), py::arg("end_idx"))
      .def("max_in_col", &Service::max_in_col, py::arg("x"), py::arg("start_idx"), py::arg("end_idx"))
      .def("inner", &Service::inner, py::arg("x"), py::arg("m"), py::arg("expansion"), py::arg("is_omp") = bool(true))
      .def("vector_matrix_mult", &Service::vector_matrix_mult, py::arg("x"), py::arg("m"), py::arg("expansion") = double(1.0), py::arg("is_omp") = bool(true))
      .def("run_custom_test_vector", &Service::run_custom_test_vector, py::arg("x"), py::arg("m"));
}

// Ctxt run_custon_test_vector(Ctxt x, std::array<std::array<lvl1param::T, lvl1param::n>, 2> m)
