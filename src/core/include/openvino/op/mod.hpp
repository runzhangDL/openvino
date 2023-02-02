// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "openvino/op/util/binary_elementwise_arithmetic.hpp"

namespace ov {
namespace op {
namespace v1 {
/// \brief Mod returns an element-wise division reminder with two given tensors applying
/// multi-directional broadcast rules.
/// \ingroup ov_ops_cpp_api
class OPENVINO_API Mod : public util::BinaryElementwiseArithmetic {
public:
    OPENVINO_OP("Mod", "opset1", op::util::BinaryElementwiseArithmetic, 1);
    BWDCMP_RTTI_DECLARATION;

    /// \brief Constructs a Mod node.
    Mod() : util::BinaryElementwiseArithmetic(AutoBroadcastType::NUMPY) {}
    ///
    /// \param A - Dividend tensor
    /// \param B - Divisor tensor
    /// \param auto_broadcast Auto broadcast specification
    Mod(const Output<Node>& A,
        const Output<Node>& B,
        const AutoBroadcastSpec& auto_broadcast = AutoBroadcastSpec(AutoBroadcastType::NUMPY));

    std::shared_ptr<Node> clone_with_new_inputs(const OutputVector& new_args) const override;
};
}  // namespace v1
}  // namespace op
}  // namespace ov
