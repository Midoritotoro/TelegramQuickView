#pragma once

#include "../ClickHandler.h"
#include "../../core/Flags.h"

#include "../../core/Types.h"
#include "../style/StyleTypes.h"

#include <QString>


namespace text {
	enum class EntityType: uchar {
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

	enum class EntityLinkShown: uchar {
		Full,
		Partial,
	};

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

	struct StateRequest {
		enum class StateFlag {
			BreakEverywhere = (1 << 0),
			LookupSymbol = (1 << 1),
			LookupLink = (1 << 2),
			LookupCustomTooltip = (1 << 3),
		};
		DECLARE_FLAGS(StateFlags, StateFlag);
		friend inline constexpr auto is_flag_type(StateFlag) {
			return true;
		};

		StateRequest() {
		}

		style::align align = style::alignLeft;
		StateFlags flags = StateFlag::LookupLink;

	};

	struct TextState {
		ClickHandlerPtr link;
		bool uponSymbol = false;
		bool afterSymbol = false;
		uint16 symbol = 0;
	};

	struct StateRequestElided : StateRequest {
		StateRequestElided() {
		}
		StateRequestElided(const StateRequest& other) : StateRequest(other) {
		}
		int lines = 1;
		int removeFromEnd = 0;
	};

	static constexpr TextSelection AllTextSelection = { 0, 0xFFFF };

	class EntityInText;
	using EntitiesInText = QVector<EntityInText>;

	struct TextWithEntities {
		QString text;
		EntitiesInText entities;

		[[nodiscard]] bool empty() const;
		void reserve(int size, int entitiesCount = 0);

		TextWithEntities& append(TextWithEntities&& other);
		TextWithEntities& append(const TextWithEntities& other);

		TextWithEntities& append(const QString& other);
		TextWithEntities& append(QLatin1String other);
		TextWithEntities& append(QChar other);

		[[nodiscard]] static TextWithEntities Simple(const QString& simple);

		friend inline auto operator<=>(
			const TextWithEntities&,
			const TextWithEntities&) = default;
		friend inline bool operator==(
			const TextWithEntities&,
			const TextWithEntities&) = default;
	};

	struct TextForMimeData {
		QString expanded;
		TextWithEntities rich;

		[[nodiscard]] bool empty() const;
		void reserve(int size, int entitiesCount = 0);

		[[nodiscard]] TextForMimeData& append(TextForMimeData&& other);
		[[nodiscard]] TextForMimeData& append(TextWithEntities&& other);
		[[nodiscard]] TextForMimeData& append(const QString& other);

		[[nodiscard]] TextForMimeData& append(QLatin1String other);
		[[nodiscard]] TextForMimeData& append(QChar other);

		[[nodiscard]] static TextForMimeData Rich(TextWithEntities&& rich);
		[[nodiscard]] static TextForMimeData Simple(const QString& simple);
	};


	class EntityInText {
	public:
		EntityInText(
			EntityType type,
			int offset,
			int length,
			const QString& data = QString());

		static [[nodiscard]] int FirstMonospaceOffset(
			const EntitiesInText& entities,
			int textLength);
		[[nodiscard]] EntityType type() const;
		[[nodiscard]] int offset() const;

		[[nodiscard]] int length() const;
		[[nodiscard]] QString data() const;

		void extendToLeft(int extent);
		void shrinkFromRight(int shrink);

		void shiftLeft(int shift);
		void shiftRight(int shift);

		void updateTextEnd(int textEnd);
		explicit operator bool() const {
			return type() != EntityType::Invalid;
		}
	private:
		EntityType _type = EntityType::Invalid;

		int _offset = 0;
		int _length = 0;

		QString _data;
	};

	struct EntityLinkData {
		QString text;
		QString data;
		EntityType type = EntityType::Invalid;
		EntityLinkShown shown = EntityLinkShown::Full;

		friend inline auto operator<=>(
			const EntityLinkData&,
			const EntityLinkData&) = default;
		friend inline bool operator==(
			const EntityLinkData&,
			const EntityLinkData&) = default;
	};
} // namespace text