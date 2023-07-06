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

    // class CtxtRGSW
    // {
    // public:
    //     TRGSWFFT<lvl0param> ctxt;
    //     CtxtRGSW() {}

    //     TRGSW<lvl0param> get() const noexcept
    //     {
    //         return this->ctxt;
    //     }
    // };

    class CtxtRGSWLevel1
    {
    public:
        TRGSWFFT<lvl1param> ctxt;
        CtxtRGSWLevel1() {}

        CtxtRGSWLevel1(TRGSWFFT<lvl1param> ctxt)
        {
            this->ctxt = ctxt;
        }

        TRGSWFFT<lvl1param> get() const noexcept
        {
            return this->ctxt;
        }
    };
}