#pragma once

#include <torch/csrc/utils/variadic.h>
#include <torch/nn/cloneable.h>
#include <torch/nn/pimpl.h>
#include <torch/types.h>
#include <torch/csrc/WindowsTorchApiMacro.h>

#include <functional>
#include <utility>

namespace torch {
namespace nn {

/// Wraps a function in a `Module`.
///
/// The `Functional` module allows wrapping an arbitrary function or function
/// object in an `nn::Module`. This is primarily handy for usage in
/// `Sequential`.
///
/// \rst
/// .. code-block:: cpp
///
///   Sequential sequential(
///     Linear(3, 4),
///     Functional(torch::relu),
///     BatchNorm(3),
///     Functional(torch::elu, /*alpha=*/1));
/// \endrst
///
/// While a `Functional` module only accepts a single `Tensor` as input, it is
/// possible for the the wrapped function to accept further arguments. However,
/// these have to be bound *at construction time*. For example, if
/// you want to wrap `torch::leaky_relu`, which accepts a `slope` scalar as its
/// second argument, with a particular value for its `slope` in a `Functional`
/// module, you could write
///
/// \rst
/// .. code-block:: cpp
///
///   Functional(torch::leaky_relu, /*slope=*/0.5)
/// \endrst
///
/// The value of `0.5` is then stored within the `Functional` object and supplied
/// to the function call at invocation time. Note that such bound values are
/// evaluated eagerly and stored a single time. See the documentation of
/// [std::bind](https://en.cppreference.com/w/cpp/utility/functional/bind) for
/// more information on the semantics of argument binding.
///
/// \rst
/// .. attention::
///   After passing any bound arguments, the function must accept a single
///   tensor and return a single tensor.
/// \endrst
///
/// Note that `Functional` overloads the call operator (`operator()`) such that
/// you can invoke it with `my_func(...)`.
class FunctionalImpl : public torch::nn::Cloneable<FunctionalImpl> {
 public:
  explicit FunctionalImpl(std::function<Tensor(Tensor)> function);

  void reset() override;

  /// Pretty prints the `Functional` module into the given `stream`.
  void pretty_print(std::ostream& stream) const override;

  /// Forwards the `input` tensor to the underlying (bound) function object.
  Tensor forward(Tensor input);

  /// Calls forward(input).
  Tensor operator()(Tensor input);

  bool is_serializable() const override;

 private:
  std::function<Tensor(Tensor)> function_;
};

/// A `ModuleHolder` subclass for `FunctionalImpl`.
/// See the documentation for `FunctionalImpl` class to learn what methods it
/// provides, or the documentation for `ModuleHolder` to learn about PyTorch's
/// module storage semantics.
TORCH_MODULE(Functional);

} // namespace nn
} // namespace torch
