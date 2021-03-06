/**************************************************************************
This file is part of Matrice, an effcient and elegant C++ library.
Copyright(C) 2018, Zhilong(Dgelom) Su, all rights reserved.

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
#pragma once

#include "..\include\Matrice\util\_macros.h"

#ifdef __use_ocv_as_view__
#include <opencv2\core.hpp>
#define VIEW_BASE_OCV cv::Mat
#ifdef __CXX11__
using ocv_view_t = VIEW_BASE_OCV;
template<typename Type>
using ocv_view_t_cast = cv::DataType<Type>;
#else
#define __view_space__ cv::
typedef cv::Mat ocv_view_t;
#endif
#endif // __use_ocv_as_view__

