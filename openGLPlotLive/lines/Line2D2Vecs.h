//
// Created by tbatt on 11/04/2020.
//

#ifndef OPENGLPLOTLIVE_PROJ_LINE2D2VECS_H
#define OPENGLPLOTLIVE_PROJ_LINE2D2VECS_H

#include "ISingleLine2D.h"

#include <span>

namespace GLPL  {
    class Line2D2Vecs : public ISingleLine2D {
        // Two vectors corresponding to an x vector and a y vector
    public:
        /* Constructor */
        Line2D2Vecs(std::shared_ptr<std::span<float>> dataPtX, std::shared_ptr<std::span<float>> dataPtY,
                std::shared_ptr<ParentDimensions> parentDimensions, GLenum mode = GL_LINE_STRIP);

        /* Destructor */
        ~Line2D2Vecs();

        /* Functions */
        void updateInternalData();
        void updateIncrementalInternalData();
        void Draw();
        std::string getID();
        void clearData();
        std::vector<float> getMinMax(bool onlyPositiveX, bool onlyPositiveY);
        std::tuple<float, float> getClosestPoint(float xVal);
        std::tuple<float, float> getClosestPoint(float xVal, float xmin, float xmax, float ymin, float ymax);

        std::vector<float> getInternalData();
        std::vector<unsigned int> getInternalIndices();

        std::shared_ptr<std::span<float>> dataPtX;
        std::shared_ptr<std::span<float>> dataPtY;

    protected:
        /* Data */
        int nPts = 0;

        std::vector<float> internalData;    // This is sorted by x values
        std::vector<unsigned int> internalIndices;   // This keeps track of the indices in their original order, for plotting
        bool initialized = false;
    };

}


#endif //OPENGLPLOTLIVE_PROJ_LINE2D2VECS_H
