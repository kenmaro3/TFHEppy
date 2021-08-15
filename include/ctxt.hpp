#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include <stdio.h>
#include <cassert>

namespace TFHEpp {
using namespace std;

class Ctxt
{
  public:
    //array<lvl0param::T, lvl0param::n+1> ctxt;
    TLWE<lvl0param> ctxt;
    Encoder encoder;


    Ctxt(){
    }

    //Ctxt(array<lvl0param::T, lvl0param::n+1> ctxt){
    Ctxt(TLWE<lvl0param> ctxt, Encoder encoder){
      this->ctxt = ctxt;
      this->encoder = encoder;
    }
    
    //array<lvl0param::T, lvl0param::n+1> get() const noexcept{
    TLWE<lvl0param> get() const noexcept{
      return this->ctxt;
    }

    Encoder get_encoder() const noexcept{
      return this->encoder;
    }
};



}//
