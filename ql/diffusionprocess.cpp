
/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file diffusionprocess.cpp
    \brief Diffusion process

    \fullpath
    ql/%diffusionprocess.cpp
*/

// $Id$

#include <ql/diffusionprocess.hpp>

namespace QuantLib {

    BlackScholesProcess::BlackScholesProcess(
        const RelinkableHandle<TermStructure>& riskFreeTS,
        const RelinkableHandle<TermStructure>& dividendTS,
        const RelinkableHandle<BlackVolTermStructure>& blackVolTS,
        double s0)
    : DiffusionProcess(s0), riskFreeTS_(riskFreeTS),
      dividendTS_(dividendTS) {
        localVolTS_ = RelinkableHandle<LocalVolTermStructure>(
            Handle<LocalVolTermStructure>(new
                VolTermStructures::LocalVolSurface(blackVolTS, riskFreeTS,
                    dividendTS, s0)));
    }

    double BlackScholesProcess::drift(Time t, double x) const {
        // this is a quick and dirty patch
        // rethink how to handle extrapolation
        double sigma = localVolTS_->localVol(t, x, true);

        // we could be more anticipatory if we know the right dt
        // for which the drift will be used
        double t1 = t + 0.0001;
        return riskFreeTS_->forward(t, t1)
            - dividendTS_->forward(t, t1)
            - 0.5 * sigma * sigma;
    }
}
