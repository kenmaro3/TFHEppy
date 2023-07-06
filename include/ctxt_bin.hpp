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

  class CtxtBin
  {
  public:
    // array<lvl0param::T, lvl0param::n+1> ctxt;
    TLWE<lvl0param> ctxt;

    CtxtBin()
    {
    }

    // Ctxt(array<lvl0param::T, lvl0param::n+1> ctxt){
    CtxtBin(TLWE<lvl0param> ctxt)
    {
      this->ctxt = ctxt;
    }

    // array<lvl0param::T, lvl0param::n+1> get() const noexcept{
    TLWE<lvl0param> get() const noexcept
    {
      return this->ctxt;
    }
  };

  class CtxtBinLevel1
  {
  public:
    // array<lvl0param::T, lvl0param::n+1> ctxt;
    TLWE<lvl1param> ctxt;

    CtxtBinLevel1()
    {
    }

    // Ctxt(array<lvl0param::T, lvl0param::n+1> ctxt){
    CtxtBinLevel1(TLWE<lvl1param> ctxt)
    {
      this->ctxt = ctxt;
    }

    // array<lvl0param::T, lvl0param::n+1> get() const noexcept{
    TLWE<lvl1param> get() const noexcept
    {
      return this->ctxt;
    }
  };

} //
