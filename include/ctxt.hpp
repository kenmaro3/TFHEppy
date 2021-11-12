#pragma once

#include "sk.hpp"
#include "gk.hpp"
#include <array>
#include <cstdint>
#include <vector>
#include <stdio.h>
#include <cassert>

namespace TFHEpp
{
  using namespace std;

  class Ctxt
  {
  public:
    //array<lvl0param::T, lvl0param::n+1> ctxt;
    TLWE<lvl0param> ctxt;
    Encoder encoder;

    Ctxt()
    {
    }

    //Ctxt(array<lvl0param::T, lvl0param::n+1> ctxt){
    Ctxt(TLWE<lvl0param> ctxt, Encoder encoder)
    {
      this->ctxt = ctxt;
      this->encoder = encoder;
    }

    //array<lvl0param::T, lvl0param::n+1> get() const noexcept{
    TLWE<lvl0param> get() const noexcept
    {
      return this->ctxt;
    }

    Encoder get_encoder() const noexcept
    {
      return this->encoder;
    }

    std::array<double, lvl1param::n> get_basic_lut()
    {
      std::array<double, lvl1param::n> args = {};
      DirectCustomTestVector<lvl1param>::basic_custom_test_vector(args, this->encoder);

      return args;
    }

    void apply_custom_lut(std::array<double, lvl1param::n> custom_test_vector_args, Gk gk)
    {
      TLWE<lvl0param> res;
      std::array<std::array<lvl1param::T, lvl1param::n>, 2> custom_test_vector;

      DirectCustomTestVector<lvl1param> test_vector = DirectCustomTestVector<lvl1param>::from_unencoded(custom_test_vector, custom_test_vector_args, this->encoder);
      TFHEpp::ProgrammableBootstrapping(res, this->get(), *(gk.get_gk().get()), this->encoder, this->encoder, test_vector);

      this->ctxt = res;
      this->encoder = this->encoder;
    }

    void apply_custom_lut(std::array<double, lvl1param::n> custom_test_vector_args, Encoder encoder_target, Gk gk)
    {
      TLWE<lvl0param> res;
      std::array<std::array<lvl1param::T, lvl1param::n>, 2> custom_test_vector;

      DirectCustomTestVector<lvl1param> test_vector = DirectCustomTestVector<lvl1param>::from_unencoded(custom_test_vector, custom_test_vector_args, encoder_target);
      TFHEpp::ProgrammableBootstrapping(res, this->get(), *(gk.get_gk().get()), this->encoder, encoder_target, test_vector);

      this->ctxt = res;
      this->encoder = encoder_target;
    }


    void rescale(Ctxt c2, Gk gk){
      std::array<double, lvl1param::n> args = {};
      DirectCustomTestVector<lvl1param>::basic_custom_test_vector(args, this->encoder);

      for(int i=0; i<lvl1param::n; i++){
        args[i] = (((args[i]- this->encoder.a) * (c2.encoder.b - c2.encoder.a)) / (this->encoder.b- this->encoder.a)) + c2.encoder.a;
      }

      TLWE<lvl0param> res;
      std::array<std::array<lvl1param::T, lvl1param::n>, 2> custom_test_vector;

      DirectCustomTestVector<lvl1param> test_vector = DirectCustomTestVector<lvl1param>::from_unencoded(custom_test_vector, args, c2.encoder);
      TFHEpp::ProgrammableBootstrapping(res, this->ctxt, *(gk.get_gk().get()), this->encoder, c2.encoder, test_vector);

      this->ctxt = res;
      this->encoder = c2.encoder;


    }

    void rescale(Encoder encoder_target, Gk gk){
      std::array<double, lvl1param::n> args = {};
      DirectCustomTestVector<lvl1param>::basic_custom_test_vector(args, this->encoder);

      for(int i=0; i<lvl1param::n; i++){
        args[i] = (((args[i]- this->encoder.a) * (encoder_target.b - encoder_target.a)) / (this->encoder.b- this->encoder.a)) + encoder_target.a;
      }

      TLWE<lvl0param> res;
      std::array<std::array<lvl1param::T, lvl1param::n>, 2> custom_test_vector;

      DirectCustomTestVector<lvl1param> test_vector = DirectCustomTestVector<lvl1param>::from_unencoded(custom_test_vector, args, encoder_target);
      TFHEpp::ProgrammableBootstrapping(res, this->ctxt, *(gk.get_gk().get()), this->encoder, encoder_target, test_vector);

      this->ctxt = res;
      this->encoder = encoder_target;

    }

    void map(Ctxt c2, Gk gk){
      assert(this->encoder.a >= c2.encoder.a); // asserting target encoder (c2) can cover entire range of original encoder (c1)
      assert(this->encoder.b <= c2.encoder.b);
      std::array<double, lvl1param::n> args = {};
      DirectCustomTestVector<lvl1param>::basic_custom_test_vector(args, this->encoder);


      TLWE<lvl0param> res;
      std::array<std::array<lvl1param::T, lvl1param::n>, 2> custom_test_vector;

      DirectCustomTestVector<lvl1param> test_vector = DirectCustomTestVector<lvl1param>::from_unencoded(custom_test_vector, args, c2.encoder);
      TFHEpp::ProgrammableBootstrapping(res, this->ctxt, *(gk.get_gk().get()), this->encoder, c2.encoder, test_vector);

      this->ctxt = res;
      this->encoder = c2.encoder;

    }

    void map(Encoder encoder_target, Gk gk){
      assert(this->encoder.a >= encoder_target.a); // asserting target encoder (c2) can cover entire range of original encoder (c1)
      assert(this->encoder.b <= encoder_target.b);
      std::array<double, lvl1param::n> args = {};
      DirectCustomTestVector<lvl1param>::basic_custom_test_vector(args, this->encoder);


      TLWE<lvl0param> res;
      std::array<std::array<lvl1param::T, lvl1param::n>, 2> custom_test_vector;

      DirectCustomTestVector<lvl1param> test_vector = DirectCustomTestVector<lvl1param>::from_unencoded(custom_test_vector, args, encoder_target);
      TFHEpp::ProgrammableBootstrapping(res, this->ctxt, *(gk.get_gk().get()), this->encoder, encoder_target, test_vector);

      this->ctxt = res;
      this->encoder = encoder_target;

    }

  };

} //
