#include "TextUtility.h"

#include "../../core/CoreUtility.h"


Qt::LayoutDirection text::Direction(
	const QString& str,
	int from,
	int to) 
{
	auto pointer = reinterpret_cast<const ushort*>(str.unicode()) + from;
	const auto end = pointer + (to - from);

	while (pointer < end) {
		uint ucs4 = *pointer;

		if (QChar::isHighSurrogate(ucs4) && pointer < end - 1) {
			const auto low = pointer[1];
			if (QChar::isLowSurrogate(low)) {
				ucs4 = QChar::surrogateToUcs4(ucs4, low);
				++pointer;
			}
		}

		switch (QChar::direction(ucs4)) {
			case QChar::DirL:
				return Qt::LeftToRight;
			case QChar::DirR:
			case QChar::DirAL:
				return Qt::RightToLeft;
			default:
				break;
			}
		++pointer;
	}
	return Qt::LayoutDirectionAuto;
}

bool text::IsParagraphSeparator(QChar ch) {
	switch (ch.unicode()) {
		case QChar::LineFeed:
			return true;
		default:
			break;
	}
	return false;
}

bool text::IsWordSeparator(QChar ch) {
	switch (ch.unicode()) {
		case QChar::Space:
		case QChar::LineFeed:
		case '.':
		case ',':
		case '?':
		case '!':
		case '@':
		case '#':
		case '$':
		case ':':
		case ';':
		case '-':
		case '<':
		case '>':
		case '[':
		case ']':
		case '(':
		case ')':
		case '{':
		case '}':
		case '=':
		case '/':
		case '+':
		case '%':
		case '&':
		case '^':
		case '*':
		case '\'':
		case '"':
		case '`':
		case '~':
		case '|':
			return true;
		default:
			break;
	}
	return false;
}

bool text::IsSpace(QChar ch) {
	return ch.isSpace()
		|| (ch.unicode() < 32)
		|| (ch == QChar::ParagraphSeparator)
		|| (ch == QChar::LineSeparator)
		|| (ch == QChar::ObjectReplacementCharacter)
		|| (ch == QChar::CarriageReturn)
		|| (ch == QChar::Tabulation);
}

bool text::IsNewline(QChar ch) {
	return (ch == QChar::LineFeed)
		|| (ch.unicode() == 156);
}

bool text::IsBad(QChar ch) {
	return (ch.unicode() == 0)
		|| (ch.unicode() >= 8232 && ch.unicode() < 8237)
		|| (ch.unicode() >= 65024 && ch.unicode() < 65040 && ch.unicode() != 65039)
		|| (ch.unicode() >= 127 && ch.unicode() < 160 && ch.unicode() != 156);
}

bool text::IsTrimmed(QChar ch) {
	return IsSpace(ch)
		|| IsBad(ch)
		|| (ch == QChar(8203));
}

bool text::IsDiacritic(QChar ch) {
	return (ch.category() == QChar::Mark_NonSpacing)
		|| (ch.unicode() == 1652)
		|| (ch.unicode() >= 64606 && ch.unicode() <= 64611);
}


