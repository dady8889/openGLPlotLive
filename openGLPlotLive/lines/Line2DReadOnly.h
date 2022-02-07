#pragma once

#include "ISingleLine2D.h"
#include "../shadedLines/IShadedLine2D.h"

namespace GLPL {
	class Line2DReadOnly : public IShadedLine2D {
		// Two arrays corresponding to x values and y values
		// The owner of the arrays is responsible for allocation and updating.
	public:
		/* Constructor */
		Line2DReadOnly(std::shared_ptr<ParentDimensions> parentDimensions, GLenum mode = GL_LINE_STRIP);

		Line2DReadOnly(float* dataPtX, float* dataPtY, size_t dataSize,
			std::shared_ptr<ParentDimensions> parentDimensions, GLenum mode = GL_LINE_STRIP);

		/* Destructor */
		~Line2DReadOnly();

		/* Functions */
		void clearBuffer();
		void setSourceArrays(float* yArray, float* xArray, size_t dataSize);
		void updateInternalData();
		void Draw();
		std::string getID();
		std::vector<float> getMinMax(bool onlyPositiveX, bool onlyPositiveY);
		std::tuple<float, float> getClosestPoint(float xVal);
		std::tuple<float, float> getClosestPoint(float xVal, float xmin, float xmax, float ymin, float ymax);

	protected:
		/* Data */
		float* m_dataPtX{ nullptr };
		float* m_dataPtY{ nullptr };
		size_t m_dataSize{ 0 };
		bool m_initialized{ false };
		bool m_clear{ false };

		std::vector<float> internalData;    // This is sorted by x values
		std::vector<unsigned int> internalIndices;   // This keeps track of the indices in their original order, for plotting
	};

}