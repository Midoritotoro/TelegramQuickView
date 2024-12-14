#pragma once 

#include "../../core/Types.h"


namespace text {
	struct TextParseOptions {
		int32 flags;
		int32 maxw;
		int32 maxh;
		Qt::LayoutDirection dir;
	};
} // namespace text