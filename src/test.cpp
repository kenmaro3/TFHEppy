#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <tfhe++.hpp>
#include "include/ctxt.hpp"
#include "include/service.hpp"
#include "include/sk.hpp"
#include "include/gk.hpp"
#include "include/ctxt_bin.hpp"
#include "include/service_bin.hpp"
#include "include/ctxt_ring.hpp"
#include "include/ctxt_rgsw.hpp"

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
      .def("print", &Encoder::print)
      .def("get_a", &Encoder::get_a)
      .def("get_b", &Encoder::get_b)
      .def("get_d", &Encoder::get_d)
      .def("get_half", &Encoder::get_half)
      .def("get_half_d", &Encoder::get_half_d)
      .def("get_bp", &Encoder::get_bp);

  py::class_<Ctxt>(m, "Ctxt")
      .def(py::init<>())
      .def(py::init<array<lvl0param::T, lvl0param::n + 1>, Encoder>())
      .def("get", &Ctxt::get)
      .def("get_encoder", &Ctxt::get_encoder)
      .def("rescale", py::overload_cast<Ctxt, Gk>(&Ctxt::rescale), py::arg("c2"), py::arg("gk"))
      .def("rescale", py::overload_cast<Encoder, Gk>(&Ctxt::rescale), py::arg("encoder_target"), py::arg("gk"))
      .def("map", py::overload_cast<Ctxt, Gk>(&Ctxt::map), py::arg("c2"), py::arg("gk"))
      .def("map", py::overload_cast<Encoder, Gk>(&Ctxt::map), py::arg("encoder_target"), py::arg("gk"))
      .def("get_basic_lut", &Ctxt::get_basic_lut)
      .def("apply_custom_lut", py::overload_cast<std::array<double, lvl1param::n>, Gk>(&Ctxt::apply_custom_lut), py::arg("custom_lut"), py::arg("gk"))
      .def("apply_custom_lut", py::overload_cast<std::array<double, lvl1param::n>, Encoder, Gk>(&Ctxt::apply_custom_lut), py::arg("custom_lut"), py::arg("encoder_target"), py::arg("gk"));

  py::class_<CtxtBin>(m, "CtxtBin")
      .def(py::init<>())
      .def(py::init<array<lvl0param::T, lvl0param::n + 1>>())
      .def("get", &CtxtBin::get);

  py::class_<CtxtBinLevel1>(m, "CtxtBinLevel1")
      .def(py::init<>())
      .def(py::init<array<lvl1param::T, lvl1param::n + 1>>())
      .def("get", &CtxtBinLevel1::get);

  py::class_<CtxtRing>(m, "CtxtRing")
      .def(py::init<>())
      .def("get", &CtxtRing::get);

  py::class_<CtxtRingLevel1>(m, "CtxtRingLevel1")
      .def(py::init<>())
      .def("get", &CtxtRingLevel1::get);

  // py::class_<CtxtRGSW>(m, "CtxtRGSW")
  //     .def(py::init<>())
  //     .def("get", &CtxtRGSW::get);

  py::class_<CtxtRGSWLevel1>(m, "CtxtRGSWLevel1")
      .def(py::init<>())
      .def("get", &CtxtRGSWLevel1::get);

  py::class_<Sk>(m, "Sk")
      .def(py::init<>());
  py::class_<Gk>(m, "Gk")
      .def(py::init<>());

  py::class_<Service>(m, "Service")
      .def(py::init<>())
      .def(py::init<Encoder>())
      .def("gen_keys", &Service::gen_keys)
      .def("get_encoder", &Service::get_encoder)
      .def("get_sk", &Service::get_sk)
      .def("get_gk", &Service::get_gk)
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
      .def("set_sk_byte", py::overload_cast<py::bytes>(&Service::set_sk), py::arg("x"))
      .def("set_sk", py::overload_cast<Sk>(&Service::set_sk), py::arg("x"))
      .def("set_gk_byte", py::overload_cast<py::bytes>(&Service::set_gk), py::arg("x"))
      .def("set_gk", py::overload_cast<Gk>(&Service::set_gk), py::arg("x"))
      .def("set_encoder", &Service::set_encoder, py::arg("encoder"))
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
      .def("get_basic_lut", &Service::get_basic_lut)
      .def("apply_custom_lut", py::overload_cast<Ctxt, std::array<double, lvl1param::n>>(&Service::apply_custom_lut), py::arg("x"), py::arg("custom_test_vector"))
      .def("apply_custom_lut", py::overload_cast<Ctxt, std::array<double, lvl1param::n>, Encoder>(&Service::apply_custom_lut), py::arg("x"), py::arg("custom_test_vector"), py::arg("encoder_target"))
      .def("rescale", py::overload_cast<Ctxt, Ctxt>(&Service::rescale), py::arg("c1"), py::arg("c2"))
      .def("rescale", py::overload_cast<Ctxt, Encoder>(&Service::rescale), py::arg("c1"), py::arg("encoder_target"))
      .def("map", py::overload_cast<Ctxt, Ctxt>(&Service::map), py::arg("c1"), py::arg("c2"))
      .def("map", py::overload_cast<Ctxt, Encoder>(&Service::map), py::arg("c1"), py::arg("encoder_target"));

  py::class_<ServiceBin>(m, "ServiceBin")
      .def(py::init<>())
      .def("gen_keys", &ServiceBin::gen_keys)
      .def("get_sk", &ServiceBin::get_sk)
      .def("get_gk", &ServiceBin::get_gk)
      .def("encrypt", &ServiceBin::encrypt, py::arg("x"))
      .def("encrypt_vector", &ServiceBin::encrypt_vector, py::arg("x"), py::arg("is_omp") = bool(true))
      .def("encrypt_level1", &ServiceBin::encrypt_level1, py::arg("x"))
      .def("encrypt_vector_level1", &ServiceBin::encrypt_vector_level1, py::arg("x"), py::arg("is_omp") = bool(true))
      // .def("encrypt_ring", &ServiceBin::encrypt_ring, py::arg("x"))
      .def("encrypt_ring_level1", &ServiceBin::encrypt_ring_level1, py::arg("x"))
      .def("encrypt_rgsw", &ServiceBin::encrypt_rgsw, py::arg("x"))

      .def("decrypt_level1", &ServiceBin::decrypt_level1, py::arg("x"))
      .def("decrypt_vector_level1", &ServiceBin::decrypt_vector_level1, py::arg("x"), py::arg("is_omp") = bool(true))
      .def("decrypt", &ServiceBin::decrypt, py::arg("x"))
      .def("decrypt_vector", &ServiceBin::decrypt_vector, py::arg("x"), py::arg("is_omp") = bool(true))
      // .def("decrypt_ring", &ServiceBin::decrypt_ring, py::arg("x"))
      .def("decrypt_ring_level1", &ServiceBin::decrypt_ring_level1, py::arg("x"))

      .def("serialize_ctxt", &ServiceBin::serialize_ctxt, py::arg("x"))
      .def("serialize_ctxt_vector", &ServiceBin::serialize_ctxt_vector, py::arg("x"))
      .def("serialize_ctxt_to_file", &ServiceBin::serialize_ctxt_to_file, py::arg("x"), py::arg("path"))
      .def("serialize_ctxt_to_file_vector", &ServiceBin::serialize_ctxt_to_file_vector, py::arg("x"), py::arg("path"))
      .def("deserialize_ctxt", &ServiceBin::deserialize_ctxt, py::arg("x"))
      .def("deserialize_ctxt_vector", &ServiceBin::deserialize_ctxt_vector, py::arg("x"), py::arg("is_omp") = bool(true))
      .def("deserialize_ctxt_from_file", &ServiceBin::deserialize_ctxt_from_file, py::arg("path"))
      .def("deserialize_ctxt_from_file_vector", &ServiceBin::deserialize_ctxt_from_file_vector, py::arg("path"), py::arg("is_omp") = bool(true))
      .def("serialize_sk", &ServiceBin::serialize_sk)
      .def("serialize_sk_to_file", &ServiceBin::serialize_sk_to_file, py::arg("path"))
      .def("deserialize_sk", &ServiceBin::deserialize_sk, py::arg("x"))
      .def("deserialize_sk_from_file", &ServiceBin::deserialize_sk_from_file, py::arg("path"))
      .def("serialize_gk", &ServiceBin::serialize_gk)
      .def("serialize_gk_to_file", &ServiceBin::serialize_gk_to_file, py::arg("path"))
      .def("deserialize_gk", &ServiceBin::deserialize_gk, py::arg("x"))
      .def("deserialize_gk_from_file", &ServiceBin::deserialize_gk_from_file, py::arg("path"))
      .def("set_sk_byte", py::overload_cast<py::bytes>(&ServiceBin::set_sk), py::arg("x"))
      .def("set_sk", py::overload_cast<Sk>(&ServiceBin::set_sk), py::arg("x"))
      .def("set_gk_byte", py::overload_cast<py::bytes>(&ServiceBin::set_gk), py::arg("x"))
      .def("set_gk", py::overload_cast<Gk>(&ServiceBin::set_gk), py::arg("x"))

      .def("inverse_sample_extract_index", &ServiceBin::inverse_sample_extract_index, py::arg("x"), py ::arg("index"))
      .def("cmux_fft", &ServiceBin::cmux_fft, py::arg("c_flag"), py ::arg("c_true"), py::arg("c_false"));
}

// Ctxt run_custon_test_vector(Ctxt x, std::array<std::array<lvl1param::T, lvl1param::n>, 2> m)
