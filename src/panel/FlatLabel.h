#pragma once

#include "../core/StyleCore.h"
#include <QWidget>

namespace style {
	namespace flatLabel {
		inline constexpr QMargins margins = {
				8, 5, 20, 10
		};
	}
} // namespace style


struct TextSelection {
	enum class Type {
		Letters = 0x01,
		Words = 0x02,
		Paragraphs = 0x03,
	};

	constexpr TextSelection() = default;
	constexpr TextSelection(quint16 from, quint16 to) : from(from), to(to) {
	}

	[[nodiscard]] constexpr bool empty() const noexcept {
		return from == to;
	}
	quint16 from = 0;
	quint16 to = 0;
};

inline bool operator==(TextSelection a, TextSelection b) {
	return a.from == b.from && a.to == b.to;
}

inline bool operator!=(TextSelection a, TextSelection b) {
	return !(a == b);
}

static constexpr TextSelection AllTextSelection = { 0, 0xFFFF };


class FlatLabel: public QWidget {
	Q_OBJECT
public:
	FlatLabel(QWidget* parent = nullptr);

	[[nodiscard]] QSize sizeHint() const override;

	[[nodiscard]] int textMaxWidth() const noexcept;
	[[nodiscard]] bool hasLinks() const noexcept;

	void setText(const QString& text);
	[[nodiscard]] QString text() const noexcept;

	void setSelectable(bool selectable);
	[[nodiscard]] bool selectable() const noexcept;

	void setOpacity(float opacity);
	[[nodiscard]] float opacity() const noexcept;

	void setTextAlignment(Qt::Alignment alignment);
	[[nodiscard]] Qt::Alignment alignment() const noexcept;

	void setContextMenu(QMenu* menu);
	[[nodiscard]] QMenu* contextMenu() const noexcept;

	void setLink(quint16 index);
protected:
	void paintEvent(QPaintEvent* e) override;
	void mouseMoveEvent(QMouseEvent* e) override;
	void mousePressEvent(QMouseEvent* e) override;

	void mouseReleaseEvent(QMouseEvent* e) override;
	void mouseDoubleClickEvent(QMouseEvent* e) override;
	
	void focusOutEvent(QFocusEvent* e) override;
	void focusInEvent(QFocusEvent* e) override;

	void keyPressEvent(QKeyEvent* e) override;
	void contextMenuEvent(QContextMenuEvent* e) override;
private:
	[[nodiscard]] int countTextWidth() const noexcept;
	[[nodiscard]] int countTextHeight();

	void refreshSize();
	void textUpdated();

	QString _text = "";
	Qt::Alignment _alignment;

	float _opacity = 1.;
	bool _selectable;

	int _allowedWidth = 0;
	int _textWidth = 0;

	int _fullTextHeight = 0;

	TextSelection _selection, _savedSelection;
	QMenu* _contextMenu = nullptr;
};
