#pragma once

#include <QVector>

namespace text {
	class Word;
	class Block

	using Words = QVector<Word>;
	using Blocks = QVector<Block>;

	struct TextWithTags {
		struct Tag;
		using Tags = QVector<Tag>;
	};

	enum class EntityType : uchar {
		Invalid = 0,

		Url,
		CustomUrl,
		Email,
		Hashtag,
		Cashtag,
		Mention,
		MentionName,
		CustomEmoji,
		BotCommand,
		MediaTimestamp,
		Colorized,
		Phone,

		Bold,
		Semibold,
		Italic,
		Underline,
		StrikeOut,
		Code, // inline
		Pre,  // block
		Blockquote,
		Spoiler
	};

	enum class EntityLinkShown : uchar {
		Full,
		Partial,
	};

	struct TextSelection {
		enum class Type;
	};

	struct TextState;

	class EntityInText;
	using EntitiesInText = QVector<EntityInText>;

	struct EntityLinkData;
}