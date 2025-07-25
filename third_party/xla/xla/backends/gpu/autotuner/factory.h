/* Copyright 2025 The OpenXLA Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef XLA_BACKENDS_GPU_AUTOTUNER_FACTORY_H_
#define XLA_BACKENDS_GPU_AUTOTUNER_FACTORY_H_

#include <memory>
#include <vector>

#include "xla/backends/autotuner/codegen_backend.h"
#include "xla/backends/gpu/autotuner/block_level_emitter.h"
#include "xla/backends/gpu/autotuner/cublas.h"
#include "xla/backends/gpu/autotuner/triton.h"
#include "xla/service/compiler.h"
#include "xla/stream_executor/stream_executor.h"

namespace xla {

namespace gpu {

// TODO: b/407494793 - Add support for ROCM, currently assumes CUDA.
inline std::vector<std::unique_ptr<CodegenBackend>> GetAllGpuCodegenBackends(
    stream_executor::StreamExecutor* stream_executor,
    const DebugOptions* debug_options, Compiler* compiler) {
  std::vector<std::unique_ptr<CodegenBackend>> backends;
  backends.push_back(std::make_unique<TritonBackend>(stream_executor,
                                                     debug_options, compiler));
  backends.push_back(std::make_unique<CublasBackend>(stream_executor,
                                                     debug_options, compiler));
  backends.push_back(std::make_unique<BlockLevelEmitterBackend>(
      stream_executor, debug_options, compiler));
  /*
  TODO(b/407494793): Enable backends as they are ready and verified.
  backends.push_back(std::make_unique<CublasLtBackend>(
      stream_executor, debug_options, compiler));
  backends.push_back(std::make_unique<CudnnBackend>(
      stream_executor, debug_options, compiler));
  backends.push_back(std::make_unique<CustomKernelBackend>(
      stream_executor, debug_options, compiler));
  */
  /* TODO(b/407494793) : Enable FissionBackend which can rewrite fusions.
  backends.push_back(std::make_unique<FissionBackend>(
      stream_executor, debug_options, compiler));
  */
  return backends;
}

}  // namespace gpu
}  // namespace xla

#endif  // XLA_BACKENDS_GPU_AUTOTUNER_FACTORY_H_
