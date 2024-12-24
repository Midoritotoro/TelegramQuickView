#pragma once 

#include <QOpenGlFunctions>



namespace gl {
	class Image {
	public:
		Image();

		void bind();
		void draw();
		
	};
		 
} // namespace gl