#pragma once

#include <QString>
#include <QMimeData>

#include <QGuiApplication>
#include <QClipboard>

#include "TextEntities.h"
#include "TextWord.h"

struct TextWithTags {
	struct Tag {
		int offset = 0;
		int length = 0;
		QString id;

		friend inline auto operator<=>(const Tag&, const Tag&) = default;
		friend inline bool operator==(const Tag&, const Tag&) = default;
	};
	using Tags = QVector<Tag>;

	QString text;
	Tags tags;

	[[nodiscard]] bool empty() const {
		return text.isEmpty();
	}
	friend inline auto operator<=>(
		const TextWithTags&,
		const TextWithTags&) = default;
	friend inline bool operator==(
		const TextWithTags&,
		const TextWithTags&) = default;
};

namespace text {
	[[nodiscard]] Qt::LayoutDirection Direction(
		const QString& str,
		int from,
		int to);

	[[nodiscard]] bool IsParagraphSeparator(QChar ch);
	[[nodiscard]] bool IsWordSeparator(QChar ch);

	[[nodiscard]] bool IsSpace(QChar ch);
	[[nodiscard]] bool IsNewline(QChar ch);

	[[nodiscard]] bool IsBad(QChar ch);
	[[nodiscard]] bool IsTrimmed(QChar ch);

	[[nodiscard]] bool IsDiacritic(QChar ch);

	EntitiesInText ConvertTextTagsToEntities(const TextWithTags::Tags& tags);
	TextWithTags::Tags ConvertEntitiesToTextTags(
		const EntitiesInText& entities);

	std::unique_ptr<QMimeData> MimeDataFromText(
		TextWithTags&& text,
		const QString& expanded);
	std::unique_ptr<QMimeData> MimeDataFromText(const TextForMimeData& text);
	std::unique_ptr<QMimeData> MimeDataFromText(TextWithTags&& text);

	void SetClipboardText(
		const TextForMimeData& text,
		QClipboard::Mode mode = QClipboard::Clipboard);
} // namespace string
