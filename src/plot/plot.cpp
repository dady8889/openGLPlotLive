//
// Created by tbatt on 19/04/2020.
//

#include "plot.h"


namespace GLPL {

    Plot::Plot(float x, float y, float width, float height, const ParentDimensions& parentDimensions) :
            ConstantScaleDrawable(x, y, width, height, parentDimensions) {

        // Set Bounding Box Color
        boundingBoxColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        // Add default axes
        Plot::addAxes(0.1f, 0.1f, 0.7f, 0.6f);

    }

    Plot::Plot::~Plot() {

    }

    void Plot::addAxes(float x, float y, float width, float height) {
        // Create axes
        ParentDimensions newParentPointers = IDrawable::createParentDimensions();
        // Register child
        std::shared_ptr<IDrawable> axes = std::make_shared<Axes>(x, y, width, height, newParentPointers);
        std::shared_ptr<Axes> axesPt = std::dynamic_pointer_cast<Axes>(axes);
        Plot::registerChild(axes);
        // Store axes
        axesList.push_back(axesPt);
    }

    void Plot::Plot::Draw() {
        for(auto & i : children) {
            i->Draw();
        }
    }

}