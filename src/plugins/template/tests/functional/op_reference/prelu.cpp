// Copyright (C) 2018-2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "openvino/op/prelu.hpp"

#include <gtest/gtest.h>

#include "base_reference_test.hpp"

using namespace reference_tests;
using namespace ov;

namespace {
struct PreluParams {
    template <class IT>
    PreluParams(const ov::Shape& shape,
                const ov::element::Type& iType,
                const std::vector<IT>& iValues,
                const std::vector<IT>& oValues,
                const ov::Shape& slopeShape,
                const std::vector<IT>& negativeSlopeValues,
                const std::string& test_name = "")
        : pshape(shape),
          inType(iType),
          outType(iType),
          inputData(CreateTensor(shape, iType, iValues)),
          refData(CreateTensor(shape, iType, oValues)),
          negativeSlopeShape(slopeShape),
          negativeSlope(CreateTensor(slopeShape, iType, negativeSlopeValues)),
          testcaseName(test_name) {}

    ov::Shape pshape;
    ov::element::Type inType;
    ov::element::Type outType;
    ov::Tensor inputData;
    ov::Tensor refData;
    ov::Shape negativeSlopeShape;
    ov::Tensor negativeSlope;
    std::string testcaseName;
};

class ReferencePreluLayerTest : public testing::TestWithParam<PreluParams>, public CommonReferenceTest {
public:
    void SetUp() override {
        auto params = GetParam();
        function = CreateFunction(params.pshape, params.negativeSlopeShape, params.inType);
        inputData = {params.inputData, params.negativeSlope};
        refOutData = {params.refData};
    }
    static std::string getTestCaseName(const testing::TestParamInfo<PreluParams>& obj) {
        auto param = obj.param;
        std::ostringstream result;
        result << "shape=" << param.pshape << "_";
        result << "iType=" << param.inType << "_";
        result << "oType=" << param.outType << "_";
        if (param.testcaseName != "") {
            result << "slopeShape=" << param.negativeSlopeShape << "_";
            result << param.testcaseName;
        } else {
            result << "slopeShape=" << param.negativeSlopeShape;
        }

        return result.str();
    }

private:
    static std::shared_ptr<Model> CreateFunction(const Shape& input_shape,
                                                 const Shape& slope_shape,
                                                 const element::Type& input_type) {
        const auto in = std::make_shared<op::v0::Parameter>(input_type, input_shape);
        const auto SLOPE = std::make_shared<op::v0::Parameter>(input_type, slope_shape);
        const auto Prelu = std::make_shared<op::v0::PRelu>(in, SLOPE);
        return std::make_shared<ov::Model>(NodeVector{Prelu}, ParameterVector{in, SLOPE});
    }
};

TEST_P(ReferencePreluLayerTest, CompareWithRefs) {
    Exec();
}

template <element::Type_t IN_ET>
std::vector<PreluParams> generatePreluFloatParams() {
    using T = typename element_type_traits<IN_ET>::value_type;

    std::vector<PreluParams> preluParams{
        PreluParams(ov::Shape{6},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 5, 6},
                    std::vector<T>{1, 2, -6, -8, 5, 6},
                    ov::Shape{1},
                    std::vector<T>{2}),
        PreluParams(ov::Shape{6},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 5, 6},
                    std::vector<T>{1, 2, -12, -20, 5, 6},
                    ov::Shape{6},
                    std::vector<T>{2, 3, 4, 5, 6, 7}),
        PreluParams(ov::Shape{3, 2},
                    IN_ET,
                    std::vector<T>{-2, 3, -2, 1, -1, 0},
                    std::vector<T>{0, 3, 0, 1, 0, 0},
                    ov::Shape{2},
                    std::vector<T>{0, 1}),
        PreluParams(ov::Shape{2, 6},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 5, 6, 7, 8, -9, -10, 11, 12},
                    std::vector<T>{1, 2, -9, -16, 5, 6, 7, 8, -27, -40, 11, 12},
                    ov::Shape{6},
                    std::vector<T>{1, 2, 3, 4, 5, 6}),
        PreluParams(ov::Shape{3, 2},
                    IN_ET,
                    std::vector<T>{-1, -1, -1, -1, -1, -1},
                    std::vector<T>{-2, -0.5, -2, -0.5, -2, -0.5},
                    ov::Shape{2},
                    std::vector<T>{2, 0.5},
                    "C_2_const"),
        PreluParams(ov::Shape{2, 2, 2},
                    IN_ET,
                    std::vector<T>{-0.5, -2, -3, -4, -5, -6, -7, -8},
                    std::vector<T>{0.25, 1, 6, 8, 2.5, 3, 14, 16},
                    ov::Shape{2},
                    std::vector<T>{-0.5, -2}),
        PreluParams(ov::Shape{3, 2},
                    IN_ET,
                    std::vector<T>{-2, 3, -2, 1, -1, 0},
                    std::vector<T>{1, 3, 1, 1, 0.5, 0},
                    ov::Shape{2},
                    std::vector<T>{-0.5, -1},
                    "negative_slope"),
        PreluParams(ov::Shape{2, 6},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 5, 6, 1, 2, -3, -4, 5, 6},
                    std::vector<T>{1, 2, -6, -8, 5, 6, 1, 2, -12, -8, 5, 6},
                    ov::Shape{2, 6},
                    std::vector<T>{2, 2, 2, 2, 2, 2, 1, 1, 4, 2, 1, 1}),
        PreluParams(ov::Shape{2, 2, 2, 2},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 1, 2, -3, -4, 1, 2, -3, -4, 1, 2, -3, -4},
                    std::vector<T>{1, 2, -3, -8, 1, 2, -9, -16, 1, 2, -3, -8, 1, 2, -9, -16},
                    ov::Shape{2, 1, 2},
                    std::vector<T>{1, 2, 3, 4}),
        PreluParams(ov::Shape{2, 2, 2, 2},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 1, 2, -3, -4, 1, 2, -3, -4, 1, 2, -3, -4},
                    std::vector<T>{1, 2, -3, -8, 1, 2, -9, -16, 1, 2, -3, -8, 1, 2, -9, -16},
                    ov::Shape{1, 2, 1, 2},
                    std::vector<T>{1, 2, 3, 4}),
        PreluParams(
            ov::Shape{2, 2, 6},
            IN_ET,
            std::vector<T>{1, 2, -3, -4, -5, 6, -1, -2, -3, -4, -5, -6, 1, 2, -3, -4, 5, 6, -2, 4, -6, -8, 10, 12},
            std::vector<T>{1, 2, -9, -16, -5, 6, -2, -2, -9,  -16, -5, -42,
                           1, 2, -9, -16, 5,  6, -2, 4,  -18, -32, 10, 12},
            ov::Shape{2, 1, 6},
            std::vector<T>{2, 1, 3, 4, 1, 7, 1, 2, 3, 4, 5, 6}),
        PreluParams(ov::Shape{2, 3, 2},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, -5, 6, -1, -2, -3, -4, -5, -6},
                    std::vector<T>{1, 2, -9, -16, -5, 6, -1, -4, -9, -16, -25, -36},
                    ov::Shape{2, 3, 2},
                    std::vector<T>{2, 1, 3, 4, 1, 7, 1, 2, 3, 4, 5, 6}),
        PreluParams(ov::Shape{2, 1, 2},
                    IN_ET,
                    std::vector<T>{-10, -10, -10, -10},
                    std::vector<T>{-1, -100, -1, -100},
                    ov::Shape{2},
                    std::vector<T>{0.1, 10}),
        PreluParams(ov::Shape{1, 2, 1, 2},
                    IN_ET,
                    std::vector<T>{-10, -10, -10, -10},
                    std::vector<T>{-1, -1, -100, -100},
                    ov::Shape{2},
                    std::vector<T>{0.1, 10}),
        PreluParams(ov::Shape{1, 5, 1, 1},
                    IN_ET,
                    std::vector<T>{-1, 0, -1, -1, -1},
                    std::vector<T>{-1, 0, -3, -4, -5},
                    ov::Shape{5},
                    std::vector<T>{1, 2, 3, 4, 5}),
        PreluParams(
            ov::Shape{2, 3, 4, 5},
            IN_ET,
            std::vector<T>{-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.},
            std::vector<T>{-0.,   -1.,   -2.,   -3.,   -4.,   -5.,   -6.,   -7.,   -8.,   -9.,   -10.,  -11.,
                           -12.,  -13.,  -14.,  -15.,  -16.,  -17.,  -18.,  -19.,  -20.,  -21.,  -22.,  -23.,
                           -24.,  -25.,  -26.,  -27.,  -28.,  -29.,  -30.,  -31.,  -32.,  -33.,  -34.,  -35.,
                           -36.,  -37.,  -38.,  -39.,  -40.,  -41.,  -42.,  -43.,  -44.,  -45.,  -46.,  -47.,
                           -48.,  -49.,  -50.,  -51.,  -52.,  -53.,  -54.,  -55.,  -56.,  -57.,  -58.,  -59.,
                           -60.,  -61.,  -62.,  -63.,  -64.,  -65.,  -66.,  -67.,  -68.,  -69.,  -70.,  -71.,
                           -72.,  -73.,  -74.,  -75.,  -76.,  -77.,  -78.,  -79.,  -80.,  -81.,  -82.,  -83.,
                           -84.,  -85.,  -86.,  -87.,  -88.,  -89.,  -90.,  -91.,  -92.,  -93.,  -94.,  -95.,
                           -96.,  -97.,  -98.,  -99.,  -100., -101., -102., -103., -104., -105., -106., -107.,
                           -108., -109., -110., -111., -112., -113., -114., -115., -116., -117., -118., -119.},
            ov::Shape{2, 3, 4, 5},
            std::vector<T>{0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,
                           18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,
                           36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,
                           54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,
                           72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
                           90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107,
                           108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119}),
        PreluParams(
            ov::Shape{2, 3, 4, 5},
            IN_ET,
            std::vector<T>{-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.},
            std::vector<T>{-0., -1., -2., -3., -4., -0., -1., -2., -3., -4., -0., -1., -2., -3., -4., -0., -1., -2.,
                           -3., -4., -0., -1., -2., -3., -4., -0., -1., -2., -3., -4., -0., -1., -2., -3., -4., -0.,
                           -1., -2., -3., -4., -0., -1., -2., -3., -4., -0., -1., -2., -3., -4., -0., -1., -2., -3.,
                           -4., -0., -1., -2., -3., -4., -0., -1., -2., -3., -4., -0., -1., -2., -3., -4., -0., -1.,
                           -2., -3., -4., -0., -1., -2., -3., -4., -0., -1., -2., -3., -4., -0., -1., -2., -3., -4.,
                           -0., -1., -2., -3., -4., -0., -1., -2., -3., -4., -0., -1., -2., -3., -4., -0., -1., -2.,
                           -3., -4., -0., -1., -2., -3., -4., -0., -1., -2., -3., -4.},
            ov::Shape{5},
            std::vector<T>{0, 1, 2, 3, 4}),
        PreluParams(
            ov::Shape{2, 3, 4, 5},
            IN_ET,
            std::vector<T>{-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.},
            std::vector<T>{-0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0.,
                           -0., -0., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2.,
                           -2., -2., -2., -2., -2., -2., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0.,
                           -0., -0., -0., -0., -0., -0., -0., -0., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -2., -2., -2., -2., -2., -2., -2., -2.,
                           -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2.},
            ov::Shape{3},
            std::vector<T>{0, 1, 2}),
        PreluParams(
            ov::Shape{2, 3, 4, 5},
            IN_ET,
            std::vector<T>{-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.},
            std::vector<T>{-0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0.,
                           -0., -0., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2.,
                           -2., -2., -2., -2., -2., -2., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0., -0.,
                           -0., -0., -0., -0., -0., -0., -0., -0., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
                           -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -2., -2., -2., -2., -2., -2., -2., -2.,
                           -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2., -2.},
            ov::Shape{3, 1, 1},
            std::vector<T>{0, 1, 2})};
    return preluParams;
}

