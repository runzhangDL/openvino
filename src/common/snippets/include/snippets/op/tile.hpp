// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "ngraph/op/op.hpp"
#include "snippets/emitter.hpp"

namespace ngraph {
namespace snippets {
namespace op {

/**
 * @interface Tile
 * @brief Generated by Canonicalization and represents Loop in affine notation
 * @ingroup snippets
 */
class Tile : public ngraph::op::Op {
public:
    OPENVINO_OP("Tile", "SnippetsOpset");

    /// \brief Construct an Tile
    /// \param region The vector of pairs: emitters and the corresponding registers
    /// \param increment Tile size - count of elements to load and store.
    ///                  Vector Tile should have size of vector register and Scalar Tile should have 1
    /// \param num_inputs Count of inputs
    /// \param num_outputs Count of outputs
    /// \param io_dims Vector of last dimensions of inputs and outputs
    /// \param io_data_sizes Vector of data type sizes of inputs and outputs
    Tile(const std::vector<AllocatedEmitter>& region, size_t increment, size_t num_inputs, size_t num_outputs,
         const std::vector<size_t>& io_dims, const std::vector<size_t>& io_data_sizes);
    Tile() = default;
    std::vector<AllocatedEmitter> region;
    size_t increment = 0;
    size_t num_inputs = 0;
    size_t num_outputs = 0;
    std::vector<size_t> io_dims {};
    std::vector<size_t> io_data_size {};

    std::shared_ptr<Node> clone_with_new_inputs(const OutputVector& inputs) const override {
        return std::make_shared<Tile>(region, increment, num_inputs, num_outputs, io_dims, io_data_size);
    }
};

} // namespace op
} // namespace snippets
} // namespace ngraph