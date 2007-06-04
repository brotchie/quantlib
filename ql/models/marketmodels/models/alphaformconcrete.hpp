/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Mark Joshi

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#ifndef quantlib_alpha_form_concrete_hpp
#define quantlib_alpha_form_concrete_hpp

#include <ql/models/marketmodels/models/alphaform.hpp>
#include <vector>

namespace QuantLib {

    class alphaforminverselinear : public alphaform {
      public:
        alphaforminverselinear(const std::vector<Time>& times,
                               Real alpha =0.0);
        virtual ~alphaforminverselinear() {}
        virtual Real operator()(Integer i) const;
        virtual void setAlpha(Real alpha_);
      private:
        std::vector<Time> times_;
        Real alpha_;
    };

    class alphaformlinearhyperbolic : public alphaform {
      public:
        alphaformlinearhyperbolic(const std::vector<Time>& times,
                                  Real alpha =0.0);
        virtual ~alphaformlinearhyperbolic() {}
        virtual Real operator()(Integer i) const;
        virtual void setAlpha(Real alpha_);
      private:
        std::vector<Time> times_;
        Real alpha_;
    };

}

#endif