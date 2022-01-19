//
// Created by tbatt on 19/04/2020.
//

// Standard Includes
#include <utility>

// Project Includes
#include "axes.h"



namespace GLPL {

    Axes::Axes(float x, float y, float width, float height,
               std::shared_ptr<ParentDimensions> parentDimensions) :
            ConstantXYDrawable(x, y, width, height, CONSTANT_SCALE, CONSTANT_SCALE, std::move(parentDimensions)) {

        // Set Bounding Box Color
        boundingBoxColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        // Set Not Hoverable
        setHoverable(false);

        // Add axes area
        Axes::createAxesArea();

        // Create Axes
        Axes::createAxesLines();

    }

    void Axes::createAxesArea() {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();

        // Register Child
        std::shared_ptr<IDrawable> axesAreaObj = std::make_shared<AxesArea>(0, 0, 1, 1, newParentPointers);
        std::shared_ptr<AxesArea> axesAreaPt = std::dynamic_pointer_cast<AxesArea>(axesAreaObj);
        Axes::registerChild(axesAreaObj);
        // Store Text String
        this->axesArea = axesAreaPt;
    }

    void Axes::createAxesLines() {
        // Get parent pointers
        std::shared_ptr<ParentDimensions> ourParentDimensions = createParentDimensions();
        // Create axes
        axesArea->addAxesLine("x", X_AXES_CENTRE);
        axesArea->addAxesLine("y", Y_AXES_CENTRE);
    }

    void Axes::addText(const char* textString, float x, float y, float fontSize, AttachLocation attachLocation) {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Register Child
        std::shared_ptr<IDrawable> textStringObj = std::make_shared<TextString>(textString, x, y, fontSize, newParentPointers);
        std::shared_ptr<TextString> textStringPt = std::dynamic_pointer_cast<TextString>(textStringObj);
        // Set pin position
        textStringPt->setAttachLocation(attachLocation);
        // Register Child
        Axes::registerChild(textStringObj);
        // Store Text String
        textStringMap.insert(std::pair<unsigned int, std::shared_ptr<TextString>>(textStringCount, textStringPt));
        textStringCount += 1;
    }

    std::shared_ptr<TextString> Axes::getText(unsigned int textStringId) {
        if (textStringMap.count(textStringId) > 0) {
            return textStringMap.at(textStringId);
        } else {
            std::cout << "TextString " << textStringId << " does not exist!" << std::endl;
            return nullptr;
        }
    }

    void Axes::removeTextString(unsigned int textStringId) {
        if (textStringMap.count(textStringId) > 0) {
            std::shared_ptr<TextString> textString2Remove = textStringMap.at(textStringId);
            // Remove child
            IDrawable::removeChild(textString2Remove);
            // Remove axes
            textStringMap.erase(textStringId);
        } else {
            std::cout << "Cannot remove TextString " << textStringId << ", TextString does not exist!" << std::endl;
        }
    }

    void Axes::Draw() {
        // Sort children if required
        IDrawable::sortChildren();
        // Draw children
        for(auto & i : children) {
            i->Draw();
        }
    }

    std::string Axes::getID() {
        return "Axes:" + std::to_string(x) + ":" + std::to_string(y);
    }

    std::shared_ptr<ILine2D> Axes::addLine(std::shared_ptr<std::span<float>> dataPtX, std::shared_ptr<std::span<float>> dataPtY, LineType lineType, glm::vec3 colour,
                       float opacityRatio) {
        return axesArea->addLine(dataPtX, dataPtY, lineType, colour, opacityRatio);
    }

    std::shared_ptr<IScatterPlot> Axes::addScatterPlot(std::vector<float> *dataPtX, std::vector<float> *dataPtY,
                                                       glm::vec3 colour, float opacityRatio, MarkerType markerType) {
        return axesArea->addScatterPlot(dataPtX, dataPtY, colour, opacityRatio, markerType);
    }

    void Axes::setAxesBoxOn(bool axesBoxOnBool) {
        axesArea->setAxesBoxOn(axesBoxOnBool);
    }

    void Axes::setLogScale(bool logOn, unsigned int newLogBase, LogAxes logAxes) {
        axesArea->setLogScale(logOn, newLogBase, logAxes);
    }

    void Axes::setTitle(std::string newTitle) {
        axesArea->setText("axes-title", std::move(newTitle));
    }

    void Axes::setXLabel(std::string newXLabel) {
        axesArea->setText("x-label", std::move(newXLabel));
    }

    void Axes::setYLabel(std::string newYLabel) {
        axesArea->setText("y-label", std::move(newYLabel));
    }

    void Axes::setXLabelRotation(TextRotation newTextRotation) {
        axesArea->setTextRotation("x-label", newTextRotation);
    }

    void Axes::setYLabelRotation(TextRotation newTextRotation) {
        axesArea->setTextRotation("y-label", newTextRotation);
    }

    void Axes::setButtonState(const std::string& buttonName, bool activeState) {
        axesArea->setButtonState(buttonName, activeState);
    }

    void Axes::showTitle(bool show) {
        axesArea->showTitle(show);
    }
}