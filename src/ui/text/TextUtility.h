#pragma once

#include <QString>


namespace text {
	[[nodiscard]] Qt::LayoutDirection Direction(
		const QString& str,
		int from,
		int to);

	[[nodiscard]] bool IsParagraphSeparator(QChar ch);
	[[nodiscard]] bool IsWordSeparator(QChar ch);
	[[nodiscard]] bool IsSpace(QChar ch);
} // namespace string
