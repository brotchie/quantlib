
/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include "stats.hpp"
#include <ql/Math/statistics.hpp>
#include <ql/Math/sequencestatistics.hpp>

using namespace QuantLib;
using namespace QuantLib::Math;

#define LENGTH(a) (sizeof(a)/sizeof(a[0]))

typedef GaussianStatistics<IncrementalStatistics>
    IncrementalGaussianStatistics;

void StatisticsTest::runTest() {

    double data[] = { 3.0, 4.0, 5.0, 2.0, 3.0, 4.0, 5.0, 6.0, 4.0, 7.0 };
    std::vector<double> weights(LENGTH(data),1.0);

    IncrementalGaussianStatistics igs;
    igs.addSequence(data,data+LENGTH(data),weights.begin());

    Statistics s;
    s.addSequence(data,data+LENGTH(data),weights.begin());

    unsigned long dimension = 5;
    SequenceStatistics<IncrementalGaussianStatistics> sigs(dimension);
    SequenceStatistics<Statistics> ss(dimension);

    Size i;
    for (i = 0; i<LENGTH(data); i++) {
        std::vector<double> temp(dimension, data[i]);
        sigs.add(temp, weights[i]);
        ss.add(temp, weights[i]);
    }

    double calculated, expected;
    double tolerance;
    std::vector<double> calculatedSequence;


    if (igs.samples() != LENGTH(data))
        CPPUNIT_FAIL(
            "IncrementalGaussianStatistics: "
            "wrong number of samples\n"
            "    calculated: "
            + IntegerFormatter::toString(igs.samples()) + "\n"
            "    expected:   "
            + IntegerFormatter::toString(LENGTH(data)));
    if (s.samples() != LENGTH(data))
        CPPUNIT_FAIL(
            "Statistics: "
            "wrong number of samples\n"
            "    calculated: "
            + IntegerFormatter::toString(igs.samples()) + "\n"
            "    expected:   "
            + IntegerFormatter::toString(LENGTH(data)));
    if (sigs.samples() != LENGTH(data))
        CPPUNIT_FAIL(
            "SequenceStatistics<IncrementalGaussianStatistics>: "
            "wrong number of samples\n"
            "    calculated: "
            + IntegerFormatter::toString(igs.samples()) + "\n"
            "    expected:   "
            + IntegerFormatter::toString(LENGTH(data)));
    if (ss.samples() != LENGTH(data))
        CPPUNIT_FAIL(
            "SequenceStatistics<Statistics>: "
            "wrong number of samples\n"
            "    calculated: "
            + IntegerFormatter::toString(igs.samples()) + "\n"
            "    expected:   "
            + IntegerFormatter::toString(LENGTH(data)));


    expected = std::accumulate(weights.begin(),weights.end(),0.0);
    calculated = igs.weightSum();
    if (calculated != expected)
        CPPUNIT_FAIL(
            "IncrementalGaussianStatistics: "
            "wrong sum of weights\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculated = s.weightSum();
    if (calculated != expected)
        CPPUNIT_FAIL(
            "Statistics: "
            "wrong sum of weights\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculated = sigs.weightSum();
    if (calculated != expected)
        CPPUNIT_FAIL(
            "SequenceStatistics<IncrementalGaussianStatistics>: "
            "wrong sum of weights\n"
            "    calculated: "
            + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculated = ss.weightSum();
    if (calculated != expected)
        CPPUNIT_FAIL(
            "SequenceStatistics<Statistics>: "
            "wrong sum of weights\n"
            "    calculated: "
            + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));


    expected = *std::min_element(data,data+LENGTH(data));
    calculated = igs.min();
    if (calculated != expected)
        CPPUNIT_FAIL(
            "IncrementalGaussianStatistics: "
            "wrong minimum value\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculated = s.min();
    if (calculated != expected)
        CPPUNIT_FAIL(
            "Statistics: "
            "wrong minimum value\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculatedSequence = sigs.min();
    for (i=0; i<dimension; i++) {
        if (calculatedSequence[i] != expected)
            CPPUNIT_FAIL(
                "SequenceStatistics<IncrementalGaussianStatistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong minimum value\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }
    calculatedSequence = ss.min();
    for (i=0; i<dimension; i++) {
        if (calculatedSequence[i] != expected)
            CPPUNIT_FAIL(
                "SequenceStatistics<Statistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong minimum value\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }


    expected = *std::max_element(data,data+LENGTH(data));
    calculated = igs.max();
    if (calculated != expected)
        CPPUNIT_FAIL(
            "IncrementalGaussianStatistics: "
            "wrong maximum value\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculated = s.max();
    if (calculated != expected)
        CPPUNIT_FAIL(
            "Statistics: "
            "wrong maximum value\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculatedSequence = sigs.max();
    for (i=0; i<dimension; i++) {
        if (calculatedSequence[i] != expected)
            CPPUNIT_FAIL(
                "SequenceStatistics<IncrementalGaussianStatistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong maximun value\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }
    calculatedSequence = ss.max();
    for (i=0; i<dimension; i++) {
        if (calculatedSequence[i] != expected)
            CPPUNIT_FAIL(
                "SequenceStatistics<Statistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong maximun value\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }


    expected = 4.3;
    tolerance = 1.0e-9;
    calculated = igs.mean();
    if (QL_FABS(calculated-expected) > tolerance)
        CPPUNIT_FAIL(
            "IncrementalGaussianStatistics: "
            "wrong mean value\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculated = s.mean();
    if (QL_FABS(calculated-expected) > tolerance)
        CPPUNIT_FAIL(
            "Statistics: "
            "wrong mean value\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculatedSequence = sigs.mean();
    for (i=0; i<dimension; i++) {
        if (QL_FABS(calculatedSequence[i]-expected) > tolerance)
            CPPUNIT_FAIL(
                "SequenceStatistics<IncrementalGaussianStatistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong mean value\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }
    calculatedSequence = ss.mean();
    for (i=0; i<dimension; i++) {
        if (QL_FABS(calculatedSequence[i]-expected) > tolerance)
            CPPUNIT_FAIL(
                "SequenceStatistics<Statistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong mean value\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }


    expected = 2.23333333333;
    calculated = igs.variance();
    if (QL_FABS(calculated-expected) > tolerance)
        CPPUNIT_FAIL(
            "IncrementalGaussianStatistics: "
            "wrong variance\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculated = s.variance();
    if (QL_FABS(calculated-expected) > tolerance)
        CPPUNIT_FAIL(
            "Statistics: "
            "wrong variance\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculatedSequence = sigs.variance();
    for (i=0; i<dimension; i++) {
        if (QL_FABS(calculatedSequence[i]-expected) > tolerance)
            CPPUNIT_FAIL(
                "SequenceStatistics<IncrementalGaussianStatistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong variance\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }
    calculatedSequence = ss.variance();
    for (i=0; i<dimension; i++) {
        if (QL_FABS(calculatedSequence[i]-expected) > tolerance)
            CPPUNIT_FAIL(
                "SequenceStatistics<Statistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong variance\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }

    
    expected = 1.4944341181;
    calculated = igs.standardDeviation();
    if (QL_FABS(calculated-expected) > tolerance)
        CPPUNIT_FAIL(
            "IncrementalGaussianStatistics: "
            "wrong standard deviation\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculated = s.standardDeviation();
    if (QL_FABS(calculated-expected) > tolerance)
        CPPUNIT_FAIL(
            "Statistics: "
            "wrong standard deviation\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculatedSequence = sigs.standardDeviation();
    for (i=0; i<dimension; i++) {
        if (QL_FABS(calculatedSequence[i]-expected) > tolerance)
            CPPUNIT_FAIL(
                "SequenceStatistics<IncrementalGaussianStatistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong standard deviation\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }
    calculatedSequence = ss.standardDeviation();
    for (i=0; i<dimension; i++) {
        if (QL_FABS(calculatedSequence[i]-expected) > tolerance)
            CPPUNIT_FAIL(
                "SequenceStatistics<Statistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong standard deviation\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }
    
    
    expected = 0.359543071407;
    calculated = igs.skewness();
    if (QL_FABS(calculated-expected) > tolerance)
        CPPUNIT_FAIL(
            "IncrementalGaussianStatistics: "
            "wrong skewness\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculated = s.skewness();
    if (QL_FABS(calculated-expected) > tolerance)
        CPPUNIT_FAIL(
            "Statistics: "
            "wrong skewness\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculatedSequence = sigs.skewness();
    for (i=0; i<dimension; i++) {
        if (QL_FABS(calculatedSequence[i]-expected) > tolerance)
            CPPUNIT_FAIL(
                "SequenceStatistics<IncrementalGaussianStatistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong skewness\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }
    calculatedSequence = ss.skewness();
    for (i=0; i<dimension; i++) {
        if (QL_FABS(calculatedSequence[i]-expected) > tolerance)
            CPPUNIT_FAIL(
                "SequenceStatistics<Statistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong skewness\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }


    expected = -0.151799637209;
    calculated = igs.kurtosis();
    if (QL_FABS(calculated-expected) > tolerance)
        CPPUNIT_FAIL(
            "IncrementalGaussianStatistics: "
            "wrong kurtosis\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculated = s.kurtosis();
    if (QL_FABS(calculated-expected) > tolerance)
        CPPUNIT_FAIL(
            "Statistics: "
            "wrong kurtosis\n"
            "    calculated: "
            + DoubleFormatter::toString(calculated) + "\n"
            "    expected:   "
            + DoubleFormatter::toString(expected));
    calculatedSequence = sigs.kurtosis();
    for (i=0; i<dimension; i++) {
        if (QL_FABS(calculatedSequence[i]-expected) > tolerance)
            CPPUNIT_FAIL(
                "SequenceStatistics<IncrementalGaussianStatistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong kurtosis\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }
    calculatedSequence = ss.kurtosis();
    for (i=0; i<dimension; i++) {
        if (QL_FABS(calculatedSequence[i]-expected) > tolerance)
            CPPUNIT_FAIL(
                "SequenceStatistics<Statistics>: "
                + IntegerFormatter::toOrdinal(i+1) + " dimension: "
                "wrong kurtosis\n"
                "    calculated: "
                + DoubleFormatter::toString(calculatedSequence[i]) + "\n"
                "    expected:   "
                + DoubleFormatter::toString(expected));
    }

}

