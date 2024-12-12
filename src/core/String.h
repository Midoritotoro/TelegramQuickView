#pragma once 

#include "Types.h"

struct TextSelection {
	enum class Type {
		Letters = 0x01,
		Words = 0x02,
		Paragraphs = 0x03,
	};

	constexpr TextSelection() = default;
	constexpr TextSelection(uint16 from, uint16 to) : from(from), to(to) {
	}

	[[nodiscard]] constexpr bool empty() const noexcept {
		return from == to;
	}

	[[nodiscard]] bool isFullSelection(const QString& text) const {
		return (from == 0) && (to >= text.size());
	}

	uint16 from = 0;
	uint16 to = 0;
};

inline bool operator==(TextSelection a, TextSelection b) {
	return a.from == b.from && a.to == b.to;
}

inline bool operator!=(TextSelection a, TextSelection b) {
	return !(a == b);
}

static constexpr TextSelection AllTextSelection = { 0, 0xFFFF };

namespace string {
	[[nodiscard]] bool IsParagraphSeparator(QChar ch);
	[[nodiscard]] bool IsWordSeparator(QChar ch);

	[[nodiscard]] bool IsSpace(QChar ch);

	[[nodiscard]] TextSelection adjustSelection(
		const QString& text,
		TextSelection selection,
		TextSelection::Type selectType);
} // namespace string