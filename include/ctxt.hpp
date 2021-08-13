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

    Ctxt(){
    }

    //Ctxt(array<lvl0param::T, lvl0param::n+1> ctxt){
    Ctxt(TLWE<lvl0param> ctxt){
      this->ctxt = ctxt;
    }
    
    //array<lvl0param::T, lvl0param::n+1> get() const noexcept{
    TLWE<lvl0param> get() const noexcept{
      return this->ctxt;
    }
};



}//
