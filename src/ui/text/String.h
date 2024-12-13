#pragma once 


#include "../../core/Types.h"
#include "../ClickHandler.h"

#include "TextEntities.h"
#include "../../core/CoreUtility.h"

#include "TextUtility.h"
#include "TextBlock.h"

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
	private:
		void recountNaturalSize(
			bool initial,
			Qt::LayoutDirection optionsDirection);

		QString _text;

		int _maxWidth = 0;
		int _minHeight = 0;

		Blocks _blocks;
		std::vector<QString> _words;

		int _minResizeWidth = 0;

		int _maxWidth = 0;
		int _minHeight = 0;

		uint16 _startQuoteIndex = 0;

		bool _startParagraphLTR : 1;
		bool _startParagraphRTL : 1;
		bool _hasCustomEmoji : 1;
		bool _isIsolatedEmoji : 1;
		bool _isOnlyCustomEmoji : 1;
		bool _hasNotEmojiAndSpaces : 1;
		bool _skipBlockAddedNewline : 1;
		bool _endsWithQuoteOrOtherDirection : 1;
	};
} // namespace text
