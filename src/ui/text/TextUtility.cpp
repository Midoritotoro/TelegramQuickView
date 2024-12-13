#include "TextUtility.h"


namespace text {
	Qt::LayoutDirection Direction(
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
} // namespace string
