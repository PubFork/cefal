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

#include <tuple>

namespace cefal {
namespace detail {
template <typename...>
struct SimpleInnerType;
template <template <typename...> typename C, typename T, typename... Ts>
struct SimpleInnerType<C<T, Ts...>> {
    using type = T;
};

template <typename T>
struct FullDecay {
    using type = std::remove_cvref_t<T>;
};
template <typename T1, typename T2>
struct FullDecay<std::pair<T1, T2>> {
    using type = std::pair<std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>;
};
template <typename... T>
struct FullDecay<std::tuple<T...>> {
    using type = std::tuple<std::remove_cvref_t<T>...>;
};
} // namespace detail

template <typename T>
struct InnerType {
    using type = detail::SimpleInnerType<T>::type;
};
template <typename T>
using InnerType_T = InnerType<T>::type;

template <typename T>
struct NakedInnerType {
    using type = detail::FullDecay<typename InnerType<T>::type>::type;
};
template <typename T>
using NakedInnerType_T = NakedInnerType<T>::type;

template <typename...>
struct WithInnerType;
template <template <typename...> typename C, typename T, typename... Ts, typename NewT>
struct WithInnerType<C<T, Ts...>, NewT> {
    using type = C<NewT>;
};
template <typename... Ts>
using WithInnerType_T = WithInnerType<Ts...>::type;
} // namespace cefal
