/*******************************************************************************
 *
 * MIT License
 *
 * Copyright (c) 2023 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/
#include <tuple>
#include <string_view>

#include "gtest_common.hpp"
#include "get_handle.hpp"

#include "../conv2d.hpp"

namespace {

auto GetTestCases()
{
    const auto env = std::tuple{
        std::pair{ENV(MIOPEN_FIND_ENFORCE), std::string_view("SEARCH_DB_UPDATE")},
        std::pair{ENV(MIOPEN_DEBUG_TUNING_ITERATIONS_MAX), std::string_view("5")},
        std::pair{ENV(MIOPEN_DEBUG_CONVOLUTION_ATTRIB_FP16_ALT_IMPL), std::string_view("0")},
        std::pair{ENV(MIOPEN_FIND_MODE), std::string_view("normal")},
        std::pair{ENV(MIOPEN_DEBUG_FIND_ONLY_SOLVER), std::string_view("ConvBinWinogradRxSf2x3")}};

    return std::vector{
        // clang-format off
    std::pair{env, std::string(" --input 1 40 20 20 --weights 20 20 3 3 --pads_strides_dilations 1 1 1 1 1 1 --group-count 2")}
        // clang-format on
    };
}

using TestCase = decltype(GetTestCases())::value_type;

bool SkipTest() { return get_handle_xnack(); }

bool IsTestSupportedForDevice()
{
    using e_mask = enabled<Gpu::gfx94X, Gpu::gfx103X>;
    using d_mask = disabled<Gpu::gfx900>;
    return ::IsTestSupportedForDevMask<d_mask, e_mask>();
}

} // namespace

class Conv2dAltTuningFloat : public FloatTestCase<std::vector<TestCase>>
{
};

class Conv2dAltTuningHalf : public HalfTestCase<std::vector<TestCase>>
{
};

TEST_P(Conv2dAltTuningFloat, FloatTest_smoke_solver_ConvBinWinogradRxSf2x3)
{
    if(IsTestSupportedForDevice() && !SkipTest())
    {
        invoke_with_params<conv2d_driver, Conv2dAltTuningFloat>(tuning_check);
    }
    else
    {
        GTEST_SKIP();
    }
};

TEST_P(Conv2dAltTuningHalf, HalfTest_smoke_solver_ConvBinWinogradRxSf2x3)
{
    if(IsTestSupportedForDevice() && !SkipTest())
    {
        invoke_with_params<conv2d_driver, Conv2dAltTuningHalf>(tuning_check);
    }
    else
    {
        GTEST_SKIP();
    }
};

INSTANTIATE_TEST_SUITE_P(SmokeSolverConvBinWinogradRxSf2x3,
                         Conv2dAltTuningFloat,
                         testing::Values(GetTestCases()));

INSTANTIATE_TEST_SUITE_P(SmokeSolverConvBinWinogradRxSf2x3,
                         Conv2dAltTuningHalf,
                         testing::Values(GetTestCases()));
