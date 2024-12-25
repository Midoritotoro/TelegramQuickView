#include "StyleCore.h"

#include "../../media/ffmpeg/video/FrameGenerator.h"
#include <QMimeDataBase>

#include <QPainterPath>
#include <QPainter>

#include <QFile>
#include <QDir>

#include <QCoreApplication>
#include "StyleFont.h"

#include "StyleWidgets.h"
#include "../../media/images/ImagesBlur.h"

#include "../../core/CoreUtility.h"
#include <QMediaMetaData>


namespace style {
	namespace {
		bool rightToLeft = false;
	} // namespace

	bool RightToLeft() {
		return rightToLeft;
	}

	void SetRightToLeft(bool _rightToLeft) {
		rightToLeft = rightToLeft;
	}

	Qt::LayoutDirection LayoutDirection() {
		return RightToLeft() ? Qt::RightToLeft : Qt::LeftToRight;
	}

	void RoundCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius,
		CornersRoundMode roundType) 
	{
		auto path = QPainterPath();
		path.setFillRule(Qt::WindingFill);

		switch (roundType) {
			case CornersRoundMode::Top:
				path.moveTo(borderRadius, 0);
				path.lineTo(widgetSize.width() - borderRadius, 0);

				path.arcTo(widgetSize.width() - 2 * borderRadius, 0, 
					2 * borderRadius, 2 * borderRadius, 90, -90);
				path.lineTo(widgetSize.width(), widgetSize.height());

				path.lineTo(0, widgetSize.height());
				path.lineTo(0, borderRadius);

				path.arcTo(0, 0, 2 * borderRadius, 2 * borderRadius, 180, -90);

				break;

			case CornersRoundMode::Bottom:
				path.moveTo(0, 0);

				path.lineTo(widgetSize.width(), 0);

				path.lineTo(widgetSize.width(), widgetSize.height() - borderRadius);
				path.quadTo(widgetSize.width(), widgetSize.height(),
					widgetSize.width() - borderRadius, widgetSize.height());

				path.lineTo(borderRadius, widgetSize.height());
				path.quadTo(0, widgetSize.height(), 0,
					widgetSize.height() - borderRadius);

				path.lineTo(0, 0);

				break;

			case CornersRoundMode::All:
				path.moveTo(borderRadius, 0);

				path.lineTo(widgetSize.width() - borderRadius, 0);
				path.quadTo(widgetSize.width(), 0, widgetSize.width(), borderRadius);

				path.lineTo(widgetSize.width(), widgetSize.height() - borderRadius);
				path.quadTo(widgetSize.width(), widgetSize.height(),
					widgetSize.width() - borderRadius, widgetSize.height());

				path.lineTo(borderRadius, widgetSize.height());
				path.quadTo(0, widgetSize.height(), 0,
					widgetSize.height() - borderRadius);

				path.lineTo(0, borderRadius);
				path.quadTo(0, 0, borderRadius, 0);

				break;
		}

		painter.setClipPath(path);
	}

	void RoundCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius)
	{
		return RoundCorners(painter, widgetSize, borderRadius, CornersRoundMode::All);
	}

	void RoundTopCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius) 
	{
		return RoundCorners(painter, widgetSize, borderRadius, CornersRoundMode::Top);
	}

	void RoundBottomCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius) 
	{
		return RoundCorners(painter, widgetSize, borderRadius, CornersRoundMode::Bottom);
	}

	void Start() {
		internal::RegisterFontFamily("Open Sans");
	}

	void Stop() {
		internal::DestroyFonts();
	}

	void RegisterStyles() {
		loadStyles();

		defaultTextStyle = new TextStyle{
		   ._font = style::font(13, 0, 0),
		   .lineHeight = 14,
		   .linkUnderLine = true,
		   .blockquote = {} };

		defaultFlatLabelStyle = new FlatLabel{
			.margin = margins{
				10, 10, 10, 10
			},
			.colorBg = QColor(24, 37, 51),
			.textStyle = defaultTextStyle,
			.textPalette = &defaultTextPalette,
			.maximumWidth = style::maximumMessageWidth,
			.maximumHeight = style::maximumTextHeight,
			.minimumWidth = style::minimumMessageWidth,
			.minimumHeight = style::minimumMessageHeight
		};
	}
} // namespace style