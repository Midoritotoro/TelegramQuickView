#pragma once 

#include <QCursor>


namespace style {
	using cursor = Qt::CursorShape;
	using margins = QMargins;

	static constexpr cursor cursorDefault = Qt::ArrowCursor;
	static constexpr cursor cursorPointer = Qt::PointingHandCursor;
	static constexpr cursor cursorText = Qt::IBeamCursor;
	static constexpr cursor cursorCross = Qt::CrossCursor;
}