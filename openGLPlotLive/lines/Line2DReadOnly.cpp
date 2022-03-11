#include "Line2DReadOnly.h"

namespace GLPL {

	Line2DReadOnly::Line2DReadOnly(std::shared_ptr<ParentDimensions> parentDimensions, GLenum mode) 
		: IShadedLine2D(std::move(parentDimensions))
	{
		setVisible(false);
		this->setMode(mode);
	}

	Line2DReadOnly::Line2DReadOnly(float* dataPtX, float* dataPtY, size_t dataSize,
		std::shared_ptr<ParentDimensions> parentDimensions,
		GLenum mode) : IShadedLine2D(std::move(parentDimensions))
	{
		setVisible(false);
		this->setMode(mode);
		setSourceArrays(dataPtX, dataPtY, dataSize);
	}

	Line2DReadOnly::~Line2DReadOnly() {
	}

	void Line2DReadOnly::clearBuffer()
	{
		m_clear = true;
		setVisible(false);
	}

	void GLPL::Line2DReadOnly::setSourceArrays(float* xArray, float* yArray, size_t dataSize)
	{
		m_dataPtX = xArray;
		m_dataPtY = yArray;
		m_dataSize = dataSize;

		internalData.resize(2 * dataSize);
	}

	void Line2DReadOnly::updateInternalData() {
		
		auto dataPtX = std::vector<float>(m_dataPtX, m_dataPtX + m_dataSize);
		auto dataPtY = std::vector<float>(m_dataPtY, m_dataPtY + m_dataSize);

		// Sort by x value
		std::pair<std::vector<unsigned int>, std::vector<unsigned int>> outputIndices = genIndicesSortedVector(&dataPtX);
		std::vector<unsigned int> indicesForSorting = outputIndices.first;
		std::vector<unsigned int> finalIndices = outputIndices.second;

		// Sort data by these indices
		std::vector<float> sortedX = sortVectorByIndices(&dataPtX, indicesForSorting);
		std::vector<float> sortedY = sortVectorByIndices(&dataPtY, indicesForSorting);
		internalIndices = finalIndices;

		// Update with new data
		for (int i = 0; i < m_dataSize; i++) {
			internalData[2 * i] = sortedX[i];
			internalData[2 * i + 1] = sortedY[i];
		}

		if (!m_initialized)
		{
			m_initialized = true;
			setVisible(true);

			int vertDataSizeBytes = internalData.size() * sizeof(internalData[0]);
			int indicesDataSizeBytes = internalIndices.size() * sizeof(internalIndices[0]);
			createAndSetupBuffers(vertDataSizeBytes, indicesDataSizeBytes,
				internalData.data(), internalIndices.data(),
				2 * sizeof(internalData[0]));
		}

		// Update buffer and attributes
		glBindVertexArray(lineVAO);
		// Pts
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, internalData.size() * sizeof(internalData[0]), &internalData[0], GL_DYNAMIC_DRAW);
		// Indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, internalIndices.size() * sizeof(internalIndices[0]), &internalIndices[0], GL_DYNAMIC_DRAW);

		glBindVertexArray(0);
	}

	void Line2DReadOnly::Draw() {
		if (!m_initialized)
			return;

		if (m_clear)
		{
			glBindVertexArray(lineVAO);
			glDeleteBuffers(1, &lineVBO);
			glDeleteBuffers(1, &lineEBO);
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &lineVAO);

			m_initialized = false;
			m_clear = false;
			return;
		}

		// Draw plot
		//if (isSelected())
		//	drawData(m_dataSize, true);

		if (getVisible())
			drawData(m_dataSize, selected);
	}

	std::string Line2DReadOnly::getID() {
		return "Line2DReadOnly:" + std::to_string(x) + ":" + std::to_string(y);
	}

	std::vector<float> Line2DReadOnly::getMinMax(bool onlyPositiveX, bool onlyPositiveY) {
		// Gets the minimum and maximum values of both x and y for the data
		if (internalData.size() > 1) {
			constexpr float maxFloat = std::numeric_limits<float>::max();
			float xmin = maxFloat;
			float xmax = -maxFloat;
			float ymin = maxFloat;
			float ymax = -maxFloat;
			for (unsigned int i = 0; i < internalData.size() / 2.0; i++) {
				float xval = (internalData)[2 * i];
				float yval = (internalData)[2 * i + 1];
				if (xval > xmax && (!onlyPositiveX || xval > 0)) {
					xmax = xval;
				}
				if (xval < xmin && (!onlyPositiveX || xval > 0)) {
					xmin = xval;
				}
				if (yval > ymax && (!onlyPositiveY || yval > 0)) {
					ymax = yval;
				}
				if (yval < ymin && (!onlyPositiveY || yval > 0)) {
					ymin = yval;
				}
			}

			return std::vector<float>{xmin, xmax, ymin, ymax};
		}
		else {
			return std::vector<float>{};
		}
	}

	std::tuple<float, float> Line2DReadOnly::getClosestPoint(float xVal) {
		unsigned int ind = binarySearch(internalData, 0, (internalData.size() / 2) - 1, xVal);
		if (ind < internalData.size() / 2) {
			// Check which point is closer
			if (ind > 1 && ind < internalData.size() / 2 - 1) {
				float diffLeft = abs(internalData[2 * ind] - xVal);
				float diffRight = abs(internalData[2 * (ind + 1)] - xVal);
				if (diffRight < diffLeft) {
					// Use the right index
					ind += 1;
				}
			}
			return std::make_tuple(internalData[2 * ind], internalData[2 * ind + 1]);
		}
		else {
			return std::make_tuple(0, 0);
		}
	}

	std::tuple<float, float> Line2DReadOnly::getClosestPoint(float xVal, float xmin, float xmax, float ymin, float ymax) {
		// Scale by a little amount
		xmin = xmin - (0.01f * abs(xmin));
		xmax = xmax + (0.01f * abs(xmax));
		ymin = ymin - (0.01f * abs(ymin));
		ymax = ymax + (0.01f * abs(ymax));
		// Get the closest point, but within the window of provided ranges
		std::vector<float> filteredData;
		for (unsigned int i = 0; i < internalData.size() / 2.0; i++) {
			float x = internalData[2 * i];
			float y = internalData[2 * i + 1];
			if (x >= xmin && x <= xmax && y >= ymin && y <= ymax) {
				filteredData.push_back(x);
				filteredData.push_back(y);
			}
		}

		// Find the closest value
		if (filteredData.size() > 1) {
			unsigned int ind = binarySearch(filteredData, 0, (filteredData.size() / 2) - 1, xVal);
			if (ind < filteredData.size() / 2) {
				// Check which point is closer
				if (ind > 1 && ind < filteredData.size() / 2 - 1) {
					float diffLeft = abs(filteredData[2 * ind] - xVal);
					float diffRight = abs(filteredData[2 * (ind + 1)] - xVal);
					if (diffRight < diffLeft) {
						// Use the right index
						ind += 1;
					}
				}
				return std::make_tuple(filteredData[2 * ind], filteredData[2 * ind + 1]);
			}
		}

		return std::make_tuple(0, 0);
	}
}