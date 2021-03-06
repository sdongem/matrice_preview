#pragma once

#include "../private/tensor/_tensor.hpp"

DGE_MATRICE_BEGIN
template<typename _Ty, std::size_t... _Shape>
using tensor = detail::_Tensor_impl<_Ty, _Shape...>;
template<typename _Ty, int... _Shape>
using multi_matrix = detail::_Multi_matrix<_Ty, _Shape...>;
DGE_MATRICE_END
