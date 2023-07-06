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
    class CtxtRing
    {
    public:
        TRLWE<lvl0param> ctxt;

        CtxtRing() {}

        CtxtRing(TRLWE<lvl0param> ctxt)
        {
            this->ctxt = ctxt;
        }

        TRLWE<lvl0param> get() const noexcept
        {
            return this->ctxt;
        }
    };

    class CtxtRingLevel1
    {
    public:
        TRLWE<lvl1param> ctxt;

        CtxtRingLevel1() {}

        CtxtRingLevel1(TRLWE<lvl1param> ctxt)
        {
            this->ctxt = ctxt;
        }

        TRLWE<lvl1param> get() const noexcept
        {
            return this->ctxt;
        }
    };

}