#pragma once 

#include <QCursor>
#include "StyleFont.h"


namespace style {
	using rect = QRect;
	using cursor = Qt::CursorShape;
	using margins = QMargins;
	using font = internal::Font;
	using owned_font = internal::OwnedFont;

	static constexpr cursor cursorDefault = Qt::ArrowCursor;
	static constexpr cursor cursorPointer = Qt::PointingHandCursor;
	static constexpr cursor cursorText = Qt::IBeamCursor;
	static constexpr cursor cursorCross = Qt::CrossCursor;
}