#pragma once

#include "../../core/Types.h"
#include "../style/StyleTypes.h"

#include "AbstractLayoutItem.h"


namespace Ui::Layout {
	struct Position {
		int row = -1;
		int column = -1;
	};

	namespace {
		inline constexpr auto kMatrixRowShift = 40000;

		[[nodiscard]] Position indexToPosition(int index);

		[[nodiscard]] int positionToIndex(int row, int column);
		[[nodiscard]] int positionToIndex(const Position& position);
	} // namespace


	struct FoundItem {
		int index = -1;
		bool exact = false;
		QPoint relative;
	};


	class AbstractMosaicLayout {
	public:
		AbstractMosaicLayout(int bigWidth);

		[[nodiscard]] int rowHeightAt(int row) const;
		[[nodiscard]] int countDesiredHeight(int newWidth);

		[[nodiscard]] FoundItem findByPoint(const QPoint& globalPoint) const;
		[[nodiscard]] QRect findRect(int index) const;

		void setRightSkip(int rightSkip);
		void setPadding(QMargins padding);
		void setFullWidth(int w);

		[[nodiscard]] bool empty() const;
		[[nodiscard]] int rowsCount() const;

		void setMinimumWidth(int width);
		void setMinimumHeight(int height);

		void clearRows(bool resultsDeleted);
	protected:
		void addItems(gsl::span<const not_null<AbstractLayoutItem*>> items);

		[[nodiscard]] not_null<AbstractLayoutItem*> itemAt(int row, int column) const;
		[[nodiscard]] not_null<AbstractLayoutItem*> itemAt(int index) const;

		[[nodiscard]] AbstractLayoutItem* maybeItemAt(int row, int column) const;
		[[nodiscard]] AbstractLayoutItem* maybeItemAt(int index) const;

		void forEach(Fn<void(not_null<const AbstractLayoutItem*>)> callback);

		void paint(
			Fn<void(not_null<AbstractLayoutItem*>, QPoint)> paintItem,
			const QRect& clip) const;
		[[nodiscard]] int validateExistingRows(
			Fn<bool(not_null<const AbstractLayoutItem*>, int)> checkItem,
			int count);
	private:
		static constexpr auto kInlineItemsMaxPerRow = 5;
		struct Row {
			int maxWidth = 0;
			int height = 0;
			std::vector<AbstractLayoutItem*> items;
		};

		void addItem(not_null<AbstractLayoutItem*> item, Row& row, int& sumWidth);
		[[nodiscard]] bool rowFinalize(Row& row, int& sumWidth, bool force);
		void layoutRow(Row& row, int fullWidth);

		int _minWidth = 0;
		int _minHeight = 0;

		int _bigWidth;
		int _width = 0;
		int _rightSkip = 0;
		QMargins _padding;
		std::vector<Row> _rows;

	};

} // namespace Ui::Layout