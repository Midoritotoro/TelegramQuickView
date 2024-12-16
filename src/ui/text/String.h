#pragma once 

#include "../BasicClickHandlers.h"

#include "TextEntities.h"
#include "../../core/CoreUtility.h"

#include "TextUtility.h"
#include "TextBlock.h"

#include "../style/StyleFont.h"

#include <QString>
#include <QPainter>

#include <xutility>
#include <private/qfixed_p.h>

#include "Types.h"
#include <span>


class PreClickHandler;
class BlockquoteClickHandler;

namespace text {
	inline constexpr auto kQFixedMax = (INT_MAX / 256);
	inline constexpr auto kMaxQuoteOutlines = 3;

	struct QuotePaintCache {
		QImage corners;
		QImage outline;
		QImage expand;
		QImage collapse;
		mutable QImage bottomCorner;
		mutable QImage bottomRounding;
		mutable QImage collapsedLine;

		std::array<QColor, kMaxQuoteOutlines> outlinesCached;
		QColor headerCached;
		QColor bgCached;
		QColor iconCached;

		std::array<QColor, kMaxQuoteOutlines> outlines;
		QColor header;
		QColor bg;
		QColor icon;
	};

	struct SpecialColor {
		const QPen* pen = nullptr;
		const QPen* penSelected = nullptr;
	};

	struct HighlightInfoRequest {
		TextSelection range;
		QRect interpolateTo;
		double interpolateProgress = 0.;
		QPainterPath* outPath = nullptr;
	};

	struct LineGeometry {
		int left = 0;
		int width = 0;
		bool elided = false;
	};

	struct GeometryDescriptor {
		Fn<LineGeometry(int line)> layout;
		bool breakEverywhere = false;
		bool* outElided = nullptr;
	};

	struct PaintContext {
		QPoint position;
		int outerWidth = 0;
		int availableWidth = 0;
		GeometryDescriptor geometry;
		Qt::Alignment align = Qt::AlignLeft;
		QRect clip;

		const style::TextPalette* palette = new style::TextPalette();

		QuotePaintCache* pre = nullptr;
		QuotePaintCache* blockquote = nullptr;
		std::span<SpecialColor> colors = {};
		Time::time now = 0;

		bool paused = false;
		bool pausedEmoji = false;
		bool pausedSpoiler = false;

		bool fullWidthSelection = true;
		TextSelection selection;

		HighlightInfoRequest* highlight = nullptr;

		int elisionHeight = 0;
		int elisionLines = 0;
		int elisionRemoveFromEnd = 0;
		bool elisionBreakEverywhere = false;

		bool elisionMiddle = false;
		bool useFullWidth = false; // !(width = min(availableWidth, maxWidth()))
};

	class WordParser;
	class TextWord;
	class BlockParser;
	class BidiAlgorithm;

	struct QuoteDetails {
		QString language;

		std::shared_ptr<PreClickHandler> copy;
		std::shared_ptr<BlockquoteClickHandler> toggle;

		int copyWidth = 0;
		int maxWidth = 0;

		int minHeight = 0;
		int scrollLeft = 0;

		bool blockquote = false;
		bool collapsed = false;
		bool expanded = false;
		bool pre = false;
	};

	struct QuotesData {
		std::vector<QuoteDetails> list;
		Fn<void(int index, bool expanded)> expandCallback;
	};

	struct Modification {
		int position = 0;
		uint16 skipped = 0;
		bool added = false;
	};

	struct ExtendedData {
		std::vector<ClickHandlerPtr> links;
		std::unique_ptr<QuotesData> quotes;
		std::vector<Modification> modifications;
	};

	enum {
		TextParseMultiline = 0x001,
		TextParseLinks = 0x002,
		TextParseMentions = 0x004,
		TextParseHashtags = 0x008,
		TextParseBotCommands = 0x010,
		TextParseMarkdown = 0x020,
		TextParseColorized = 0x040,
	};

	const TextParseOptions kDefaultTextOptions = {
		TextParseLinks | TextParseMultiline, // flags
		0, // maxw
		0, // maxh
		Qt::LayoutDirectionAuto, // dir
	};

