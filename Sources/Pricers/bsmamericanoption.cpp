
/*
 * Copyright (C) 2000
 * Ferdinando Ametrano, Luigi Ballabio, Adolfo Benin, Marco Marchioro
 * 
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.sourceforge.net/
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
 * if not, contact ferdinando@ametrano.net
 *
 * QuantLib license is also available at http://quantlib.sourceforge.net/LICENSE.TXT
*/

#include "pricers.h"
#include "finitedifferences.h"

namespace QuantLib {

	namespace Pricers {
	
		using FiniteDifferences::FiniteDifferenceModel;
		using FiniteDifferences::CrankNicolson;
		using FiniteDifferences::StepCondition;
		using FiniteDifferences::TridiagonalOperator;
		
		double BSMAmericanOption::value() const {
			if (!hasBeenCalculated) {
				setGridLimits();
				initializeGrid(sMin,sMax);
				initializeInitialCondition();
				initializeOperator();
				// Small time step which will be used to evaluate theta using finite difference
				double smallDt = theResidualTime/(100*theTimeSteps); 
				FiniteDifferenceModel<CrankNicolson<TridiagonalOperator> > model(theOperator);
				// Control-variate variance reduction:
				// 1) calculate the greeks of the European option analytically
				BSMEuropeanOption analyticEuro(theType,theUnderlying,theStrike,theUnderlyingGrowthRate, 
				  theRiskFreeRate,theResidualTime,theVolatility);
				double analyticEuroValue = analyticEuro.value();
				double analyticEuroDelta = analyticEuro.delta();
				double analyticEuroGamma = analyticEuro.gamma();
				double analyticEuroTheta = analyticEuro.theta();
				// 2) calculate the greeks of the European option numerically
				Array theEuroPrices = thePrices;
				model.rollback(theEuroPrices,theResidualTime,0.0,theTimeSteps);
				double numericEuroValue = valueAtCenter(theEuroPrices);
				double numericEuroDelta = firstDerivativeAtCenter(theEuroPrices,theGrid);
				double numericEuroGamma = secondDerivativeAtCenter(theEuroPrices,theGrid);
				model.rollback(theEuroPrices,0.0,-smallDt,1);
				double numericEuroTheta = (numericEuroValue-valueAtCenter(theEuroPrices))/smallDt;
				// 3) calculate the greeks of the American option numerically on the same grid
				Handle<StepCondition<Array> > americanCondition(new BSMAmericanCondition(thePrices));
				model.rollback(thePrices,theResidualTime,0.0,theTimeSteps,americanCondition);
				double numericAmericanValue = valueAtCenter(thePrices);
				double numericAmericanDelta = firstDerivativeAtCenter(thePrices,theGrid);
				double numericAmericanGamma = secondDerivativeAtCenter(thePrices,theGrid);
				model.rollback(thePrices,0.0,-smallDt,1,americanCondition);
				double numericAmericanTheta = (numericAmericanValue-valueAtCenter(thePrices))/smallDt;
				// 4) combine the results
				theValue = numericAmericanValue - numericEuroValue + analyticEuroValue;
				theDelta = numericAmericanDelta - numericEuroDelta + analyticEuroDelta;
				theGamma = numericAmericanGamma - numericEuroGamma + analyticEuroGamma;
				theTheta = numericAmericanTheta - numericEuroTheta + analyticEuroTheta;
				hasBeenCalculated = true;
			}
			return theValue;
		}

	}

}
