// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <vector>

#include "single_layer_tests/generate_proposals.hpp"
#include "common_test_utils/ov_tensor_utils.hpp"

using namespace ov::test;
using namespace ov::test::subgraph;

namespace {

const std::vector<float> min_size = { 1.0f, 0.0f };
const std::vector<float> nms_threshold = { 0.7f };
const std::vector<int64_t> post_nms_count = { 6 };
const std::vector<int64_t> pre_nms_count = { 14, 1000 };

template <typename T>
const std::vector<std::pair<std::string, std::vector<ov::Tensor>>> getInputTensors() {
    const std::vector<std::pair<std::string, std::vector<ov::Tensor>>> input_tensors = {
        {
            "empty",
            {
                // 3
                ov::test::utils::create_tensor<T>(ov::element::from<T>(), ov::Shape{2, 3}, {1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f}),
                // 2 x 6 x 3 x 4 = 144
                ov::test::utils::create_tensor<T>(ov::element::from<T>(), ov::Shape{2, 6, 3, 4}, std::vector<T>(144, 1.0f)),
                // 2 x 12 x 2 x 6 = 144 * 2
                ov::test::utils::create_tensor<T>(ov::element::from<T>(), ov::Shape{2, 12, 2, 6}, std::vector<T>(288, 1.0f)),
                // {2 x 3 x 2 x 6} = 36 * 2
                ov::test::utils::create_tensor<T>(ov::element::from<T>(), ov::Shape{2, 3, 2, 6}, {
                        5.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 4.0f, 1.0f, 1.0f, 1.0f,
                        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 1.0f,
                        5.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 4.0f, 1.0f, 1.0f, 1.0f,
                        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 1.0f})
            }
    },
    {
        "filled2",
        {
            ov::test::utils::create_tensor<T>(ov::element::from<T>(), ov::Shape{2, 3}, {200.0, 200.0, 4.0, 200.0, 200.0, 4.0}),
            ov::test::utils::create_tensor<T>(ov::element::from<T>(), ov::Shape{2, 6, 3, 4}, {0.0f,  1.0f,  2.0f,  3.0f,
                                                                                            4.0f,  5.0f,  6.0f,  7.0f,
                                                                                            8.0f,  9.0f, 10.0f, 11.0f,
                                                                                            12.0f, 13.0f, 14.0f, 15.0f,
                                                                                            16.0f, 17.0f, 18.0f, 19.0f,
                                                                                            20.0f, 21.0f, 22.0f, 23.0f,
                                                                                            24.0f, 25.0f, 26.0f, 27.0f,
                                                                                            28.0f, 29.0f, 30.0f, 31.0f,
                                                                                            32.0f, 33.0f, 34.0f, 35.0f,
                                                                                            36.0f, 37.0f, 38.0f, 39.0f,
                                                                                            40.0f, 41.0f, 42.0f, 43.0f,
                                                                                            44.0f, 45.0f, 46.0f, 47.0f,
                                                                                            48.0f, 49.0f, 50.0f, 51.0f,
                                                                                            52.0f, 53.0f, 54.0f, 55.0f,
                                                                                            56.0f, 57.0f, 58.0f, 59.0f,
                                                                                            60.0f, 61.0f, 62.0f, 63.0f,
                                                                                            64.0f, 65.0f, 66.0f, 67.0f,
                                                                                            68.0f, 69.0f, 70.0f, 71.0f,
                                                                                            72.0f, 73.0f, 74.0f, 75.0f,
                                                                                            76.0f, 77.0f, 78.0f, 79.0f,
                                                                                            80.0f, 81.0f, 82.0f, 83.0f,
                                                                                            84.0f, 85.0f, 86.0f, 87.0f,
                                                                                            88.0f, 89.0f, 90.0f, 91.0f,
                                                                                            92.0f, 93.0f, 94.0f, 95.0f,
                                                                                            96.0f,  97.0f,  98.0f,  99.0f,
                                                                                            100.0f, 101.0f, 102.0f, 103.0f,
                                                                                            104.0f, 105.0f, 106.0f, 107.0f,
                                                                                            108.0f, 109.0f, 110.0f, 111.0f,
                                                                                            112.0f, 113.0f, 114.0f, 115.0f,
                                                                                            116.0f, 117.0f, 118.0f, 119.0f,
                                                                                            120.0f, 121.0f, 122.0f, 123.0f,
                                                                                            124.0f, 125.0f, 126.0f, 127.0f,
                                                                                            128.0f, 129.0f, 130.0f, 131.0f,
                                                                                            132.0f, 133.0f, 134.0f, 135.0f,
                                                                                            136.0f, 137.0f, 138.0f, 139.0f,
                                                                                            140.0f, 141.0f, 142.0f, 143.0f}),
            ov::test::utils::create_tensor<T>(ov::element::from<T>(), ov::Shape{2, 12, 2, 6}, {
                    0.5337073,  0.86607957, 0.55151343, 0.21626699, 0.4462629,  0.03985678,
                    0.5157072,  0.9932138,  0.7565954,  0.43803605, 0.802818,   0.14834064,
                    0.53932905, 0.14314,    0.3817048,  0.95075196, 0.05516243, 0.2567484,
                    0.25508744, 0.77438325, 0.43561,    0.2094628,  0.8299043,  0.44982538,
                    0.95615596, 0.5651084,  0.11801951, 0.05352486, 0.9774733,  0.14439464,
                    0.62644225, 0.14370479, 0.54161614, 0.557915,   0.53102225, 0.0840179,
                    0.7249888,  0.9843559,  0.5490522,  0.53788143, 0.822474,   0.3278008,
                    0.39688024, 0.3286012,  0.5117038,  0.04743988, 0.9408995,  0.29885054,
                    0.81039643, 0.85277915, 0.06807619, 0.86430097, 0.36225632, 0.16606331,
                    0.5401001,  0.7541649,  0.11998601, 0.5131829,  0.40606487, 0.327888,
                    0.27721855, 0.6378373,  0.22795396, 0.4961256,  0.3215895,  0.15607187,
                    0.14782153, 0.8908137,  0.8835288,  0.834191,   0.29907143, 0.7983525,
                    0.755875,   0.30837986, 0.0839176,  0.26624718, 0.04371626, 0.09472824,
                    0.20689541, 0.37622106, 0.1083321,  0.1342548,  0.05815459, 0.7676379,
                    0.8105144,  0.92348766, 0.26761323, 0.7183306,  0.8947588,  0.19020908,
                    0.42731014, 0.7473663,  0.85775334, 0.9340091,  0.3278848,  0.755993,
                    0.05307213, 0.39705503, 0.21003333, 0.5625373,  0.66188884, 0.80521655,
                    0.6125863,  0.44678232, 0.97802377, 0.0204936,  0.02686367, 0.7390654,
                    0.74631,    0.58399844, 0.5988792,  0.37413648, 0.5946692,  0.6955776,
                    0.36377597, 0.7891322,  0.40900692, 0.99139464, 0.50169915, 0.41435778,
                    0.17142445, 0.26761186, 0.31591868, 0.14249913, 0.12919712, 0.5418711,
                    0.6523203,  0.50259084, 0.7379765,  0.01171071, 0.94423133, 0.00841132,
                    0.97486794, 0.2921785,  0.7633071,  0.88477814, 0.03563205, 0.50833166,
                    0.01354555, 0.535081,   0.41366324, 0.0694767,  0.9944055,  0.9981207,
                    0.5337073,  0.86607957, 0.55151343, 0.21626699, 0.4462629,  0.03985678,
                    0.5157072,  0.9932138,  0.7565954,  0.43803605, 0.802818,   0.14834064,
                    0.53932905, 0.14314,    0.3817048,  0.95075196, 0.05516243, 0.2567484,
                    0.25508744, 0.77438325, 0.43561,    0.2094628,  0.8299043,  0.44982538,
                    0.95615596, 0.5651084,  0.11801951, 0.05352486, 0.9774733,  0.14439464,
                    0.62644225, 0.14370479, 0.54161614, 0.557915,   0.53102225, 0.0840179,
                    0.7249888,  0.9843559,  0.5490522,  0.53788143, 0.822474,   0.3278008,
                    0.39688024, 0.3286012,  0.5117038,  0.04743988, 0.9408995,  0.29885054,
                    0.81039643, 0.85277915, 0.06807619, 0.86430097, 0.36225632, 0.16606331,
                    0.5401001,  0.7541649,  0.11998601, 0.5131829,  0.40606487, 0.327888,
                    0.27721855, 0.6378373,  0.22795396, 0.4961256,  0.3215895,  0.15607187,
                    0.14782153, 0.8908137,  0.8835288,  0.834191,   0.29907143, 0.7983525,
                    0.755875,   0.30837986, 0.0839176,  0.26624718, 0.04371626, 0.09472824,
                    0.20689541, 0.37622106, 0.1083321,  0.1342548,  0.05815459, 0.7676379,
                    0.8105144,  0.92348766, 0.26761323, 0.7183306,  0.8947588,  0.19020908,
                    0.42731014, 0.7473663,  0.85775334, 0.9340091,  0.3278848,  0.755993,
                    0.05307213, 0.39705503, 0.21003333, 0.5625373,  0.66188884, 0.80521655,
                    0.6125863,  0.44678232, 0.97802377, 0.0204936,  0.02686367, 0.7390654,
                    0.74631,    0.58399844, 0.5988792,  0.37413648, 0.5946692,  0.6955776,
                    0.36377597, 0.7891322,  0.40900692, 0.99139464, 0.50169915, 0.41435778,
                    0.17142445, 0.26761186, 0.31591868, 0.14249913, 0.12919712, 0.5418711,
                    0.6523203,  0.50259084, 0.7379765,  0.01171071, 0.94423133, 0.00841132,
                    0.97486794, 0.2921785,  0.7633071,  0.88477814, 0.03563205, 0.50833166,
                    0.01354555, 0.535081,   0.41366324, 0.0694767,  0.9944055,  0.9981207}),
            ov::test::utils::create_tensor<T>(ov::element::from<T>(), ov::Shape{2, 3, 2, 6}, {
                    0.56637216, 0.90457034, 0.69827306, 0.4353543,  0.47985056, 0.42658508,
                    0.14516132, 0.08081771, 0.1799732,  0.9229515,  0.42420176, 0.50857586,
                    0.82664067, 0.4972319,  0.3752427,  0.56731623, 0.18241242, 0.33252355,
                    0.30608943, 0.6572437,  0.69185436, 0.88646156, 0.36985755, 0.5590753,
                    0.5256446,  0.03342898, 0.1344396,  0.68642473, 0.37953874, 0.32575172,
                    0.21108444, 0.5661886,  0.45378175, 0.62126315, 0.26799858, 0.37272978,
                    0.56637216, 0.90457034, 0.69827306, 0.4353543,  0.47985056, 0.42658508,
                    0.14516132, 0.08081771, 0.1799732,  0.9229515,  0.42420176, 0.50857586,
                    0.82664067, 0.4972319,  0.3752427,  0.56731623, 0.18241242, 0.33252355,
                    0.30608943, 0.6572437,  0.69185436, 0.88646156, 0.36985755, 0.5590753,
                    0.5256446,  0.03342898, 0.1344396,  0.68642473, 0.37953874, 0.32575172,
                    0.21108444, 0.5661886,  0.45378175, 0.62126315, 0.26799858, 0.37272978}),
            }
        }
    };
    return input_tensors;
}

constexpr size_t num_batches = 2;
constexpr size_t height = 2;
constexpr size_t width = 6;
constexpr size_t number_of_anchors = 3;

const std::vector<std::vector<InputShape>> input_shape = {
        // im_info / anchors / boxesdeltas / scores
        static_shapes_to_test_representation({{num_batches, 3},
                                              {height, width, number_of_anchors, 4},
                                              {num_batches, number_of_anchors * 4, height, width},
                                              {num_batches, number_of_anchors, height, width}}),
};



INSTANTIATE_TEST_SUITE_P(
        smoke_GenerateProposalsLayerTest_f16,
        GenerateProposalsLayerTest,
        ::testing::Combine(
                ::testing::ValuesIn(input_shape),
                ::testing::ValuesIn(min_size),
                ::testing::ValuesIn(nms_threshold),
                ::testing::ValuesIn(post_nms_count),
                ::testing::ValuesIn(pre_nms_count),
                ::testing::ValuesIn({true}),
                ::testing::ValuesIn(getInputTensors<ov::float16>()),
                ::testing::ValuesIn({ov::element::Type_t::f16}),
                ::testing::ValuesIn({ov::element::Type_t::i32, ov::element::Type_t::i64}),
                ::testing::Values(CommonTestUtils::DEVICE_GPU)),
        GenerateProposalsLayerTest::getTestCaseName);

INSTANTIATE_TEST_SUITE_P(
        smoke_GenerateProposalsLayerTest_f32,
        GenerateProposalsLayerTest,
        ::testing::Combine(
                ::testing::ValuesIn(input_shape),
                ::testing::ValuesIn(min_size),
                ::testing::ValuesIn(nms_threshold),
                ::testing::ValuesIn(post_nms_count),
                ::testing::ValuesIn(pre_nms_count),
                ::testing::ValuesIn({false}),
                ::testing::ValuesIn(getInputTensors<float>()),
                ::testing::ValuesIn({ov::element::Type_t::f32}),
                ::testing::ValuesIn({ov::element::Type_t::i32, ov::element::Type_t::i64}),
                ::testing::Values(CommonTestUtils::DEVICE_GPU)),
        GenerateProposalsLayerTest::getTestCaseName);

} // namespace
