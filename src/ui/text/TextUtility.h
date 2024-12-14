#pragma once

#include <QString>
#include <QMimeData>

#include <QGuiApplication>
#include <QClipboard>

#include "TextEntities.h"
#include "TextWord.h"


namespace text {
	const auto kTagBold = u"**"_q;
	const auto kTagItalic = u"__"_q;
	const auto kTagUnderline = u"^^"_q;
	const auto kTagStrikeOut = u"~~"_q;
	const auto kTagCode = u"`"_q;
	const auto kTagPre = u"```"_q;
	const auto kTagBlockquote = u">"_q;
	const auto kTagBlockquoteCollapsed = u">^"_q;
	constexpr auto kTagSeparator = '\\';

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
	[[nodiscard]] bool IsMentionLink(QStringView link);

	[[nodiscard]] bool IsSeparateTag(QStringView tag);
	[[nodiscard]] bool IsValidMarkdownLink(QStringView link);

	namespace details {

		struct ContainerImplHelper {
			enum CutResult { Null, Empty, Full, Subset };
			static constexpr CutResult mid(
				qsizetype originalLength,
				qsizetype* _position,
				qsizetype* _length) {
				qsizetype& position = *_position;
				qsizetype& length = *_length;
				if (position > originalLength) {
					position = 0;
					length = 0;
					return Null;
				}

				if (position < 0) {
					if (length < 0 || length + position >= originalLength) {
						position = 0;
						length = originalLength;
						return Full;
					}
					if (length + position <= 0) {
						position = length = 0;
						return Null;
					}
					length += position;
					position = 0;
				}
				else if (size_t(length) > size_t(originalLength - position)) {
					length = originalLength - position;
				}

				if (position == 0 && length == originalLength)
					return Full;

				return length > 0 ? Subset : Empty;
			}
		};

	} // namespace details

	[[nodiscard]] QString TagsMimeType();
	[[nodiscard]] QString TagsTextMimeType();

	[[nodiscard]] QString MentionEntityData(QStringView link);
	[[nodiscard]] QList<QStringView> SplitTags(QStringView tag);

	[[nodiscard]] QString JoinTag(const QList<QStringView>& list);
	[[nodiscard]] QString TagWithRemoved(const QString& tag, const QString& removed);
	[[nodiscard]] QString TagWithAdded(const QString& tag, const QString& added);

	[[nodiscard]] EntitiesInText ConvertTextTagsToEntities(const TextWithTags::Tags& tags);

	[[nodiscard]] QString ExpandCustomLinks(const TextWithTags& text);
	[[nodiscard]] QStringView StringViewMid(
		QStringView view,
		qsizetype pos,
		qsizetype n = -1);

	[[nodiscard]] QByteArray SerializeTags(const TextWithTags::Tags& tags);

	inline const auto kMentionTagStart = QLatin1String("mention://");

	[[nodiscard]] TextWithTags::Tags ConvertEntitiesToTextTags(
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
