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

	[[nodiscard]] int SerializeTagsSize(const TextWithTags::Tags& tags);
	[[nodiscard]] QByteArray SerializeTags(const TextWithTags::Tags& tags);
	[[nodiscard]] TextWithTags::Tags DeserializeTags(
		QByteArray data,
		int textLength);
	[[nodiscard]] QString TagsMimeType();
	[[nodiscard]] QString TagsTextMimeType();

	inline const auto kMentionTagStart = qstr("mention://");

	[[nodiscard]] bool IsMentionLink(QStringView link);
	[[nodiscard]] QString MentionEntityData(QStringView link);
	[[nodiscard]] bool IsSeparateTag(QStringView tag);
	[[nodiscard]] QString JoinTag(const QList<QStringView>& list);
	[[nodiscard]] QList<QStringView> SplitTags(QStringView tag);
	[[nodiscard]] QString TagWithRemoved(
		const QString& tag,
		const QString& removed);
	[[nodiscard]] QString TagWithAdded(const QString& tag, const QString& added);
	[[nodiscard]] TextWithTags::Tags SimplifyTags(TextWithTags::Tags tags);

	EntitiesInText ConvertTextTagsToEntities(const TextWithTags::Tags& tags);
	TextWithTags::Tags ConvertEntitiesToTextTags(
		const EntitiesInText& entities);

	std::unique_ptr<QMimeData> MimeDataFromText(
		TextWithTags&& text,
		const QString& expanded)
	std::unique_ptr<QMimeData> MimeDataFromText(const TextForMimeData& text);
	std::unique_ptr<QMimeData> MimeDataFromText(TextWithTags&& text);

	void SetClipboardText(
		const TextForMimeData& text,
		QClipboard::Mode mode = QClipboard::Clipboard);
} // namespace string