text::EntitiesInText text::ConvertTextTagsToEntities(const TextWithTags::Tags& tags) {
	auto result = EntitiesInText();
	if (tags.isEmpty()) {
		return result;
	}

	constexpr auto kInMaskTypesInline = std::array{
		EntityType::Code
	};
	constexpr auto kInMaskTypesBlock = std::array{
		EntityType::Pre,
		EntityType::Blockquote,
	};
	struct State {
		QString link;
		QString language;
		uint32 mask : 31 = 0;
		uint32 collapsed : 1 = 0;

		void set(EntityType type) {
			mask |= (1 << int(type));
		}
		void remove(EntityType type) {
			mask &= ~(1 << int(type));
		}
		[[nodiscard]] bool has(EntityType type) const {
			return (mask & (1 << int(type)));
		}
	};

	auto offset = 0;
	auto state = State();
	auto notClosedEntities = std::vector<int>(); // Stack of indices.
	const auto closeOne = [&] {
		Expects(!notClosedEntities.empty());

		auto& entity = result[notClosedEntities.back()];
		const auto type = entity.type();
		entity = {
			type,
			entity.offset(),
			offset - entity.offset(),
			entity.data(),
		};
		if (std::ranges::contains(kInMaskTypesInline, type)
			|| std::ranges::contains(kInMaskTypesBlock, type)) {
			state.remove(entity.type());
		}
		else {
			state.link = QString();
		}
		notClosedEntities.pop_back();
		};
	const auto closeType = [&](EntityType type) {
		auto closeCount = 0;
		const auto notClosedCount = notClosedEntities.size();
		while (closeCount < notClosedCount) {
			const auto index = notClosedCount - closeCount - 1;
			if (result[notClosedEntities[index]].type() == type) {
				for (auto i = 0; i != closeCount + 1; ++i) {
					closeOne();
				}
				break;
			}
			++closeCount;
		}
		};
	const auto openType = [&](EntityType type, const QString& data = {}) {
		notClosedEntities.push_back(result.size());
		result.push_back({ type, offset, -1, data });
		};

	const auto processState = [&](State nextState) {
		const auto linkChanged = (nextState.link != state.link);
		const auto closeLink = linkChanged && !state.link.isEmpty();
		const auto closeCustomEmoji = closeLink
			&& Ui::InputField::IsCustomEmojiLink(state.link);
		if (closeCustomEmoji) {
			closeType(EntityType::CustomEmoji);
		}
		for (const auto type : kInMaskTypesInline) {
			if (state.has(type) && !nextState.has(type)) {
				closeType(type);
			}
		}
		if (closeLink && !closeCustomEmoji) {
			if (IsMentionLink(state.link)) {
				closeType(EntityType::MentionName);
			}
			else {
				closeType(EntityType::CustomUrl);
			}
		}
		for (const auto type : kInMaskTypesBlock) {
			if (state.has(type) && !nextState.has(type)) {
				closeType(type);
			}
		}

		const auto openLink = linkChanged && !nextState.link.isEmpty();
		const auto openCustomEmoji = openLink
			&& Ui::InputField::IsCustomEmojiLink(nextState.link);
		for (const auto type : kInMaskTypesBlock | std::ranges::view::reverse) {
			if (nextState.has(type) && !state.has(type)) {
				openType(type, (type == EntityType::Pre)
					? nextState.language
					: (type == EntityType::Blockquote && nextState.collapsed)
					? u"1"_q
					: QString());
			}
		}
		if (openLink && !openCustomEmoji) {
			if (IsMentionLink(nextState.link)) {
				const auto data = MentionEntityData(nextState.link);
				if (!data.isEmpty()) {
					openType(EntityType::MentionName, data);
				}
			}
			else {
				openType(EntityType::CustomUrl, nextState.link);
			}
		}
		for (const auto type : kInMaskTypesInline | ranges::views::reverse) {
			if (nextState.has(type) && !state.has(type)) {
				openType(type);
			}
		}
		if (openCustomEmoji) {
			const auto data = Ui::InputField::CustomEmojiEntityData(
				nextState.link);
			if (!data.isEmpty()) {
				openType(EntityType::CustomEmoji, data);
			}
		}
		state = nextState;
		};
	const auto stateForTag = [&](const QString& tag) {
		using Tags = Ui::InputField;
		auto result = State();
		const auto list = SplitTags(tag);
		const auto languageStart = Tags::kTagPre.size();
		for (const auto& single : list) {
			if (single == Tags::kTagBold) {
				result.set(EntityType::Bold);
			}
			else if (single == Tags::kTagItalic) {
				result.set(EntityType::Italic);
			}
			else if (single == Tags::kTagUnderline) {
				result.set(EntityType::Underline);
			}
			else if (single == Tags::kTagStrikeOut) {
				result.set(EntityType::StrikeOut);
			}
			else if (single == Tags::kTagCode) {
				result.set(EntityType::Code);
			}
			else if (single == Tags::kTagPre) {
				result.set(EntityType::Pre);
			}
			else if (single.size() > languageStart
				&& single.startsWith(Tags::kTagPre)) {
				result.set(EntityType::Pre);
				result.language = single.mid(languageStart).toString();
			}
			else if (single == Tags::kTagBlockquote) {
				result.set(EntityType::Blockquote);
				result.collapsed = 0;
			}
			else if (single == Tags::kTagBlockquoteCollapsed) {
				result.set(EntityType::Blockquote);
				result.collapsed = 1;
			}
			else if (single == Tags::kTagSpoiler) {
				result.set(EntityType::Spoiler);
			}
			else {
				result.link = single.toString();
			}
		}
		return result;
		};
	for (const auto& tag : tags) {
		if (tag.offset > offset) {
			processState(State());
		}
		offset = tag.offset;
		processState(stateForTag(tag.id));
		offset += tag.length;
	}
	processState(State());

	result.erase(ranges::remove_if(result, [](const EntityInText& entity) {
		return (entity.length() <= 0);
		}), result.end());

	return result;
}

