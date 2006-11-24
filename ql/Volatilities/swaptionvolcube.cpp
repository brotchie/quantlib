/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Ferdinando Ametrano
 Copyright (C) 2006 Katiuscia Manzoni

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/


#include <ql/Volatilities/swaptionvolcube.hpp>
#include <ql/Utilities/dataformatters.hpp>

namespace QuantLib {

    SwaptionVolatilityCube::SwaptionVolatilityCube(
        const Handle<SwaptionVolatilityStructure>& atmVol,
        const std::vector<Period>& optionTenors,
        const std::vector<Period>& swapTenors,
        const std::vector<Spread>& strikeSpreads,
        const std::vector<std::vector<Handle<Quote> > >& volSpreads,
        const boost::shared_ptr<SwapIndex>& swapIndexBase,
        bool vegaWeightedSmileFit)
    : SwaptionVolatilityStructure(0, atmVol->calendar(),
                                     atmVol->businessDayConvention()),
      atmVol_(atmVol),
      optionTenors_(optionTenors),
      optionDates_(optionTenors.size()),
      optionTimes_(optionTenors.size()),
      optionDatesAsReal_(optionTenors.size()),
      swapTenors_(swapTenors),
      swapLengths_(swapTenors.size()),
      nStrikes_(strikeSpreads.size()),
      strikeSpreads_(strikeSpreads),
      localStrikes_(nStrikes_),
      localSmile_(nStrikes_),
      volSpreads_(volSpreads),
      swapIndexBase_(swapIndexBase),
      vegaWeightedSmileFit_(vegaWeightedSmileFit)
    {

        atmVol_ = atmVol;
        registerWith(atmVol_);
        atmVol_.currentLink()->enableExtrapolation();

        // register with SwapIndexBase
        if (!swapIndexBase_)
            registerWith(swapIndexBase_);


        nOptionTenors_ = optionTenors.size();
        optionDates_[0] = optionDateFromOptionTenor(optionTenors[0]);
        optionDatesAsReal_[0] =
            static_cast<Real>(optionDates_[0].serialNumber());
        optionTimes_[0] = timeFromReference(optionDates_[0]);
        QL_REQUIRE(0.0<optionTimes_[0],
                   "first option time is negative ("
                   << optionTimes_[0] << ")");
        for (Size i=1; i<nOptionTenors_; i++) {
            optionDates_[i] = optionDateFromOptionTenor(optionTenors[i]);
            optionDatesAsReal_[i] =
                static_cast<Real>(optionDates_[i].serialNumber());
            optionTimes_[i] = timeFromReference(optionDates_[i]);
            QL_REQUIRE(optionTimes_[i-1]<optionTimes_[i],
                       "non increasing option times: time[" << i-1 <<
                       "] = " << optionTimes_[i-1] << ", time[" << i <<
                       "] = " << optionTimes_[i]);
        }

        optionInterpolator_ =
            LinearInterpolation(optionTimes_.begin(),
                                optionTimes_.end(),
                                optionDatesAsReal_.begin());
        optionInterpolator_.enableExtrapolation();

        nSwapTenors_ = swapTenors_.size();
        Date startDate = optionDates_[0]; // as good as any
        Date endDate = startDate + swapTenors_[0];
        swapLengths_[0] = dayCounter().yearFraction(startDate,endDate);
        QL_REQUIRE(0.0<swapLengths_[0],
                   "first swap length is negative");
        for (Size i=1; i<nSwapTenors_; i++) {
            Date endDate = startDate + swapTenors_[i];
            swapLengths_[i] = dayCounter().yearFraction(startDate,endDate);
            QL_REQUIRE(swapLengths_[i-1]<swapLengths_[i],
                       "non increasing swap length");
        }

        QL_REQUIRE(nStrikes_>1, "too few strikes (" << nStrikes_ << ")");
        for (Size i=1; i<nStrikes_; i++) {
            QL_REQUIRE(strikeSpreads_[i-1]<strikeSpreads_[i],
                "non increasing strike spreads");
        }

        QL_REQUIRE(!volSpreads_.empty(), "empty vol spreads matrix");

        for (Size i=0; i<volSpreads_.size(); i++)
            QL_REQUIRE(nStrikes_==volSpreads_[i].size(),
                       "mismatch between number of strikes (" << nStrikes_ <<
                       ") and number of columns (" << volSpreads_[i].size() <<
                       ") in the " << io::ordinal(i) << " row");

        QL_REQUIRE(nOptionTenors_*nSwapTenors_==volSpreads_.size(),
            "mismatch between number of option tenors * swap tenors (" <<
            nOptionTenors_*nSwapTenors_ << ") and number of rows (" <<
            volSpreads_.size() << ")");

        registerWithVolatilitySpread();

    }

    void SwaptionVolatilityCube::registerWithVolatilitySpread()
    {
        for (Size i=0; i<nStrikes_; i++)
            for (Size j=0; j<nOptionTenors_; j++)
                for (Size k=0; k<nSwapTenors_; k++)
                    registerWith(volSpreads_[j*nSwapTenors_+k][i]);
    }

    Rate SwaptionVolatilityCube::atmStrike(const Date& optionDate,
                                           const Period& swapTenor) const {

        // FIXME use a familyName-based index factory
        return SwapIndex(swapIndexBase_->familyName(),
                         swapTenor,
                         swapIndexBase_->settlementDays(),
                         swapIndexBase_->currency(),
                         swapIndexBase_->calendar(),
                         swapIndexBase_->fixedLegFrequency(),
                         swapIndexBase_->fixedLegConvention(),
                         swapIndexBase_->dayCounter(),
                         swapIndexBase_->iborIndex()).fixing(optionDate);
    }

    const std::vector<Period>&
    SwaptionVolatilityCube::optionTenors() const {
         return optionTenors_;
    }

    const std::vector<Date>&
    SwaptionVolatilityCube::optionDates() const {
        return optionDates_;
    }

    const std::vector<Time>&
    SwaptionVolatilityCube::optionTimes() const {
        return optionTimes_;
    }

    const std::vector<Period>&
     SwaptionVolatilityCube::swapTenors() const {
         return swapTenors_;
     }

    const std::vector<Time>&
    SwaptionVolatilityCube::swapLengths() const {
        return swapLengths_;
    }

}
