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
along with this program. If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
#pragma once
#include <initializer_list>
#include "../util/_macros.h"
#include "../util/genalgs.h"
#ifdef __AVX__
#include "./inl/_ixops.hpp"

MATRICE_ARCH_BEGIN

// \TEMPLATE base class for simd 
template<typename T, int _Elems> class simd_base_
{
	using Myt_op = detail::Op_<T>;
	typename Myt_op::template adaptor<_Elems> My_op;
public:
	enum { size = _Elems };
	using op_t           = Myt_op;
	using value_t        = T;
	using const_value_t  = std::add_const_t<value_t>;
	using pointer        = std::add_pointer_t<value_t>;
	using const_pointer  = std::add_const_t<pointer>;
	using internal_t     = conditional_t<value_t, size>;
	using const_internal = std::add_const_t<internal_t>;
	using initlist_t     = std::initializer_list<value_t>;
	MATRICE_HOST_FINL simd_base_() noexcept {}
	MATRICE_HOST_FINL simd_base_(const_internal _arg) noexcept : m_data(_arg) {}
	MATRICE_HOST_FINL simd_base_(const_value_t _arg) noexcept : m_data(My_op(_arg)) {}
	MATRICE_HOST_FINL simd_base_(const_pointer _arg) noexcept : m_data(My_op(_arg)) {}

	MATRICE_HOST_FINL auto& operator= (const_value_t _arg) { m_data = My_op(_arg); return(*this); }
	MATRICE_HOST_FINL auto& operator= (const_pointer _arg) { m_data = My_op(_arg); return(*this); }
	MATRICE_HOST_FINL auto& operator= (initlist_t _arg) { m_data = My_op(pointer(_arg.begin())); return(*this); }
	MATRICE_HOST_FINL auto& operator= (const_internal _arg) { m_data = _arg; return(*this); }
	MATRICE_HOST_FINL constexpr auto& operator[](size_t i) { return My_op(m_data)[i]; }
	MATRICE_HOST_FINL constexpr const auto& operator[](size_t i) const { return My_op(m_data)[i]; }
	MATRICE_HOST_FINL auto operator() (pointer data) const { My_op(m_data, data); }
	template<typename _Fwdty>
	MATRICE_HOST_FINL auto operator() (_Fwdty& arg) const { My_op(m_data, arg.data()); }
	MATRICE_HOST_FINL auto data() { return (m_data); }
	MATRICE_HOST_FINL const auto data()const { return (m_data); }
	MATRICE_HOST_FINL auto begin() { return My_op(m_data); }
	MATRICE_HOST_FINL const auto begin()const { return My_op(m_data); }
	MATRICE_HOST_FINL auto end() { return (My_op(m_data)+ size); }
	MATRICE_HOST_FINL const auto end()const { return (My_op(m_data) + size); }
	MATRICE_HOST_FINL auto reduce() { return (My_op + begin()); }
	MATRICE_HOST_FINL const auto reduce()const { return (My_op + begin()); }

	// \unpack to memory block that 'data' point to 
	MATRICE_HOST_FINL auto unpack(pointer data) const { My_op(m_data, data); }
	// \unpack to a container 'arg' with method data()
	template<typename fwdty>
	MATRICE_HOST_FINL auto unpack(fwdty& arg) const { My_op(m_data, arg.data()); }

	// \evaluate the vectorizable size for given 'length'
	MATRICE_HOST_FINL auto vsize(std::size_t _Len) const { return (_Len - _Len%size); }
	
protected:
	template<typename... _Args> MATRICE_HOST_FINL constexpr
	auto _Op(_Args... _args)  { return My_op(_args...); }
	template<typename... _Args> MATRICE_HOST_FINL constexpr
	auto _Op(_Args&... _args) { return My_op(_args...); }
	internal_t m_data;
};

template<std::size_t _Elems>
MATRICE_HOST_FINL std::size_t vsize(std::size_t _Len) {
	return (_Len - _Len % _Elems);
}

template<typename T, int _Elems> MATRICE_HOST_FINL 
T reduce(const simd_base_<T, _Elems>& _Packed) {
	return dgelom::reduce<T>(_Packed.begin(), _Packed.end());
}

MATRICE_ARCH_END
#endif