TextWithTags::Tags text::ConvertEntitiesToTextTags(
	const EntitiesInText& entities) {
	auto result = TextWithTags::Tags();
	if (entities.isEmpty()) {
		return result;
	}

	auto offset = 0;
	auto current = QString();
	const auto updateCurrent = [&](int nextOffset, const QString& next) {
		if (next == current) {
			return;
		}
		else if (nextOffset > offset) {
			if (!current.isEmpty()) {
				result.push_back({ offset, nextOffset - offset, current });
			}
			offset = nextOffset;
		}
		current = next;
		};
	auto toRemove = std::vector<std::pair<int, QString>>();
	const auto removeTill = [&](int nextOffset) {
		while (!toRemove.empty() && toRemove.front().first <= nextOffset) {
			updateCurrent(
				toRemove.front().first,
				TagWithRemoved(current, toRemove.front().second));
			toRemove.erase(toRemove.begin());
		}
		};
	for (const auto& entity : entities) {
		const auto push = [&](const QString& tag) {
			removeTill(entity.offset());
			updateCurrent(entity.offset(), TagWithAdded(current, tag));
			toRemove.push_back({ offset + entity.length(), tag });
			ranges::sort(toRemove);
			};
		switch (entity.type()) {
		case EntityType::MentionName: {
			static const auto RegExp = QRegularExpression(
				"^(\\d+\\.\\d+:\\d+)$"
			);
			const auto match = RegExp.match(entity.data());
			if (match.hasMatch()) {
				push(kMentionTagStart + entity.data());
			}
		} break;
		case EntityType::CustomUrl: {
			const auto url = entity.data();
			if (Ui::InputField::IsValidMarkdownLink(url)
				&& !IsMentionLink(url)) {
				push(url);
			}
		} break;
		case EntityType::CustomEmoji: {
			static const auto RegExp = QRegularExpression("^(\\d+)$");
			const auto match = RegExp.match(entity.data());
			if (match.hasMatch()) {
				push(Ui::InputField::CustomEmojiLink(entity.data()));
			}
		} break;
		case EntityType::Bold: push(Ui::InputField::kTagBold); break;
			//case EntityType::Semibold: // Semibold is for UI parts only.
			//	push(Ui::InputField::kTagSemibold);
			//	break;
		case EntityType::Italic: push(Ui::InputField::kTagItalic); break;
		case EntityType::Underline:
			push(Ui::InputField::kTagUnderline);
			break;
		case EntityType::StrikeOut:
			push(Ui::InputField::kTagStrikeOut);
			break;
		case EntityType::Code: push(Ui::InputField::kTagCode); break;
		case EntityType::Pre: {
			if (!entity.data().isEmpty()) {
				static const auto Language = QRegularExpression("^[a-zA-Z0-9\\-\\+]+$");
				if (Language.match(entity.data()).hasMatch()) {
					push(Ui::InputField::kTagPre + entity.data());
					break;
				}
			}
			push(Ui::InputField::kTagPre);
		} break;
		case EntityType::Blockquote:
			push(entity.data().isEmpty()
				? Ui::InputField::kTagBlockquote
				: Ui::InputField::kTagBlockquoteCollapsed);
			break;
		case EntityType::Spoiler: push(Ui::InputField::kTagSpoiler); break;
		}
	}
	if (!toRemove.empty()) {
		removeTill(toRemove.back().first);
	}
	return result;
}

std::unique_ptr<QMimeData> text::MimeDataFromText(
	TextWithTags&& text,
	const QString& expanded) 
{
	if (expanded.isEmpty()) {
		return nullptr;
	}

	auto result = std::make_unique<QMimeData>();
	result->setText(expanded);
	if (!text.tags.isEmpty()) {
		for (auto& tag : text.tags) {
			tag.id = tag.id;
		}
		result->setData(
			text::TagsTextMimeType(),
			text.text.toUtf8());
		result->setData(
			text::TagsMimeType(),
			text::SerializeTags(text.tags));
	}
	return result;
}

std::unique_ptr<QMimeData> text::MimeDataFromText(const TextForMimeData& text) {
	return MimeDataFromText(
		{ text.rich.text, ConvertEntitiesToTextTags(text.rich.entities) },
		text.expanded);
}

std::unique_ptr<QMimeData> text::MimeDataFromText(TextWithTags&& text) {
	const auto expanded = ExpandCustomLinks(text);
	return MimeDataFromText(std::move(text), expanded);
}