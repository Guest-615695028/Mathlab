#pragma once
#ifndef _MATHLAB_STRING_
#define _MATHLAB_STRING_ 1
#include "linearlist.hpp"
namespace Mathlab {
	using String = LinearList<char>;
	String toString(intmax_t i) noexcept {
		String result(1, 0);
		if (i < 0) i = -i;
		return result;
	}
}
#endif