	const TextParseOptions kMarkupTextOptions = {
		TextParseLinks | TextParseMultiline | TextParseMarkdown, // flags
		0, // maxw
		0, // maxh
		Qt::LayoutDirectionAuto, // dir
	};

	const TextParseOptions kPlainTextOptions = {
		TextParseMultiline, // flags
		0, // maxw
		0, // maxh
		Qt::LayoutDirectionAuto, // dir
	};


	class String {
	public:
		struct LineWidthsOptions {
			bool breakEverywhere = false;
			int reserve = 0;
		};

		struct DimensionsResult {
			int width = 0;
			int height = 0;
			std::vector<int> lineWidths;
		};

		struct DimensionsRequest {
			bool breakEverywhere = false;
			bool lineWidths = false;
			int reserve = 0;
		};
		String(int32 minResizeWidth = kQFixedMax);
		String(String&& other) = default;
		String() = default;
		String(
			const style::TextStyle& font,
			const QString& string);

		String& operator=(String&& other) = default;

		~String() = default;

		[[nodiscard]] int countWidth(
			int width,
			bool breakEverywhere = false) const;
		[[nodiscard]] int countHeight(
			int width,
			bool breakEverywhere = false) const;

		[[nodiscard]] DimensionsResult countDimensions(
			GeometryDescriptor geometry) const;
		[[nodiscard]] DimensionsResult countDimensions(
			GeometryDescriptor geometry,
			DimensionsRequest request) const;

		[[nodiscard]] std::vector<int> countLineWidths(int width) const;
		[[nodiscard]] std::vector<int> countLineWidths(
			int width,
			LineWidthsOptions options) const;

		void setText(
			const style::TextStyle& style,
			const QString& text,
			const TextParseOptions& options = kDefaultTextOptions);

		[[nodiscard]] bool hasLinks() const;
		void setLink(uint16 index, const ClickHandlerPtr& lnk);

		[[nodiscard]] bool hasCollapsedBlockquots() const;
		[[nodiscard]] bool blockquoteCollapsed(int index) const;
		[[nodiscard]] bool blockquoteExpanded(int index) const;
		void setBlockquoteExpanded(int index, bool expanded);
		void setBlockquoteExpandCallback(
			Fn<void(int index, bool expanded)> callback);

		[[nodiscard]] bool hasSkipBlock() const;
		bool updateSkipBlock(int width, int height);
		bool removeSkipBlock();

		[[nodiscard]] int maxWidth() const {
			return _maxWidth;
		}
		[[nodiscard]] int minHeight() const {
			return _minHeight;
		}


		[[nodiscard]] int countMaxMonospaceWidth() const;

		void draw(
			QPainter& painter,
			const PaintContext& context) const;

		[[nodiscard]] bool isEmpty() const;
		[[nodiscard]] int length() const {
			return _text.size();
		}

		[[nodiscard]] QString toString(
			TextSelection selection = AllTextSelection) const;
		[[nodiscard]] TextWithEntities toTextWithEntities(
			TextSelection selection = AllTextSelection) const;
		[[nodiscard]] TextForMimeData toTextForMimeData(
			TextSelection selection = AllTextSelection) const;

		[[nodiscard]] int lineHeight() const;

		TextSelection adjustSelection(
			TextSelection selection,
			TextSelection::Type selectType);
		void clear();
		void recountNaturalSize(
			bool initial,
			Qt::LayoutDirection optionsDirection = Qt::LayoutDirectionAuto);

		[[nodiscard]] const std::vector<Modification>& modifications() const;

		[[nodiscard]] TextState getState(
			QPoint point,
			int width,
			StateRequest request) const;
		[[nodiscard]] TextState getStateLeft(
			QPoint point,
			int width,
			int outerw,
			StateRequest request) const;

		[[nodiscard]] TextState getStateElided(
			QPoint point,
			int width,
			StateRequestElided request) const;
		[[nodiscard]] TextState getStateElidedLeft(
			QPoint point,
			int width,
			int outerw,
			StateRequestElided request) const;

