#include "String.h"

namespace text {

	String::String(const QString& text) {
		setText(text);
	}

	int String::countWidth(int width, bool breakEverywhere) const {
		if (width >= _maxWidth) {
			return _maxWidth;
		}

		auto maxLineWidth = 0;
		enumerateLines(width, breakEverywhere, [&](int lineWidth, int) {
			if (lineWidth > maxLineWidth) {
				maxLineWidth = lineWidth;
			}
			});
		return maxLineWidth;
	}

	int String::countHeight(int width, bool breakEverywhere) const {
		if (width >= _maxWidth) {
			return _minHeight;
		}
		int result = 0;
		enumerateLines(width, breakEverywhere, [&](auto, int lineBottom) {
			result = lineBottom;
			});
		return result;
	}

	String::DimensionsResult String::countDimensions(
		GeometryDescriptor geometry) const {
		return countDimensions(std::move(geometry), {});
	}

	String::DimensionsResult String::countDimensions(
		GeometryDescriptor geometry,
		DimensionsRequest request) const {
		auto result = DimensionsResult();
		if (request.lineWidths && request.reserve)
			result.lineWidths.reserve(request.reserve);

		enumerateLines(geometry, [&](int lineWidth, int lineBottom) {
			const auto width = lineWidth;
			if (request.lineWidths) {
				result.lineWidths.push_back(width);
			}
			result.width = std::max(result.width, width);
			result.height = lineBottom;
			});

		return result;
	}

