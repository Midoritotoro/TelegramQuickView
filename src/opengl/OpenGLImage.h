#pragma once 

#include <QOpenGlFunctions>

#include <QImage>
#include <QPixmap>

#include <gsl/gsl>


namespace OpenGL {
	class Image {
	public:
		Image();

		void bind();
		void draw();

		void setImage(const QImage& image);
		void setPixmap(const QPixmap& pixmap);
		
		[[nodiscard]] QImage toImage() const;
		[[nodiscard]] QPixmap toPixmap() const;
	};
		 
} // namespace OpenGL