template <element::Type_t IN_ET>
std::vector<PreluParams> generatePreluI8Params() {
    using T = typename element_type_traits<IN_ET>::value_type;

    std::vector<PreluParams> preluParams{
        PreluParams(ov::Shape{6},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 5, 6},
                    std::vector<T>{1, 2, -6, -8, 5, 6},
                    ov::Shape{1},
                    std::vector<T>{2}),
        PreluParams(ov::Shape{6},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 5, 6},
                    std::vector<T>{1, 2, -12, -20, 5, 6},
                    ov::Shape{6},
                    std::vector<T>{2, 3, 4, 5, 6, 7}),
        PreluParams(ov::Shape{3, 2},
                    IN_ET,
                    std::vector<T>{-2, 3, -2, 1, -1, 0},
                    std::vector<T>{0, 3, 0, 1, 0, 0},
                    ov::Shape{2},
                    std::vector<T>{0, 1}),
        PreluParams(ov::Shape{2, 6},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 5, 6, 7, 8, -9, -10, 11, 12},
                    std::vector<T>{1, 2, -9, -16, 5, 6, 7, 8, -27, -40, 11, 12},
                    ov::Shape{6},
                    std::vector<T>{1, 2, 3, 4, 5, 6}),
        PreluParams(ov::Shape{2, 6},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 5, 6, 1, 2, -3, -4, 5, 6},
                    std::vector<T>{1, 2, -6, -8, 5, 6, 1, 2, -12, -8, 5, 6},
                    ov::Shape{2, 6},
                    std::vector<T>{2, 2, 2, 2, 2, 2, 1, 1, 4, 2, 1, 1}),
        PreluParams(ov::Shape{2, 2, 2, 2},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 1, 2, -3, -4, 1, 2, -3, -4, 1, 2, -3, -4},
                    std::vector<T>{1, 2, -3, -8, 1, 2, -9, -16, 1, 2, -3, -8, 1, 2, -9, -16},
                    ov::Shape{2, 1, 2},
                    std::vector<T>{1, 2, 3, 4}),
        PreluParams(ov::Shape{2, 2, 2, 2},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, 1, 2, -3, -4, 1, 2, -3, -4, 1, 2, -3, -4},
                    std::vector<T>{1, 2, -3, -8, 1, 2, -9, -16, 1, 2, -3, -8, 1, 2, -9, -16},
                    ov::Shape{1, 2, 1, 2},
                    std::vector<T>{1, 2, 3, 4}),
        PreluParams(
            ov::Shape{2, 2, 6},
            IN_ET,
            std::vector<T>{1, 2, -3, -4, -5, 6, -1, -2, -3, -4, -5, -6, 1, 2, -3, -4, 5, 6, -2, 4, -6, -8, 10, 12},
            std::vector<T>{1, 2, -9, -16, -5, 6, -2, -2, -9,  -16, -5, -42,
                           1, 2, -9, -16, 5,  6, -2, 4,  -18, -32, 10, 12},
            ov::Shape{2, 1, 6},
            std::vector<T>{2, 1, 3, 4, 1, 7, 1, 2, 3, 4, 5, 6}),
        PreluParams(ov::Shape{2, 3, 2},
                    IN_ET,
                    std::vector<T>{1, 2, -3, -4, -5, 6, -1, -2, -3, -4, -5, -6},
                    std::vector<T>{1, 2, -9, -16, -5, 6, -1, -4, -9, -16, -25, -36},
                    ov::Shape{2, 3, 2},
                    std::vector<T>{2, 1, 3, 4, 1, 7, 1, 2, 3, 4, 5, 6}),
        PreluParams(ov::Shape{1, 5, 1, 1},
                    IN_ET,
                    std::vector<T>{-1, 0, -1, -1, -1},
                    std::vector<T>{-1, 0, -3, -4, -5},
                    ov::Shape{5},
                    std::vector<T>{1, 2, 3, 4, 5}),
        PreluParams(
            ov::Shape{2, 3, 4, 5},
            IN_ET,
            std::vector<T>{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                           -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                           -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                           -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                           -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                           -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
            std::vector<T>{-0,   -1,   -2,   -3,   -4,   -5,   -6,   -7,   -8,   -9,   -10,  -11,  -12,  -13,  -14,
                           -15,  -16,  -17,  -18,  -19,  -20,  -21,  -22,  -23,  -24,  -25,  -26,  -27,  -28,  -29,
                           -30,  -31,  -32,  -33,  -34,  -35,  -36,  -37,  -38,  -39,  -40,  -41,  -42,  -43,  -44,
                           -45,  -46,  -47,  -48,  -49,  -50,  -51,  -52,  -53,  -54,  -55,  -56,  -57,  -58,  -59,
                           -60,  -61,  -62,  -63,  -64,  -65,  -66,  -67,  -68,  -69,  -70,  -71,  -72,  -73,  -74,
                           -75,  -76,  -77,  -78,  -79,  -80,  -81,  -82,  -83,  -84,  -85,  -86,  -87,  -88,  -89,
                           -90,  -91,  -92,  -93,  -94,  -95,  -96,  -97,  -98,  -99,  -100, -101, -102, -103, -104,
                           -105, -106, -107, -108, -109, -110, -111, -112, -113, -114, -115, -116, -117, -118, -119},
            ov::Shape{2, 3, 4, 5},
            std::vector<T>{0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,
                           18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,
                           36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,
                           54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,
                           72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
                           90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107,
                           108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119}),
        PreluParams(ov::Shape{2, 3, 4, 5},
                    IN_ET,
                    std::vector<T>{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                    std::vector<T>{-0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4,
                                   -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4,
                                   -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4,
                                   -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4,
                                   -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4,
                                   -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4, -0, -1, -2, -3, -4},
                    ov::Shape{5},
                    std::vector<T>{0, 1, 2, 3, 4}),
        PreluParams(ov::Shape{2, 3, 4, 5},
                    IN_ET,
                    std::vector<T>{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                    std::vector<T>{-0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
                                   -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2},
                    ov::Shape{3},
                    std::vector<T>{0, 1, 2}),
        PreluParams(ov::Shape{2, 3, 4, 5},
                    IN_ET,
                    std::vector<T>{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                    std::vector<T>{-0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
                                   -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0, -0,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2},
                    ov::Shape{3, 1, 1},
                    std::vector<T>{0, 1, 2})};
    return preluParams;
}

std::vector<PreluParams> generatePreluCombinedParams() {
    const std::vector<std::vector<PreluParams>> preluTypeParams{generatePreluFloatParams<element::Type_t::f32>(),
                                                                generatePreluFloatParams<element::Type_t::f16>(),
                                                                generatePreluFloatParams<element::Type_t::bf16>(),
                                                                generatePreluI8Params<element::Type_t::i8>()};
    std::vector<PreluParams> combinedParams;

    for (const auto& params : preluTypeParams) {
        combinedParams.insert(combinedParams.end(), params.begin(), params.end());
    }
    return combinedParams;
}

INSTANTIATE_TEST_SUITE_P(smoke_Prelu_With_Hardcoded_Refs,
                         ReferencePreluLayerTest,
                         testing::ValuesIn(generatePreluCombinedParams()),
                         ReferencePreluLayerTest::getTestCaseName);

}  // namespace
