//------------------------------------------------------------------------------
/**
 * @file    DisallowCopy.hpp
 * @brief   コピー禁止Mixin
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

/// コピー禁止mix-inマクロ
#define TOFU_DISALLOW_COPY(ClassName) \
		ClassName(const ClassName&) = delete;\
		ClassName& operator=(const ClassName&) = delete;
