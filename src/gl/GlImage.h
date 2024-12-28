#pragma once 

#include <QOpenGlFunctions>
#include <QImage>

namespace gl {
	class Image {
	public:
		Image();

		void bind();
		void draw();
		
	};
		 
} // namespace gl