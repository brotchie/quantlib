
/*
 * Copyright (C) 2000-2001 QuantLib Group
 *
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.org/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, please email quantlib-users@lists.sourceforge.net
 * The license is also available at http://quantlib.org/LICENSE.TXT
 *
 * The members of the QuantLib Group are listed in the Authors.txt file, also
 * available at http://quantlib.org/group.html
*/

/*! \file finitedifferenceeuropean.cpp

    \fullpath
    Sources/Pricers/%finitedifferenceeuropean.cpp
    \brief Example of European option calculated using finite differences

*/

// $Id$
// $Log$
// Revision 1.20  2001/08/31 15:23:47  sigmud
// refining fullpath entries for doxygen documentation
//
// Revision 1.19  2001/08/28 14:47:46  nando
// unsigned int instead of int
//
// Revision 1.18  2001/08/09 14:59:48  sigmud
// header modification
//
// Revision 1.17  2001/08/08 11:07:50  sigmud
// inserting \fullpath for doxygen
//
// Revision 1.16  2001/08/07 11:25:55  sigmud
// copyright header maintenance
//
// Revision 1.15  2001/07/25 15:47:29  sigmud
// Change from quantlib.sourceforge.net to quantlib.org
//
// Revision 1.14  2001/07/19 16:40:11  lballabio
// Improved docs a bit
//
// Revision 1.13  2001/05/25 09:29:40  nando
// smoothing #include xx.hpp and cutting old Log messages
//
// Revision 1.12  2001/05/24 15:40:10  nando
// smoothing #include xx.hpp and cutting old Log messages
//

#include "ql/Pricers/finitedifferenceeuropean.hpp"
#include "ql/FiniteDifferences/valueatcenter.hpp"

namespace QuantLib {

    namespace Pricers {

        using FiniteDifferences::valueAtCenter;
        using FiniteDifferences::firstDerivativeAtCenter;
        using FiniteDifferences::secondDerivativeAtCenter;

        FiniteDifferenceEuropean::FiniteDifferenceEuropean(Type type,
            double underlying, double strike, Rate dividendYield,
            Rate riskFreeRate, Time residualTime, double volatility,
            unsigned int timeSteps, unsigned int gridPoints)
            : BSMNumericalOption(type, underlying, strike, dividendYield,
                                 riskFreeRate, residualTime, volatility,
                                 gridPoints),
            timeSteps_(timeSteps), euroPrices_(gridPoints_){}


        void FiniteDifferenceEuropean::calculate() const {
            setGridLimits(underlying_, residualTime_);
            initializeGrid();
            initializeInitialCondition();
            initializeOperator();

            FiniteDifferences::StandardFiniteDifferenceModel
                                model(finiteDifferenceOperator_);

            euroPrices_ = initialPrices_;

            model.rollback(euroPrices_, residualTime_, 0, timeSteps_);

            value_ = valueAtCenter(euroPrices_);
            delta_ = firstDerivativeAtCenter(euroPrices_, grid_);
            gamma_ = secondDerivativeAtCenter(euroPrices_, grid_);

            double dt = residualTime_/timeSteps_;
            model.rollback(euroPrices_, 0.0, -dt, 1);
            double valueMinus = valueAtCenter(euroPrices_);
            theta_ = (value_ - valueMinus) / dt;

            hasBeenCalculated_ = true;
        }

    }

}
