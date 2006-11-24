/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Ferdinando Ametrano

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

/*! \file swaptionvolcube.hpp
    \brief Swaption volatility cube
*/

#ifndef quantlib_swaption_volatility_cube_h
#define quantlib_swaption_volatility_cube_h

#include <ql/swaptionvolstructure.hpp>
#include <ql/Indexes/swapindex.hpp>
#include <ql/Math/matrix.hpp>
#include <ql/quote.hpp>

namespace QuantLib {

    /*! \warning this class is not finalized and its interface might
                 change in subsequent releases.
    */
    class SwaptionVolatilityCube : public SwaptionVolatilityStructure {
      public:
        SwaptionVolatilityCube(
            const Handle<SwaptionVolatilityStructure>& atmVolStructure,
            const std::vector<Period>& optionTenors,
            const std::vector<Period>& swapTenors,
            const std::vector<Spread>& strikeSpreads,
            const std::vector<std::vector<Handle<Quote> > >& volSpreads,
            const boost::shared_ptr<SwapIndex>& swapIndexBase,
            bool vegaWeightedSmileFit);
        //! \name TermStructure interface
        //@{
        DayCounter dayCounter() const { return atmVol_->dayCounter(); }
        Date maxDate() const { return atmVol_->maxDate(); }
        Time maxTime() const { return atmVol_->maxTime(); }
        const Date& referenceDate() const { return atmVol_->referenceDate();}
        Calendar calendar() const { return atmVol_->calendar(); }
        //@}
        //! \name SwaptionVolatilityStructure interface
        //@{
        Date maxOptionDate() const { return atmVol_->maxOptionDate(); }
        Time maxOptionTime() const { return atmVol_->maxOptionTime(); }
        Period maxSwapTenor() const { return atmVol_->maxSwapTenor(); }
        Time maxSwapLength() const { return atmVol_->maxSwapLength(); }
        Rate minStrike() const { return 0.0; }
        Rate maxStrike() const { return 1.0; }
        //@}
        //! \name Other inspectors
        //@{
        virtual boost::shared_ptr<SmileSectionInterface> smileSection(
                                                  Time optionTime,
                                                  Time swapLength) const = 0;
		virtual boost::shared_ptr<SmileSectionInterface> smileSection(
                                            const Date& optionDate,
                                            const Period& swapTenor) const = 0;
		boost::shared_ptr<SmileSectionInterface>
		smileSection(const Period& optionTenor, const Period& swapTenor) const {
				Date optionDate = optionDateFromOptionTenor(optionTenor); 
				return smileSection(optionDate, swapTenor);
		};

        Rate atmStrike(const Date& optionDate,
                       const Period& swapTenor) const;
        Rate atmStrike(const Period& optionTenor,
                       const Period& swapTenor) const {
            Date optionDate = optionDateFromOptionTenor(optionTenor);
            return atmStrike(optionDate, swapTenor);
        }
        const std::vector<Period>& optionTenors() const;
        const std::vector<Date>& optionDates() const;
        const std::vector<Time>& optionTimes() const;
        const std::vector<Period>& swapTenors() const;
        const std::vector<Time>& swapLengths() const;
        //@}
      protected:
        //! \name SwaptionVolatilityStructure interface
        //@{
        std::pair<Time,Time> convertDates(const Date& optionDate,
                                          const Period& swapTenor) const {
            return atmVol_->convertDates(optionDate, swapTenor);
        }
        void registerWithVolatilitySpread();

        Volatility volatilityImpl(Time optionTime,
                                  Time swapLength,
                                  Rate strike) const;
        Volatility volatilityImpl(const Date& optionDate,
                                  const Period& swapTenor,
                                  Rate strike) const;
        Volatility volatilityImpl(const Period& optionTenor,
								  const Period& swapTenor,
								  Rate strike) const;
        //@}
        Handle<SwaptionVolatilityStructure> atmVol_; 
        std::vector<Period> optionTenors_;
        std::vector<Date> optionDates_;
        std::vector<Time> optionTimes_;
        std::vector<Real> optionDatesAsReal_;
        LinearInterpolation optionInterpolator_;
        std::vector<Period> swapTenors_;
        std::vector<Time> swapLengths_;
        Size nOptionTenors_;
        Size nSwapTenors_;
        Size nStrikes_;
        std::vector<Spread> strikeSpreads_;
        mutable std::vector<Rate> localStrikes_;
        mutable std::vector<Volatility> localSmile_;
        std::vector<std::vector<Handle<Quote> > > volSpreads_;
        boost::shared_ptr<SwapIndex> swapIndexBase_;
        bool vegaWeightedSmileFit_;
    };

    // inline

    inline Volatility SwaptionVolatilityCube::volatilityImpl(
                                                        Time optionTime,
                                                        Time swapLength,
                                                        Rate strike) const {
            return smileSection(optionTime, swapLength)->volatility(strike);
    }

    inline Volatility SwaptionVolatilityCube::volatilityImpl(
                                                    const Date& optionDate,
                                                    const Period& swapTenor,
                                                    Rate strike) const {
            return smileSection(optionDate, swapTenor)->volatility(strike);
    }

    inline Volatility SwaptionVolatilityCube::volatilityImpl(
                                                    const Period& optionTenor,
                                                    const Period& swapTenor,
                                                    Rate strike) const {
            return smileSection(optionTenor, swapTenor)->volatility(strike);
    }
}

#endif
