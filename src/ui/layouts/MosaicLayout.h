#pragma once


#include "AbstractMosaicLayout.h"


namespace Ui::Layout {
	template<
		typename ItemBase,
		typename = std::enable_if_t<
		std::is_base_of_v<AbstractLayoutItem, ItemBase>>>

	class MosaicLayout final : public AbstractMosaicLayout {
	using Parent = AbstractMosaicLayout;

	public:
		using Parent::Parent;

		void addItems(const std::vector<not_null<ItemBase*>>& items) {
			Parent::addItems({
				reinterpret_cast<const not_null<AbstractLayoutItem*>*>(
					items.data()),
				items.size() });
		}

		void addItem(not_null<ItemBase*> item) {
			Parent::addItems({
				reinterpret_cast<const not_null<AbstractLayoutItem*>*>(
					item),
				1});
		}

		[[nodiscard]] not_null<ItemBase*> itemAt(int row, int column) const {
			return Downcast(Parent::itemAt(row, column));
		}

		[[nodiscard]] not_null<ItemBase*> itemAt(int index) const {
			return Downcast(Parent::itemAt(index));
		}

		[[nodiscard]] ItemBase* maybeItemAt(int row, int column) const {
			return Downcast(Parent::maybeItemAt(row, column));
		}

		[[nodiscard]] ItemBase* maybeItemAt(int index) const {
			return Downcast(Parent::maybeItemAt(index));
		}

		void forEach(Fn<void(not_null<const ItemBase*>)> callback) {
			Parent::forEach([&](
				not_null<const AbstractLayoutItem*> item) {
					callback(Downcast(item));
				});
		}

		void paint(
			Fn<void(not_null<ItemBase*>, QPoint)> paintItem,
			const QRect& clip) const {
			Parent::paint([&](
				not_null<AbstractLayoutItem*> item,
				QPoint point) {
					paintItem(Downcast(item), point);
				}, clip);
		}

		int validateExistingRows(
			Fn<bool(not_null<const ItemBase*>, int)> checkItem,
			int count) {
			return Parent::validateExistingRows([&](
				not_null<const AbstractLayoutItem*> item,
				int until) {
					return checkItem(Downcast(item), until);
				}, count);
		}

	private:
		[[nodiscard]] static not_null<ItemBase*> Downcast(
			not_null<AbstractLayoutItem*> item) {
			return static_cast<ItemBase*>(item.get());
		}

		[[nodiscard]] static ItemBase* Downcast(
			AbstractLayoutItem* item) {
			return static_cast<ItemBase*>(item);
		}

		[[nodiscard]] static not_null<const ItemBase*> Downcast(
			not_null<const AbstractLayoutItem*> item) {
			return static_cast<const ItemBase*>(item.get());
		}

		[[nodiscard]] static const ItemBase* Downcast(
			const AbstractLayoutItem* item) {
			return static_cast<const ItemBase*>(item);
		}
	};
} // namespace Ui::Layout
