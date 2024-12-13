#pragma once 


#include "../../core/Types.h"
#include "TextClickHandlers.h"

#include "TextEntities.h"
#include "../../core/CoreUtility.h"

#include "TextUtility.h"
#include "TextBlock.h"

#include "TextWord.h"

#include <QString>
#include <QPainter>

#include <xutility>
#include <private/qfixed_p.h>


namespace text {

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

	struct TextParseOptions {
		int32 flags;
		int32 maxw;
		int32 maxh;
		Qt::LayoutDirection dir;
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

		String(String&& other) = default;
		String(const QString& string);

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

		void setText(const QString& text);

		[[nodiscard]] bool hasLinks() const;
		void setLink(uint16 index, const ClickHandlerPtr& lnk);

		[[nodiscard]] int maxWidth() const {
			return _maxWidth;
		}
		[[nodiscard]] int minHeight() const {
			return _minHeight;
		}

		[[nodiscard]] int countMaxMonospaceWidth() const;

		void draw(
			QPainter& painter,
			int32 left,
			int32 top,
			int32 width,
			int32 yFrom = 0,
			int32 yTo = -1,
			TextSelection selection = { 0, 0 },
			bool fullWidthSelection = true) const;

		[[nodiscard]] bool isEmpty() const;
		[[nodiscard]] int length() const {
			return _text.size();
		}

		[[nodiscard]] QString toString(
			TextSelection selection = AllTextSelection) const;

		[[nodiscard]] int lineHeight() const;

		TextSelection adjustSelection(
			TextSelection selection,
			TextSelection::Type selectType);
		void clear();
		void recountNaturalSize(
			bool initial,
			Qt::LayoutDirection optionsDirection);

		[[nodiscard]] const std::vector<Modification>& modifications() const;
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
		void recountNaturalSize(
			bool initial,
			Qt::LayoutDirection optionsDir = Qt::LayoutDirectionAuto);

		[[nodiscard]] TextForMimeData toText(
			TextSelection selection,
			bool composeExpanded,
			bool composeEntities) const;

		QString _text;
		Blocks _blocks;
		Words _words;
		ExtendedWrap _extended;

		style::font _font;

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
	};
} // namespace text
