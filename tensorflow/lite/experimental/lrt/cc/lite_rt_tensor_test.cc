// Copyright 2024 Google LLC.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tensorflow/lite/experimental/lrt/cc/lite_rt_tensor.h"

#include <gtest/gtest.h>
#include "absl/types/span.h"
#include "tensorflow/lite/experimental/lrt/c/lite_rt_model.h"
#include "tensorflow/lite/experimental/lrt/core/graph_tools.h"
#include "tensorflow/lite/experimental/lrt/test_data/test_data_util.h"

namespace {

using ::lrt::LrtTensorManager;

TEST(TestLrtTensorManager, SimpleRankedTensorSubgraphInput) {
  auto model = LoadTestFileModel("one_mul.tflite");

  ASSERT_RESULT_OK_ASSIGN(auto subgraph,
                          ::graph_tools::GetSubgraph(model.get()));
  ASSERT_RESULT_OK_ASSIGN(auto inputs,
                          ::graph_tools::GetSubgraphInputs(subgraph));

  LrtTensorManager::Unique tensor;
  ASSERT_STATUS_OK(LrtTensorManager::MakeFromTensor(inputs[0], tensor));

  ASSERT_EQ(tensor->Rank(), 2);
  EXPECT_EQ(tensor->Dims(), absl::MakeConstSpan({2, 2}));
  EXPECT_EQ(tensor->ElementType(), kLrtElementTypeFloat32);
  EXPECT_EQ(tensor->Tensor(), inputs[0]);
  EXPECT_TRUE(tensor->IsSubgraphInput());
  EXPECT_FALSE(tensor->IsSubgraphOutput());
}

TEST(TestLrtTensorManager, SimpleRankedTensorSubgraphOutput) {
  auto model = LoadTestFileModel("one_mul.tflite");

  ASSERT_RESULT_OK_ASSIGN(auto subgraph,
                          ::graph_tools::GetSubgraph(model.get()));
  ASSERT_RESULT_OK_ASSIGN(auto outputs,
                          ::graph_tools::GetSubgraphOutputs(subgraph));

  LrtTensorManager::Unique tensor;
  ASSERT_STATUS_OK(LrtTensorManager::MakeFromTensor(outputs[0], tensor));

  ASSERT_EQ(tensor->Rank(), 2);
  EXPECT_EQ(tensor->Dims(), absl::MakeConstSpan({2, 2}));
  EXPECT_EQ(tensor->ElementType(), kLrtElementTypeFloat32);
  EXPECT_EQ(tensor->Tensor(), outputs[0]);
  EXPECT_TRUE(tensor->IsSubgraphOutput());
  EXPECT_FALSE(tensor->IsSubgraphInput());
}

TEST(TestLrtTensorManager, SimpleRankedTensor) {
  auto model = LoadTestFileModel("simple_multi_op.tflite");

  ASSERT_RESULT_OK_ASSIGN(auto subgraph,
                          ::graph_tools::GetSubgraph(model.get()));
  ASSERT_RESULT_OK_ASSIGN(auto ops, ::graph_tools::GetSubgraphOps(subgraph));
  ASSERT_RESULT_OK_ASSIGN(auto op_outs, ::graph_tools::GetOpOuts(ops[1]));

  LrtTensorManager::Unique tensor;
  ASSERT_STATUS_OK(LrtTensorManager::MakeFromTensor(op_outs[0], tensor));

  ASSERT_EQ(tensor->Rank(), 2);
  EXPECT_EQ(tensor->Dims(), absl::MakeConstSpan({2, 2}));
  EXPECT_EQ(tensor->ElementType(), kLrtElementTypeFloat32);
  EXPECT_EQ(tensor->Tensor(), op_outs[0]);
  EXPECT_FALSE(tensor->IsSubgraphOutput());
  EXPECT_FALSE(tensor->IsSubgraphInput());
}
}  // namespace
