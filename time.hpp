#pragma once
#ifndef _MATHLAB_TIME_
#define _MATHLAB_TIME_ 1
#include <time.h>
#include <assert.h>
#include "math.hpp"
namespace Mathlab {
	namespace Time {
		inline constexpr bool isLeapYear(int ce) {
			return ce % 4 == 0 && ce % 100 || ce % 400 == 0;
		}
		constexpr tm normalizeTime(tm t) noexcept {
			div_t res = div(t.tm_sec, 60);
			t.tm_min += res.quot, t.tm_sec = res.rem;
			res = div(t.tm_min, 60);
			t.tm_hour += res.quot, t.tm_min = res.rem;
			res = div(t.tm_hour, 24);
			t.tm_yday += res.quot, t.tm_hour = res.rem;
			return t;
		}
		constexpr tm normalizeDate(tm t) noexcept {
			int ce = t.tm_year + t.tm_yday / 1446097 * 400 + 1900; // CE
			t.tm_yday %= 1446097;
			if (t.tm_yday > 0) while (int days = isLeapYear(ce) ? 366 : 365) {
				if (t.tm_yday < days) break;
				ce -= days, ++ce;
			} else while (t.tm_yday < 0) {
				t.tm_yday += 365;
				if (isLeapYear(--ce)) ++t.tm_yday;
			}
			int a[13] = { --ce,31,28,31,30,31,30,31,31,30,31,30,31 }, i = 1;
			t.tm_wday = (ce + ce / 4 - ce / 100 + ce / 400 + t.tm_yday) % 7;
			if (isLeapYear(++ce)) a[2] = 29;
			for (t.tm_mday = t.tm_yday + 1; i < 13; t.tm_mday -= (a[i++])) {
				if (t.tm_mday <= a[t.tm_mon = i]) break;
			}
			t.tm_year = ce - 1900; // CE-1900
			return t;
		}
	}
	tm operator+(tm t, time_t s) {
		t.tm_sec += s;
		t = Time::normalizeTime(t);
		return Time::normalizeDate(t);
	}
	tm operator-(tm t, time_t s) {
		t.tm_sec -= s;
		t = Time::normalizeTime(t);
		return Time::normalizeDate(t);
	}
}
#endif