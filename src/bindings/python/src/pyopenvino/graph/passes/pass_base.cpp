// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "pyopenvino/graph/passes/pass_base.hpp"

#include <pybind11/pybind11.h>

#include <memory>
#include <openvino/pass/pass.hpp>

namespace py = pybind11;

void regclass_passes_PassBase(py::module m) {
    py::class_<ov::pass::PassBase, std::shared_ptr<ov::pass::PassBase>> pass_base(m, "PassBase");
    pass_base.doc() = "openvino.runtime.passes.PassBase wraps ov::pass::PassBase";
    pass_base.def("set_name",
                  &ov::pass::PassBase::set_name,
                  py::arg("name"),
                  R"(
                  Set transformation name.

                  :param name: Transformation name.
                  :type name: str
    )");
    pass_base.def("get_name",
                  &ov::pass::PassBase::get_name,
                  R"(
                  Get transformation name.

                  :return: Transformation name.
                  :rtype: str
    )");
}
