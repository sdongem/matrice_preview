#include "../../include/Matrice/algs/interpolation/_bicubic_spline_interp.h"

MATRICE_ALGS_BEGIN
template<typename _Ty, size_t _Options>
void BicubicSplineInterp<_Ty, _Options>::_Bspline_coeff(const matrix_t& _Data) {
	const auto _Width = _Data.cols(), _Height = _Data.rows();
	m_coeff.create(_Height, _Width, zero<value_t>::value);

	//initialization
	const value_t _Z = -2. + sqrt(value_t(3));
	const value_t _A = _Z / (_Z*_Z - 1);
	const index_t _K0 = static_cast<index_t>(log(base_t::m_eps) / log(abs(_Z))) + 1;

	matrix_t _Buff(_Height, _Width);

	//Recursion over each column

	//pre-calculate the initial value for the first recursion
	matrix_t _R0(1, _Width, zero<value_t>::value);
#pragma omp parallel if(_K0 > 100)
{
#pragma omp for
	for (index_t _Row = 0; _Row < _K0; ++_Row) {
		_R0 = _R0 + _Data.rview(_Row) * pow(_Z, _Row);
	}
}

	_Buff.rview(0) = _R0;
	for (size_t _Row = 1; _Row < _Height; ++_Row) {
		_Buff.rview(_Row) = _Data.rview(_Row) + _Buff.rview(_Row - 1)*_Z;
	}

	m_coeff.rview(_Height - 1) = _A * (_Buff.rview(_Height - 1) + _Buff.rview(_Height - 2)*_Z);
	for (index_t _Row = _Height - 2; _Row >= 0; --_Row) {
		m_coeff.rview(_Row) = _Z * (m_coeff.rview(_Row + 1) - _Buff.rview(_Row));
	}

	//Recursion over each row

	//pre-calculate the initial value for the first recursion
	matrix_t _C0(_Height, 1, zero<value_t>::value);
#pragma omp parallel if(_K0 > 100)
{
#pragma omp for
	for (index_t _Col = 0; _Col < _K0; ++_Col) {
		_C0 = _C0 + m_coeff.cview(_Col) * pow(_Z, _Col);
	}
}

	_Buff.cview(0) = _C0;
	for (size_t _Col = 1; _Col < _Width; ++_Col) {
		_Buff.cview(_Col) = m_coeff.cview(_Col) + _Buff.cview(_Col - 1)*_Z;
	}

	m_coeff.cview(_Width - 1) = _A * (_Buff.cview(_Width - 1) + _Buff.cview(_Width - 2)*_Z);
	for (index_t _Col = _Width - 2; _Col >= 0; --_Col) {
		m_coeff.cview(_Col) = (m_coeff.cview(_Col + 1) - _Buff.cview(_Col))*_Z;
	}
}

template class BicubicSplineInterp<float, INTERP | BSPLINE | BICUBIC>;
template class BicubicSplineInterp<double, INTERP | BSPLINE | BICUBIC>;
MATRICE_ALGS_END