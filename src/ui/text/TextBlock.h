#pragma once

#include "../style/StyleTypes.h"
#include "../style/StyleFont.h"

#include "../../core/Time.h"

#include "../../core/Types.h"
#include <private/qfixed_p.h>



namespace text {
	enum class TextBlockType : uint16 {
		Newline = 0x01,
		Text = 0x02,
		Emoji = 0x03,
		CustomEmoji = 0x04,
		Skip = 0x05,
	};

	enum class TextBlockFlag : uint16 {
		Bold = 0x001,
		Italic = 0x002,
		Underline = 0x004,
		StrikeOut = 0x008,
		Tilde = 0x010,
		Semibold = 0x020,
		Code = 0x040,
		Pre = 0x080,
		Spoiler = 0x100,
		Blockquote = 0x200,
	};

	Q_DECLARE_FLAGS(TextBlockFlags, TextBlockFlag)

	class Block;
	using Blocks = std::vector<Block>;

	inline constexpr bool is_flag_type(TextBlockFlag) { 
		return true; 
	}

	[[nodiscard]] style::font WithFlags(
		const style::font& font,
		TextBlockFlags flags,
		style::FontFlags fontFlags) ;

	[[nodiscard]] Qt::LayoutDirection UnpackParagraphDirection(
		bool ltr,
		bool rtl);

	struct BlockDescriptor {
		TextBlockFlags flags;
		uint16 position = 0;
		uint16 linkIndex = 0;
		uint16 colorIndex = 0;
	};

	class AbstractBlock {
	public:
		[[nodiscard]] uint16 position() const;
		[[nodiscard]] TextBlockType type() const;
		[[nodiscard]] TextBlockFlags flags() const;

		[[nodiscard]] int objectWidth() const;
		[[nodiscard]] uint16 colorIndex() const;
		[[nodiscard]] uint16 linkIndex() const;

		void setLinkIndex(uint16 index);

	protected:
		AbstractBlock(TextBlockType type, BlockDescriptor descriptor);

		uint16 _position = 0;
		uint16 _type : 4 = 0;
		uint16 _flags : 12 = 0;
		uint16 _linkIndex = 0;
		uint16 _colorIndex = 0;

	};

	class NewlineBlock final : public AbstractBlock {
	public:
		NewlineBlock(BlockDescriptor descriptor, uint16 quoteIndex);

		void setQuoteIndex(uint16 index) {
			_quoteIndex = index;
		}
		[[nodiscard]] uint16 quoteIndex() const {
			return _quoteIndex;
		}
		void setParagraphDirection(Qt::LayoutDirection direction) {
			_paragraphLTR = (direction == Qt::LeftToRight);
			_paragraphRTL = (direction == Qt::RightToLeft);
		}
		[[nodiscard]] Qt::LayoutDirection paragraphDirection() const {
			return UnpackParagraphDirection(_paragraphLTR, _paragraphRTL);
		}

	private:
		uint16 _quoteIndex = 0;
		bool _paragraphLTR : 1 = false;
		bool _paragraphRTL : 1 = false;

	};

	class TextBlock final : public AbstractBlock {
	public:
		explicit TextBlock(BlockDescriptor descriptor);

	};

	class SkipBlock final : public AbstractBlock {
	public:
		SkipBlock(BlockDescriptor descriptor, int width, int height);

		[[nodiscard]] int width() const;
		[[nodiscard]] int height() const;

	private:
		int _width = 0;
		int _height = 0;

	};

	class Block final {
	public:
		Block();
		Block(Block&& other);
		Block& operator=(Block&& other);
		~Block();

		[[nodiscard]] static Block Newline(
			BlockDescriptor descriptor,
			uint16 quoteIndex);
		[[nodiscard]] static Block Text(BlockDescriptor descriptor);
		[[nodiscard]] static Block Skip(
			BlockDescriptor descriptor,
			int width,
			int height);

		template <typename FinalBlock>
		[[nodiscard]] FinalBlock& unsafe() {
			return *reinterpret_cast<FinalBlock*>(&_data);
		}

		template <typename FinalBlock>
		[[nodiscard]] const FinalBlock& unsafe() const {
			return *reinterpret_cast<const FinalBlock*>(&_data);
		}

		[[nodiscard]] AbstractBlock* get();
		[[nodiscard]] const AbstractBlock* get() const;

		[[nodiscard]] AbstractBlock* operator->();
		[[nodiscard]] const AbstractBlock* operator->() const;

		[[nodiscard]] AbstractBlock& operator*();
		[[nodiscard]] const AbstractBlock& operator*() const;

	private:
		struct Tag {
		};

		explicit Block(const Tag&) {
		}

		template <typename FinalType, typename ...Args>
		[[nodiscard]] static Block New(Args &&...args) {
			auto result = Block(Tag{});
			result.emplace<FinalType>(std::forward<Args>(args)...);
			return result;
		}

		template <typename FinalType, typename ...Args>
		void emplace(Args &&...args) {
			new (&_data) FinalType(std::forward<Args>(args)...);
		}

		void destroy();

		static_assert(sizeof(NewlineBlock) <= sizeof(SkipBlock));
		static_assert(alignof(NewlineBlock) <= alignof(void*));
		static_assert(sizeof(TextBlock) <= sizeof(SkipBlock));
		static_assert(alignof(TextBlock) <= alignof(void*));

		std::aligned_storage_t<sizeof(SkipBlock), alignof(void*)> _data;
	};

	[[nodiscard]] inline uint16 CountPosition(Blocks::const_iterator i) {
		return (*i)->position();
	}

	[[nodiscard]] int CountBlockHeight(
		const AbstractBlock* block,
		const style::font& font);

	[[nodiscard]] inline bool IsMono(TextBlockFlags flags) {
		return (flags & TextBlockFlag::Pre) || (flags & TextBlockFlag::Code);
	}

} // namespace Ui::Text
