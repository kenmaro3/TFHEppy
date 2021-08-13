#pragma once

#include "ctxt.hpp"
#include <tfhe++.hpp>
#include <stdio.h>

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
    


};


}


