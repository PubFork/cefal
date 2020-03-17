/* Copyright 2020, Dennis Kormalev
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *     * Neither the name of the copyright holders nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include "cefal/common.h"
#include "cefal/functor.h"

#include <concepts>
#include <functional>
#include <utility>

namespace cefal {
namespace instances {
template <typename T>
struct Monad;
} // namespace instances

namespace concepts {
// clang-format off
template <typename M, typename InnerT = InnerType_T<std::remove_cvref_t<M>>, typename CleanM = std::remove_cvref_t<M>>
concept Monad =
Functor<CleanM, InnerT> && requires (CleanM m, std::function<CleanM(InnerT)> converter) {
  {instances::Monad<CleanM>::flatMap(m, std::move(converter))} -> std::same_as<CleanM>;
  {instances::Monad<CleanM>::flatMap(std::move(m), std::move(converter))} -> std::same_as<CleanM>;
};
// clang-format on
} // namespace concepts

namespace ops {
template <typename FuncT>
struct flatMap {
    using Func = FuncT;
    flatMap(Func&& func) : func(std::move(func)) {}
    flatMap(const Func& func) : func(func) {}
    template <concepts::Monad M>
    inline auto operator()(M&& m) && {
        return instances::Monad<std::remove_cvref_t<M>>::flatMap(std::forward<M>(m), std::move(func));
    }

private:
    Func func;
};

template <typename Left, typename Func>
inline auto operator|(Left&& left, flatMap<Func>&& op) {
    return std::move(op)(std::forward<Left>(left));
}

template <typename FuncT>
struct innerFlatMap {
    using Func = FuncT;
    innerFlatMap(Func&& func) : func(std::move(func)) {}
    innerFlatMap(const Func& func) : func(func) {}
    template <concepts::Functor F, concepts::Monad InnerM = InnerType_T<std::remove_cvref_t<F>>>
    inline auto operator()(F&& f) && {
        return std::forward<F>(f) | map([this]<typename T>(T&& x) { return std::forward<T>(x) | flatMap(func); });
    }

private:
    Func func;
};

template <typename Left, typename Func>
inline auto operator|(Left&& left, innerFlatMap<Func>&& op) {
    return std::move(op)(std::forward<Left>(left));
}
} // namespace ops
} // namespace cefal