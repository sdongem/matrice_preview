/**************************************************************************
This file is part of Matrice, an effcient and elegant C++ library.
Copyright(C) 2018, Zhilong(Dgelom) Su, all rights reserved.

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
#pragma once
#include <type_traits>
#include <exception>
#include "../util/_macros.h"
#include "../util/utils.h"
#include "../private/_type_traits.h"
#include "../private/_size_traits.h"
#include "../arch/ixpacket.h"
#ifdef __use_mkl__
#include <mkl.h>
#else
#include <fkl.h>
#endif

DGE_MATRICE_BEGIN
_DETAIL_BEGIN
template<typename _Ty> MATRICE_HOST_INL constexpr
_Ty _Det(const _Ty* data, int n, typename std::enable_if<is_float32<_Ty>::value>::type* = 0) { 
	return fblas::_sdetm(static_cast<float*>(data), n);
}
template<typename _Ty> MATRICE_HOST_INL constexpr
_Ty _Det(_Ty* data, int n, typename std::enable_if<is_float64<_Ty>::value>::type* = 0) {
	return fblas::_ddetm(static_cast<double*>(data), n);
}
_DETAIL_END

/**
 * \SIMD supported determinant of a matrix or its derivatives.
 */
template<typename _T, 
	typename = std::enable_if_t<is_matrix_v<_T>||is_expression_v<_T>||is_mtxview_v<_T>>>
MATRICE_HOST_INL constexpr typename _T::value_t det(const _T& _x) {
	return detail::_Det(_x.eval().data(), _x.rows());
}

/**
 * \SIMD supported dot-product of two vectors.
 */
template<typename _T, typename _U, typename value_t = std::common_type_t<typename _T::value_t, typename _U::value_t>>
MATRICE_HOST_INL constexpr value_t dot(const _T& _x, const _U& _y) {
#ifdef _DEBUG
	if (_x.size() != _y.size())
		throw std::runtime_error("Oops, non-consistent size error.");
#endif
	const auto _Left = _x.eval();
	const auto _Right = _y.eval();
	const auto _Size = min(_Left.size(), _Right.size());

	constexpr std::size_t _Stride = packet_size_v;
	const auto _Vsize = simd::vsize<_Stride>(_Size);

	using packet_type = simd::Packet_<float, _Stride>;
	auto _Ret = zero<value_t>::value;
	for (std::size_t _Idx = 0; _Idx < _Vsize; _Idx += _Stride) {
		_Ret += (
			packet_type(_Left.data() + _Idx)*
			packet_type(_Right.data() + _Idx) ).reduce();
	}
	for (std::size_t _Idx = _Vsize; _Idx < _Size; ++_Idx) {
		_Ret += _Left(_Idx)*_Right(_Idx);
	}

	return (_Ret);
}

DGE_MATRICE_END
