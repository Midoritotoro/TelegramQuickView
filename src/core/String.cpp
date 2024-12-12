#include "String.h"

namespace string {
	bool IsParagraphSeparator(QChar ch) {
		switch (ch.unicode()) {
			case QChar::LineFeed:
				return true;
			default:
				break;
			}
		return false;
	}

	bool IsWordSeparator(QChar ch) {
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

	bool IsSpace(QChar ch) {
		return ch.isSpace()
			|| (ch.unicode() < 32)
			|| (ch == QChar::ParagraphSeparator)
			|| (ch == QChar::LineSeparator)
			|| (ch == QChar::ObjectReplacementCharacter)
			|| (ch == QChar::CarriageReturn)
			|| (ch == QChar::Tabulation);
	}

	TextSelection adjustSelection(
		const QString& text,
		TextSelection selection,
		TextSelection::Type selectType)
	{
		auto from = selection.from;
		auto to = selection.to;

		if (from < text.size() && from <= to) {
			if (to > text.size()) 
				to = text.size();
			if (selectType == TextSelection::Type::Paragraphs) {
				for (const auto& block : _blocks) {
					if (block->position() < from) {
						continue;
					}

					const auto& entities = toTextWithEntities().entities;
					const auto eIt = ranges::find_if(entities, [&](
						const EntityInText& e) {
							return (e.type() == EntityType::Pre
								|| e.type() == EntityType::Code)
								&& (from >= e.offset())
								&& ((e.offset() + e.length()) >= to);
						});
					if (eIt != entities.end()) {
						from = eIt->offset();
						to = eIt->offset() + eIt->length();
						while (to > 0 && IsSpace(text.at(to - 1))) {
							--to;
						}
						if (to >= from) {
							return { from, to };
						}
					}
					break;
				}

				if (!IsParagraphSeparator(text.at(from))) {
					while (from > 0 && !IsParagraphSeparator(text.at(from - 1))) {
						--from;
					}
				}
				if (to < text.size()) {
					if (IsParagraphSeparator(text.at(to))) {
						++to;
					}
					else {
						while (to < text.size() && !IsParagraphSeparator(text.at(to))) {
							++to;
						}
					}
				}
			}
			else if (selectType == TextSelection::Type::Words) {
				if (!IsWordSeparator(text.at(from))) {
					while (from > 0 && !IsWordSeparator(text.at(from - 1))) {
						--from;
					}
				}
				if (to < text.size()) {
					if (IsWordSeparator(text.at(to))) {
						++to;
					}
					else {
						while (to < text.size() && !IsWordSeparator(text.at(to))) {
							++to;
						}
					}
				}
			}
		}
		return { from, to };
	}
} // namespace string