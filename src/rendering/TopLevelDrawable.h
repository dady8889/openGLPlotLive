//
// Created by tbatt on 17/06/2020.
//

#ifndef OPENGLPLOTLIVE_PROJ_TOPLEVELDRAWABLE_H
#define OPENGLPLOTLIVE_PROJ_TOPLEVELDRAWABLE_H

#include "IDrawable.h"


namespace GLPL {
    class TopLevelDrawable : public IDrawable {
    public:
        // Constructor
        TopLevelDrawable(float x, float y, int widthPx, int heightPx);

        // Functions
        void setPosition(float newX, float newY);
        void setSize(float newWidth, float newHeight);
        void setParentDimensions(glm::mat4 newParentTransform,
                                 int newParentWidthPx,
                                 int newParentHeightPx);
        void setParentDimensions(const ParentDimensions& parentDimensions);


    };
}


#endif //OPENGLPLOTLIVE_PROJ_TOPLEVELDRAWABLE_H
