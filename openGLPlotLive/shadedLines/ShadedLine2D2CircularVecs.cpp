//
// Created by tbatt on 12/04/2020.
//

#include "ShadedLine2D2CircularVecs.h"

//
// Created by tbatt on 11/04/2020.
//

#include "ShadedLine2D2CircularVecs.h"

namespace GLPL {
    ShadedLine2D2CircularVecs::ShadedLine2D2CircularVecs(std::vector<float> *dataPtX, std::vector<float> *dataPtY,
                                                         std::shared_ptr<ParentDimensions> parentDimensions,
                                                         GLenum mode) : IShadedLine2D(std::move(parentDimensions)) {
        this->dataPtX = dataPtX;
        this->dataPtY = dataPtY;
        this->setMode(mode);

        /* Setup Buffers */
        updateInternalData(0);
        int vertDataSizeBytes = internalData.size()*sizeof(internalData[0]);
        int indicesDataSizeBytes = internalIndices.size()*sizeof(internalIndices[0]);
        createAndSetupBuffers(vertDataSizeBytes, indicesDataSizeBytes,
                &internalData[0], &internalIndices[0], 2*sizeof(internalData[0]));
    }

    ShadedLine2D2CircularVecs::~ShadedLine2D2CircularVecs() {

    }

    void ShadedLine2D2CircularVecs::updateInternalData(unsigned int currIndex) {
        /* Creates an internal data store from the current dataVecPt */
        // Get minimum length of both vectors
        unsigned int totLen = std::min(this->dataPtX->size(), this->dataPtY->size());
        // Resize vector to data
        internalData.resize(4*totLen); // x1, y1, x1, 0 for each point added
        sortedInternalData.clear();
        sortedInternalData.resize(2*totLen);
        // Sort by x value
        std::pair<std::vector<unsigned int>, std::vector<unsigned int>> outputIndices = genIndicesSortedVector(dataPtX);
        std::vector<unsigned int> indicesForSorting = outputIndices.first;
        std::vector<float> sortedX = sortVectorByIndices(dataPtX, indicesForSorting);
        std::vector<float> sortedY = sortVectorByIndices(dataPtY, indicesForSorting);
        // Store sorted data
        for(unsigned int i=0; i<totLen; i++) {
            sortedInternalData[2*i] = sortedX[i];
            sortedInternalData[2*i + 1] = sortedY[i];
        }
        // Update with new data, for each point we add (2 values), add the corresponding point on the x-axes (2 values)
        // First slice
        unsigned int len1 = totLen - currIndex;
        for(unsigned int i=0; i<len1; i++) {
            internalData[4*i] = (*dataPtX)[currIndex + i];
            internalData[4*i + 1] = (*dataPtY)[currIndex + i];
            internalData[4*i + 2] = (*dataPtX)[currIndex + i];
            internalData[4*i + 3] = 0.0;
        }
        // Second slice
        unsigned int len2 = currIndex;
        for(unsigned int i=0; i<len2; i++) {
            internalData[4*i + 4*len1] = (*dataPtX)[i];
            internalData[4*i + 4*len1 + 1] = (*dataPtY)[i];
            internalData[4*i + 4*len1 + 2] = (*dataPtX)[i];
            internalData[4*i + 4*len1 + 3] = 0.0;
        }
        // Generate indices for triangles
        if (totLen > 1) {
            internalIndices.resize(6*(totLen-1)); // 6(n-1) indices added for each point (2 triangles)
            for (unsigned int n = 1; n < internalData.size() / 4; n++) {
                // Current point is pt 2n, current axes pt is 2n+1
                // Old point is 2n-2, old axes point is 2n+1-2 = 2n-1
                // For odd numbered triangles, we use the old point, old axes point and new axes point
                internalIndices[6*(n-1)]     = 2*n - 2;     // Old point
                internalIndices[6*(n-1) + 1] = 2*n - 1;     // Old axes point
                internalIndices[6*(n-1) + 2] = 2*n + 1;     // New axes point
                // For even numbered triangles, we use the new point, the new axes point and the previous point
                internalIndices[6*(n-1) + 3] = 2*n;         // New point
                internalIndices[6*(n-1) + 4] = 2*n + 1;     // New axes point
                internalIndices[6*(n-1) + 5] = 2*n - 2;     // Old point
            }
        } else {
            internalIndices.resize(0);
        }
        nIndices = internalIndices.size();
        updated = true;
    }

