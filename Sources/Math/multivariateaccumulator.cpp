
/*
 * Copyright (C) 2000-2001 QuantLib Group
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
 * The license is also available at http://quantlib.sourceforge.net/LICENSE.TXT
 *
 * The members of the QuantLib Group are listed in the Authors.txt file, also
 * available at http://quantlib.sourceforge.net/Authors.txt
*/

/*! \file multivariateaccumulator.cpp

    $Source$
    $Name$
    $Log$
    Revision 1.10  2001/04/09 14:13:33  nando
    all the *.hpp moved below the Include/ql level

    Revision 1.9  2001/04/06 18:46:21  nando
    changed Authors, Contributors, Licence and copyright header

    Revision 1.8  2001/04/04 12:13:23  nando
    Headers policy part 2:
    The Include directory is added to the compiler's include search path.
    Then both your code and user code specifies the sub-directory in
    #include directives, as in
    #include <Solvers1d/newton.hpp>

    Revision 1.7  2001/04/04 11:07:24  nando
    Headers policy part 1:
    Headers should have a .hpp (lowercase) filename extension
    All *.h renamed to *.hpp

    Revision 1.6  2001/02/22 14:25:41  lballabio
    Template methods moved to header file

    Revision 1.5  2001/02/02 10:44:22  marmar
    MultivariateAccumulator does NOT have to be
    initialized with the size of data

    Revision 1.4  2001/01/26 11:09:28  marmar
    Now the covariance() method is consistent with the variance()
    method of the class Statistics

    Revision 1.3  2001/01/25 11:57:33  lballabio
    Included outer product and sqrt into matrix.h

    Revision 1.2  2001/01/24 14:56:48  aleppo
    Added iterator combining-iterator

    Revision 1.1  2001/01/24 10:51:16  marmar
    Sample accumulator for multivariate analisys


*/

#include "ql/Math/multivariateaccumulator.hpp"

namespace QuantLib {

    namespace Math {

        MultivariateAccumulator::MultivariateAccumulator()
                : size_(0){
            reset();
        }

        MultivariateAccumulator::MultivariateAccumulator(int size)
                : size_(size){
            reset();
        }

        void MultivariateAccumulator::reset() {
            sampleNumber_ = 0;
            sampleWeight_ = 0.0;
            sum_ = Array(size_,0.0);
            quadraticSum_ = Matrix(size_, size_, 0.0);
        }


        void MultivariateAccumulator::add(const Array &value, double weight) {
        /*! \pre weights must be positive or null */

            if(size_ == 0){
                size_ = value.size();
                reset();
            }
            else{
                QL_REQUIRE(value.size() == size_,
                        "MultivariateAccumulator::add : "
                                    "wrong size for input array");
            }

            QL_REQUIRE(weight >= 0.0,
                "MultivariateAccumulator::add : negative weight (" +
                DoubleFormatter::toString(weight) + ") not allowed");

            sampleNumber_ += 1.0;
            sampleWeight_ += weight;
            Array weighedValue(weight*value);

            sum_ += weighedValue;
            quadraticSum_ += outerProduct(weighedValue, value);
        }

        Matrix MultivariateAccumulator::covariance() const {
          QL_REQUIRE(sampleWeight_ > 0.0,
            "Stat::variance() : sampleWeight_=0, unsufficient");
          QL_REQUIRE(sampleNumber_ > 1,
            "Stat::variance() : sample number <=1, unsufficient");

          double inv = 1/sampleWeight_;
          return (sampleNumber_/(sampleNumber_-1.0))*
                inv*(quadraticSum_ - inv*outerProduct(sum_,sum_) );
        }

        std::vector<double> MultivariateAccumulator::meanVector() const {

            Array ma(mean());
            std::vector<double> mv(ma.size());
            std::copy(ma.begin(), ma.end(), mv.begin());
            return mv;
        }

        void MultivariateAccumulator::add(const std::vector<double> &vec,
                                                                double wei){
          Array arr(vec.size());
          std::copy(vec.begin(), vec.end(), arr.begin());
//            for(int i=0; i<vec.size(); i++) arr[i] = vec[i] ;
          add(arr, wei);
        }

    }

}