	void String::recountNaturalSize(
		bool initial,
		Qt::LayoutDirection optionsDirection) 
	{
		auto lastNewlineBlock = begin(_blocks);
		auto lastNewlineStart = 0;

		const auto computeParagraphDirection = [&](int paragraphEnd) {
			const auto direction = (optionsDirection != Qt::LayoutDirectionAuto)
				? optionsDirection
				: text::Direction(_text, lastNewlineStart, paragraphEnd);

			if (paragraphEnd) {
				while (blockPosition(lastNewlineBlock) < lastNewlineStart) {
					++lastNewlineBlock;
				}
				assert(lastNewlineBlock != end(_blocks));
				const auto block = lastNewlineBlock->get();
				if (block->type() == TextBlockType::Newline) {
					assert(block->position() == lastNewlineStart);
					static_cast<NewlineBlock*>(block)->setParagraphDirection(
						direction);
				}
				else {
					assert(!lastNewlineStart);
					_startParagraphLTR = (direction == Qt::LeftToRight);
					_startParagraphRTL = (direction == Qt::RightToLeft);
				}
			}
		};

		auto qindex = quoteIndex(nullptr);
		auto quote = quoteByIndex(qindex);

		auto qpadding = quotePadding(quote);
		auto qminwidth = quoteMinWidth(quote);

		auto qlinesleft = quoteLinesLimit(quote);
		auto qmaxwidth = QFixed(qminwidth);
		auto qoldheight = 0;

		_maxWidth = 0;
		_minHeight = qpadding.top();

		const auto lineHeight = this->lineHeight();
		auto maxWidth = QFixed();

		auto width = QFixed(qminwidth);
		auto last_rBearing = QFixed();
		auto last_rPadding = QFixed();

		for (const auto& word : _words) {
			if (word.newline()) {
				const auto block = word.newlineBlockIndex();
				const auto index = quoteIndex(_blocks[block].get());

				const auto changed = (qindex != index);
				const auto hidden = !qlinesleft;

				core::utility::accumulateMax(maxWidth, width);
				core::utility::accumulateMax(qmaxwidth, width);

				if (changed) {
					_minHeight += qpadding.bottom();

					if (quote) {
						quote->maxWidth = qmaxwidth.ceil().toInt();
						quote->minHeight = _minHeight - qoldheight;
					}

					qoldheight = _minHeight;
					qindex = index;

					quote = quoteByIndex(qindex);
					qpadding = quotePadding(quote);

					qminwidth = quoteMinWidth(quote);
					qlinesleft = quoteLinesLimit(quote);

					qmaxwidth = qminwidth;
					_minHeight += qpadding.top();
					qpadding.setTop(0);
				}
				else if (qlinesleft > 0) {
					--qlinesleft;
				}
				if (initial) {
					computeParagraphDirection(word.position());
				}
				lastNewlineStart = word.position();

				if (!hidden) {
					_minHeight += lineHeight;
				}

				last_rBearing = 0;
				last_rPadding = word.f_rpadding();

				width = qminwidth;
				continue;
			}

			auto w__f_rbearing = word.f_rbearing();

			core::utility::accumulateMax(maxWidth, width);
			core::utility::accumulateMax(qmaxwidth, width);

			width += last_rBearing + (last_rPadding + word.f_width() - w__f_rbearing);

			last_rBearing = w__f_rbearing;
			last_rPadding = word.f_rpadding();
		}
		if (initial) {
			computeParagraphDirection(_text.size());
		}
		if (width > 0) {
			const auto useSkipHeight = (_blocks.back()->type() == TextBlockType::Skip)
				&& (_words.back().f_width() == width);
			_minHeight += qpadding.top() + qpadding.bottom();
			if (qlinesleft != 0) {
				_minHeight += useSkipHeight
					? _blocks.back().unsafe<SkipBlock>().height()
					: lineHeight;
			}
			core::utility::accumulateMax(maxWidth, width);
			core::utility::accumulateMax(qmaxwidth, width);
		}
		_maxWidth = maxWidth.ceil().toInt();
		if (quote) {
			quote->maxWidth = qmaxwidth.ceil().toInt();
			quote->minHeight = _minHeight - qoldheight;
			_endsWithQuoteOrOtherDirection = true;
		}
		else {
			const auto lastIsNewline = (lastNewlineBlock != end(_blocks))
				&& (lastNewlineBlock->get()->type() == TextBlockType::Newline);
			const auto lastNewline = lastIsNewline
				? static_cast<NewlineBlock*>(lastNewlineBlock->get())
				: nullptr;
			const auto lastLineDirection = lastNewline
				? lastNewline->paragraphDirection()
				: _startParagraphRTL
				? Qt::RightToLeft
				: Qt::LeftToRight;
			_endsWithQuoteOrOtherDirection
				= (lastLineDirection != Qt::LeftToRight());
		}
	}


	TextSelection String::adjustSelection(
		TextSelection selection,
		TextSelection::Type selectType)
	{
		auto from = selection.from;
		auto to = selection.to;

		if (from < _text.size() && from <= to) {
			if (to > _text.size())
				to = _text.size();
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
						while (to > 0 && text::IsSpace(_text.at(to - 1))) {
							--to;
						}
						if (to >= from) {
							return { from, to };
						}
					}
					break;
				}

				if (!text::IsParagraphSeparator(_text.at(from))) {
					while (from > 0 && !text::IsParagraphSeparator(_text.at(from - 1))) {
						--from;
					}
				}
				if (to < _text.size()) {
					if (text::IsParagraphSeparator(_text.at(to))) {
						++to;
					}
					else {
						while (to < _text.size() && !text::IsParagraphSeparator(_text.at(to))) {
							++to;
						}
					}
				}
			}
			else if (selectType == TextSelection::Type::Words) {
				if (!text::IsWordSeparator(_text.at(from))) {
					while (from > 0 && !text::IsWordSeparator(_text.at(from - 1))) {
						--from;
					}
				}
				if (to < _text.size()) {
					if (text::IsWordSeparator(_text.at(to))) {
						++to;
					}
					else {
						while (to < _text.size() && !text::IsWordSeparator(_text.at(to))) {
							++to;
						}
					}
				}
			}
		}
		return { from, to };
	}
} // namespace text