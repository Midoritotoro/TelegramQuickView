#include "TextBlock.h"

namespace text {
	style::font WithFlags(
		const style::font& font,
		TextBlockFlags flags,
		style::FontFlags fontFlags) {
		using Flag = style::FontFlag;
		if (!flags && !fontFlags) {
			return font;
		}
		else if (IsMono(flags) || (fontFlags & Flag::Monospace)) {
			return font.monospace();
		}
		auto result = font;
		if ((flags & TextBlockFlag::Bold) || (fontFlags & Flag::Bold)) {
			result = result->bold();
		}
		else if ((flags & TextBlockFlag::Semibold)
			|| (fontFlags & Flag::Semibold)) {
			result = result->semibold();
		}
		if ((flags & TextBlockFlag::Italic) || (fontFlags & Flag::Italic)) {
			result = result->italic();
		}
		if ((flags & TextBlockFlag::Underline)
			|| (fontFlags & Flag::Underline)) {
			result = result->underline();
		}
		if ((flags & TextBlockFlag::StrikeOut)
			|| (fontFlags & Flag::StrikeOut)) {
			result = result->strikeout();
		}
		if (flags & TextBlockFlag::Tilde) { // Tilde fix in OpenSans.
			result = result->semibold();
		}
		return result;
	}

	Qt::LayoutDirection UnpackParagraphDirection(bool ltr, bool rtl) {
		return ltr
			? Qt::LeftToRight
			: rtl
			? Qt::RightToLeft
			: Qt::LayoutDirectionAuto;
	}

	AbstractBlock::AbstractBlock(TextBlockType type, BlockDescriptor descriptor)
		: _position(descriptor.position)
		, _type(static_cast<uint16>(type))
		, _flags(descriptor.flags)
		, _linkIndex(descriptor.linkIndex)
		, _colorIndex(descriptor.colorIndex) {
	}

	uint16 AbstractBlock::position() const {
		return _position;
	}

	TextBlockType AbstractBlock::type() const {
		return static_cast<TextBlockType>(_type);
	}

	TextBlockFlags AbstractBlock::flags() const {
		return TextBlockFlags::from_raw(_flags);
	}

	int AbstractBlock::objectWidth() const {
		switch (type()) {
			case TextBlockType::Skip:
				return static_cast<const SkipBlock*>(this)->width();
		}
	}

	uint16 AbstractBlock::linkIndex() const {
		return _linkIndex;
	}

	uint16 AbstractBlock::colorIndex() const {
		return _colorIndex;
	}

	void AbstractBlock::setLinkIndex(uint16 index) {
		_linkIndex = index;
	}

	TextBlock::TextBlock(BlockDescriptor descriptor)
		: AbstractBlock(TextBlockType::Text, descriptor) {
	}

	NewlineBlock::NewlineBlock(BlockDescriptor descriptor, uint16 quoteIndex)
		: AbstractBlock(TextBlockType::Newline, descriptor)
		, _quoteIndex(quoteIndex) {
	}

	SkipBlock::SkipBlock(BlockDescriptor descriptor, int width, int height)
		: AbstractBlock(TextBlockType::Skip, descriptor)
		, _width(width)
		, _height(height) {
	}

	int SkipBlock::width() const {
		return _width;
	}

	int SkipBlock::height() const {
		return _height;
	}

	Block::Block() {
		Unexpected("Should not be called.");
	}

	Block::Block(Block&& other) {
		switch (other->type()) {
		case TextBlockType::Newline:
			emplace<NewlineBlock>(std::move(other.unsafe<NewlineBlock>()));
			break;
		case TextBlockType::Text:
			emplace<TextBlock>(std::move(other.unsafe<TextBlock>()));
			break;
		case TextBlockType::Emoji:
			emplace<EmojiBlock>(std::move(other.unsafe<EmojiBlock>()));
			break;
		case TextBlockType::CustomEmoji:
			emplace<CustomEmojiBlock>(
				std::move(other.unsafe<CustomEmojiBlock>()));
			break;
		case TextBlockType::Skip:
			emplace<SkipBlock>(std::move(other.unsafe<SkipBlock>()));
			break;
		default:
			Unexpected("Bad text block type in Block(Block&&).");
		}
	}

	Block& Block::operator=(Block&& other) {
		if (&other == this) {
			return *this;
		}
		destroy();
		switch (other->type()) {
		case TextBlockType::Newline:
			emplace<NewlineBlock>(std::move(other.unsafe<NewlineBlock>()));
			break;
		case TextBlockType::Text:
			emplace<TextBlock>(std::move(other.unsafe<TextBlock>()));
			break;
		case TextBlockType::Skip:
			emplace<SkipBlock>(std::move(other.unsafe<SkipBlock>()));
			break;
		default:
			break;
		}
		return *this;
	}

	Block::~Block() {
		destroy();
	}

	Block Block::Newline(BlockDescriptor descriptor, uint16 quoteIndex) {
		return New<NewlineBlock>(descriptor, quoteIndex);
	}

	Block Block::Text(BlockDescriptor descriptor) {
		return New<TextBlock>(descriptor);
	}

	Block Block::Skip(BlockDescriptor descriptor, int width, int height) {
		return New<SkipBlock>(descriptor, width, height);
	}

	AbstractBlock* Block::get() {
		return &unsafe<AbstractBlock>();
	}

	const AbstractBlock* Block::get() const {
		return &unsafe<AbstractBlock>();
	}

	AbstractBlock* Block::operator->() {
		return get();
	}

	const AbstractBlock* Block::operator->() const {
		return get();
	}

	AbstractBlock& Block::operator*() {
		return *get();
	}

	const AbstractBlock& Block::operator*() const {
		return *get();
	}

	void Block::destroy() {
		switch (get()->type()) {
		case TextBlockType::Newline:
			unsafe<NewlineBlock>().~NewlineBlock();
			break;
		case TextBlockType::Text:
			unsafe<TextBlock>().~TextBlock();
			break;
		case TextBlockType::Skip:
			unsafe<SkipBlock>().~SkipBlock();
			break;
		default:
			break;
		}
	}

	int CountBlockHeight(const AbstractBlock* block) {
		return (block->type() == TextBlockType::Skip)
			? static_cast<const SkipBlock*>(block)->height()
			: st->lineHeight
			? st->lineHeight
			: st->font->height;
	}
} // namespace text