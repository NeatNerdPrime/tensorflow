// RUN: tf-quant-opt %s -split-input-file -tf-quant-preprocess-op | FileCheck %s

module {
  // For UniformQuantized depthwise convolution, tensor shape should have
  // transformed from [H,W,C,M] to [H,W,1,CxM],
  func.func @depthwise_conv(%arg0: tensor<1x3x4x3xf32>) -> (tensor<*xf32>) {
    %cst_0 = "tf.Const"() {value = dense<0.000000e+00> : tensor<6xf32>} : () -> tensor<6xf32>
    %cst_1 = "tf.Const"() {value = dense<[[[[3.0, 2.0], [1.0, 0.0],[3.0, 2.0]],[[3.0, 2.0], [1.0, 0.0],[3.0, 2.0]],[[3.0, 2.0], [1.0, 0.0],[3.0, 2.0]]],[[[3.0, 2.0], [1.0, 0.0],[3.0, 2.0]],[[3.0, 2.0], [1.0, 0.0],[3.0, 2.0]],[[3.0, 2.0], [1.0, 0.0],[3.0, 2.0]]]]> : tensor<2x3x3x2xf32>} : () -> tensor<2x3x3x2xf32>
    %0 = "tf.PartitionedCall"(%arg0, %cst_1) {_tfl_quant_trait = "fully_quantizable", config = "", config_proto = "", executor_type = "", f = @composite_depthwise_conv2d_fn} : (tensor<1x3x4x3xf32>, tensor<2x3x3x2xf32>) -> tensor<*xf32>
    %1 = "tf.BiasAdd"(%0, %cst_0) {data_format = "NHWC", device = ""} : (tensor<*xf32>, tensor<6xf32>) -> tensor<*xf32>
    func.return %1: tensor<*xf32>
  }
  func.func private @composite_depthwise_conv2d_fn(%arg0: tensor<1x3x4x3xf32>, %arg1: tensor<2x3x3x2xf32>) -> tensor<*xf32> attributes {tf_quant.composite_function} {
    %0 = "tf.DepthwiseConv2dNative"(%arg0, %arg1) {
      attr_map = "0:strides,1:padding,2:explicit_paddings,3:dilations", data_format = "NHWC", device = "", dilations = [1, 1, 1, 1], explicit_paddings = [], padding = "SAME", strides = [1, 1, 2, 1]
    } : (tensor<1x3x4x3xf32>, tensor<2x3x3x2xf32>) -> tensor<*xf32>
    return %0 : tensor<*xf32>
  }

// CHECK-LABEL: func @depthwise_conv
// CHECK-DAG: %[[CONST_0:.*]] = arith.constant dense<0.000000e+00> : tensor<6xf32>
// CHECK: %[[CONST_1:.*]] = arith.constant dense
// CHECK-NOT: tensor<2x3x3x2xf32>
// CHECK-SAME: tensor<2x3x1x6xf32>
// CHECK: %[[PARTITIONEDCALL_0:.*]] = "tf.PartitionedCall"(%arg0, %[[CONST_1:.*]]) <{config = "", config_proto = "", executor_type = "", f = @composite_depthwise_conv2d_fn_0}> {_tfl_quant_trait = "fully_quantizable"} : (tensor<1x3x4x3xf32>, tensor<2x3x1x6xf32>) -> tensor<*xf32>
// CHECK: %[[BIAS_0:.*]] = "tf.BiasAdd"(%[[PARTITIONEDCALL_0]], %[[CONST_0:.*]]) <{data_format = "NHWC"}> {device = ""} : (tensor<*xf32>, tensor<6xf32>) -> tensor<*xf32>
// CHECK: return %[[BIAS_0:.*]] : tensor<*xf32>

// CHECK-LABEL: func private @composite_depthwise_conv2d_fn(
// CHECK-SAME:                                             %arg0: tensor<1x3x4x3xf32>,
// CHECK-SAME:                                             %arg1: tensor<2x3x3x2xf32>)

// CHECK-LABEL: func private @composite_depthwise_conv2d_fn_0(
// CHECK-SAME:                                             %arg0: tensor<1x3x4x3xf32>,
// CHECK-SAME:                                             %arg1: tensor<2x3x1x6xf32>)
// CHECK: %0 = "tf.DepthwiseConv2dNative"(%arg0, %arg1) <{data_format = "NHWC", dilations = [1, 1, 1, 1], explicit_paddings = [], padding = "SAME", strides = [1, 1, 2, 1]}> {attr_map = "0:strides,1:padding,2:explicit_paddings,3:dilations", device = ""}
// CHECK: return %0 : tensor<*xf32>
}