		const style::TextStyle* style() const noexcept {
			return _st;
		}
	private:
		class ExtendedWrap : public std::unique_ptr<ExtendedData> {
		public:
			ExtendedWrap() noexcept;
			ExtendedWrap(ExtendedWrap&& other) noexcept;
			ExtendedWrap& operator=(ExtendedWrap&& other) noexcept;
			~ExtendedWrap();

			ExtendedWrap(
				std::unique_ptr<ExtendedData>&& other) noexcept;
			ExtendedWrap& operator=(
				std::unique_ptr<ExtendedData>&& other) noexcept;

		private:
			void adjustFrom(const ExtendedWrap* other);

		};

		[[nodiscard]] not_null<ExtendedData*> ensureExtended();
		[[nodiscard]] not_null<QuotesData*> ensureQuotes();

		[[nodiscard]] uint16 blockPosition(
			std::vector<Block>::const_iterator i,
			int fullLengthOverride = -1) const;
		[[nodiscard]] uint16 blockEnd(
			std::vector<Block>::const_iterator i,
			int fullLengthOverride = -1) const;
		[[nodiscard]] uint16 blockLength(
			std::vector<Block>::const_iterator i,
			int fullLengthOverride = -1) const;

		[[nodiscard]] QuoteDetails* quoteByIndex(int index) const;
		[[nodiscard]] const style::QuoteStyle& quoteStyle(
			not_null<QuoteDetails*> quote) const;
		[[nodiscard]] QMargins quotePadding(QuoteDetails* quote) const;
		[[nodiscard]] int quoteMinWidth(QuoteDetails* quote) const;
		[[nodiscard]] const QString& quoteHeaderText(QuoteDetails* quote) const;

		// Returns -1 in case there is no limit.
		[[nodiscard]] int quoteLinesLimit(QuoteDetails* quote) const;

		// Block must be either nullptr or a pointer to a NewlineBlock.
		[[nodiscard]] int quoteIndex(const AbstractBlock* block) const;

		// Template method for originalText(), originalTextWithEntities().
		template <
			typename AppendPartCallback,
			typename ClickHandlerStartCallback,
			typename ClickHandlerFinishCallback,
			typename FlagsChangeCallback>
		void enumerateText(
			TextSelection selection,
			AppendPartCallback appendPartCallback,
			ClickHandlerStartCallback clickHandlerStartCallback,
			ClickHandlerFinishCallback clickHandlerFinishCallback,
			FlagsChangeCallback flagsChangeCallback) const;

		// Template method for countWidth(), countHeight(), countLineWidths().
		// callback(lineWidth, lineBottom) will be called for all lines with:
		// QFixed lineWidth, int lineBottom
		template <typename Callback>
		void enumerateLines(
			int w,
			bool breakEverywhere,
			Callback&& callback) const;
		template <typename Callback>
		void enumerateLines(
			GeometryDescriptor geometry,
			Callback&& callback) const;

		void insertModifications(int position, int delta);
		void removeModificationsAfter(int size);

		[[nodiscard]] TextForMimeData toText(
			TextSelection selection,
			bool composeExpanded,
			bool composeEntities) const;

		const style::TextStyle* _st = new style::TextStyle();

		QString _text;
		Blocks _blocks;
		Words _words;
		ExtendedWrap _extended;

		int _minResizeWidth = 0;
		int _maxWidth = 0;
		int _minHeight = 0;
		uint16 _startQuoteIndex = 0;
		bool _startParagraphLTR : 1 = false;
		bool _startParagraphRTL : 1 = false;
		bool _hasCustomEmoji : 1 = false;
		bool _isIsolatedEmoji : 1 = false;
		bool _isOnlyCustomEmoji : 1 = false;
		bool _hasNotEmojiAndSpaces : 1 = false;
		bool _skipBlockAddedNewline : 1 = false;
		bool _endsWithQuoteOrOtherDirection : 1 = false;

		friend class BlockParser;
		friend class WordParser;
		friend class StackEngine;
		friend class BidiAlgorithm;
		friend class Renderer;
	};
} // namespace text