    void ShadedLine2D2CircularVecs::Draw() {
        // Check if the number of points changed
        if (updated) {
            nIndices = internalIndices.size();
            if (nIndices > 0) {
                // Update buffer and attributes
                glBindVertexArray(lineVAO);
                // VBO
                glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
                glBufferData(GL_ARRAY_BUFFER, internalData.size() * sizeof(internalData[0]), &internalData[0],
                             GL_DYNAMIC_DRAW);
                // EBO
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, internalIndices.size() * sizeof(internalIndices[0]),
                             &internalIndices[0], GL_DYNAMIC_DRAW);

                glBindVertexArray(0);
            }
            updated = false;
        }

        // Draw plot
        if (nIndices > 0) {
            drawData(nIndices, selected);
        }
    }

    std::string ShadedLine2D2CircularVecs::getID() {
        return "ShadedLine2D2CircularVecs:" + std::to_string(x) + ":" + std::to_string(y);
    }

    std::vector<float> ShadedLine2D2CircularVecs::getMinMax(bool onlyPositiveX, bool onlyPositiveY) {
        // Gets the minimum and maximum values of both x and y for the data
        float maxFloat = std::numeric_limits<float>::max();
        float xmin = maxFloat;
        float xmax = -maxFloat;
        float ymin = maxFloat;
        float ymax = -maxFloat;
        for (unsigned int i = 0; i<internalData.size()/2.0; i++) {
            float xval = (internalData)[2*i];
            float yval = (internalData)[2*i+1];
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

        return std::vector<float> {xmin,xmax,ymin,ymax};
    }

    std::tuple<float, float> ShadedLine2D2CircularVecs::getClosestPoint(float xVal) {
        unsigned int ind = binarySearch(sortedInternalData, 0, (sortedInternalData.size()/2) - 1, xVal, 2);
        if (ind < sortedInternalData.size()/2) {
            // Check which point is closer
            if (ind > 1 && ind < sortedInternalData.size()/2.0 - 1) {
                float diffLeft = abs(sortedInternalData[2 * ind] - xVal);
                float diffRight = abs(sortedInternalData[2 * (ind + 1)] - xVal);
                if (diffRight < diffLeft) {
                    // Use the right index
                    ind += 1;
                }
            }
            return std::make_tuple(sortedInternalData[2 * ind], sortedInternalData[2* ind + 1]);
        } else {
            return std::make_tuple(0,0);
        }
    }

    std::tuple<float, float>
    ShadedLine2D2CircularVecs::getClosestPoint(float xVal, float xmin, float xmax, float ymin, float ymax) {
        // Scale by a little amount
        xmin = xmin - (0.01f*abs(xmin));
        xmax = xmax + (0.01f*abs(xmax));
        ymin = ymin - (0.01f*abs(ymin));
        ymax = ymax + (0.01f*abs(ymax));
        // Get the closest point, but within the window of provided ranges
        std::vector<float> filteredData;
        for(unsigned int i=0; i < sortedInternalData.size()/2.0; i++) {
            float x = sortedInternalData[2*i];
            float y = sortedInternalData[2*i + 1];
            if (x >= xmin && x <= xmax && y >= ymin && y <= ymax) {
                filteredData.push_back(x);
                filteredData.push_back(y);
            }
        }

        // Find the closest value
        if (filteredData.size() > 1) {
            unsigned int ind = binarySearch(filteredData, 0, (filteredData.size()/2) - 1, xVal, 2);
            if (ind < filteredData.size()/2) {
                // Check which point is closer
                if (ind > 1 && ind < filteredData.size() / 2.0 - 1) {
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

        return std::make_tuple(0,0);
    }

}