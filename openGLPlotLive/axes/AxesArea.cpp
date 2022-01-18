//
// Created by tbatt on 19/06/2020.
//

// Standard Includes
#include <utility>

// Project Includes
#include "AxesArea.h"
#include "../shadedLines/ShadedLine2D2CircularVecs.h"
#include "../lines/Line2D2Vecs.h"
#include "../interaction/PressButtonWithImage.h"
#include "../scatterPlot/IScatterPlot.h"
#include "../scatterPlot/Scatter2D2Vecs.h"


namespace GLPL {

    AxesArea::AxesArea(float x, float y, float width, float height, std::shared_ptr<ParentDimensions> parentDimensions) :
        IDrawable() {
        // Set bounding box color
        boundingBoxColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        // Set Not Hoverable
        setHoverable(false);

        AxesArea::setParentDimensions(std::move(parentDimensions));

        // Initialise data
        AxesArea::setPosition(x, y);
        AxesArea::setSize(width, height);
        IDrawable::createAndSetupBuffers();

        // Calculate transform
        AxesArea::updateAxesViewportTransform();

        // Add Title
        AxesArea::addText("Axes Title", "axes-title", 0.5, 1, 14, CENTRE_BOTTOM)
            ->setOffset(0, 0, 0, 20);

        // Add Axes Label
        AxesArea::addText("x label", "x-label", 0.5, 0, 12, CENTRE_TOP)
            ->setOffset(0, 0, 8, 0);

        AxesArea::addText("y label", "y-label", 0, 0.5, 12, CENTRE_RIGHT)
            ->setOffset(0, 7, 0, 0);

        // Add Buttons
        const float buttonWidth = 45;
        const float buttonHeight = 45;
        const float buttonLocationY = 0.0f;
        const int gap = buttonWidth + 10;
        int buttonCounter = 0;

        AxesArea::addButtonWithTexture("Interactor", "interactor-white", 1, buttonLocationY, buttonWidth, buttonHeight, TOP_RIGHT, true, "Toggle the interactor")
            ->setOffset(0, gap * buttonCounter++, 0, 0);

        AxesArea::addButtonWithTexture("Axes Limits Scaling", "axes-limits-white", 1, buttonLocationY, buttonWidth, buttonHeight, TOP_RIGHT, true, "Enable/disable axes auto scaling")
            ->setOffset(0, gap * buttonCounter++, 0, 0);

        AxesArea::addButtonWithTexture("Grid", "grid-white", 1, buttonLocationY, buttonWidth, buttonHeight, TOP_RIGHT, true, "Toggle grid")
            ->setOffset(0, gap * buttonCounter++, 0, 0);

        AxesArea::addButtonWithTexture("Axes", "axes-white", 1, buttonLocationY, buttonWidth, buttonHeight, TOP_RIGHT, true, "Toggle axes")
            ->setOffset(0, gap * buttonCounter++, 0, 0);

        AxesArea::addButtonWithTexture("AxesBox", "axes-box-white", 1, buttonLocationY, buttonWidth, buttonHeight, TOP_RIGHT, true, "Toggle axes box")
            ->setOffset(0, gap * buttonCounter++, 0, 0);

        // Create Interactor
        AxesArea::createInteractor();

        // Create Grid
        AxesArea::createGrid();

        // Create Zoom Box
        AxesArea::createZoomBox();

    }

    std::string AxesArea::getID() {
        return "AxesArea:" + std::to_string(x) + ":" + std::to_string(y);
    }

    void AxesArea::Draw() {
        // Update Axes Limits
        AxesArea::updateAxesLimits();

        // Update mouse over line
        AxesArea::updateInteractor();

        // Update zoom drag box
        AxesArea::updateZoomDragBox();

        // Draw Axes
        if (buttonMap["Axes"]->isActive()) {
            for (auto &i : axesLines) {
                i.second->Draw();
            }
        }

        // Draw Grid
        AxesArea::drawGrid();

        // Scissor Test
        glEnable(GL_SCISSOR_TEST);
        glScissor(xPx, yPx, widthPx, heightPx);

        // Draw lines, scatter plots
        for(auto & i : plotableMap) {
            i.second->Draw();
        }

        // Disable Scissor Testing
        glDisable(GL_SCISSOR_TEST);

        // Draw attachments
        for(auto & i : axesItems) {
            i->Draw();
        }

        // Draw Text
        for(auto & i : textStringMap) {
            i.second->Draw();
        }

        // Draw Buttons
        for(auto & i : buttonMap) {
            i.second->Draw();
        }

        // Draw Axes box
        if (buttonMap["AxesBox"]->isActive()) {
            AxesArea::drawAxesBox();
        }

    }

    void AxesArea::onScroll(double xoffset, double yoffset) {
        AxesArea::zoomAxes(yoffset);
    }

    void AxesArea::onRightDrag(bool dragging, double origXPos, double origYPos) {
        // Reset other held buttons
        rightShiftMouseHeld = false;
        // Set new held button
        rightMouseHeld = dragging;
        mouseHeldX = origXPos;
        mouseHeldY = origYPos;

        xMinDrag = xmin;
        xMaxDrag = xmax;
        yMinDrag = ymin;
        yMaxDrag = ymax;
    }

    void AxesArea::onMiddleDrag(bool dragging, double origXPos, double origYPos) {
        // Set new held button
        middleMouseHeld = dragging;
        mouseHeldX = origXPos;
        mouseHeldY = origYPos;

        if (isHovered() && isMouseOver(mouseX, mouseY, false)) {
            xMinDrag = xmin;
            xMaxDrag = xmax;
            yMinDrag = ymin;
            yMaxDrag = ymax;
        }

        // On Release, set the current axes limits to that of the zoom box line
        if (!dragging) {
            float newXMin = std::min(zoomBoxX[0], zoomBoxX[1]);
            float newXMax = std::max(zoomBoxX[0], zoomBoxX[1]);
            float newYMin = std::min(zoomBoxY[0], zoomBoxY[2]);
            float newYMax = std::max(zoomBoxY[0], zoomBoxY[2]);
            setAxesLimits(newXMin, newXMax, newYMin, newYMax);
        }

    }

    void AxesArea::onLeftDrag(bool dragging, double origXPos, double origYPos) {
        // Reset other held buttons
        leftShiftMouseHeld = false;
        // Set new held button
        leftMouseHeld = dragging;
        mouseHeldX = origXPos;
        mouseHeldY = origYPos;

        if (isHovered() && isMouseOver(mouseX, mouseY, false)) {
            xMinDrag = xmin;
            xMaxDrag = xmax;
            yMinDrag = ymin;
            yMaxDrag = ymax;
        }

    }

    void AxesArea::onLeftShiftDrag(bool dragging, double origXPos, double origYPos) {
        // Reset other held buttons
        leftMouseHeld = false;
        // Set new held button
        leftShiftMouseHeld = dragging;
        mouseHeldX = origXPos;
        mouseHeldY = origYPos;

        if (isHovered() && isMouseOver(mouseX, mouseY, false)) {
            xMinDrag = xmin;
            xMaxDrag = xmax;
            yMinDrag = ymin;
            yMaxDrag = ymax;
        }

        // On Release, set the current axes limits to that of the zoom box line
        if (!dragging) {
            float newXMin = std::min(zoomBoxX[0], zoomBoxX[1]);
            float newXMax = std::max(zoomBoxX[0], zoomBoxX[1]);
            float newYMin = std::min(zoomBoxY[0], zoomBoxY[2]);
            float newYMax = std::max(zoomBoxY[0], zoomBoxY[2]);
            setAxesLimits(newXMin, newXMax, newYMin, newYMax);
        }

    }

    void AxesArea::setLastMousePos(double lastMouseX, double lastMouseY) {
        IDrawable::setLastMousePos(lastMouseX, lastMouseY);

        // Change zoom if right mouse is being dragged
        if (rightMouseHeld) {
            zoomAxesByDragging();
        }

        // Drag the axes if shift held with the left mouse being dragged
        if (leftMouseHeld) {
            moveAxesByDragging();
        }
    }

    void AxesArea::setAxesBoxOn(bool axesBoxOnBool) {
        (buttonMap["AxesBox"]->setActive(axesBoxOnBool));
    }

    void AxesArea::setAxesBoxColor(glm::vec4 newAxesBoxColour) {
        axesBoxColor = newAxesBoxColour;
    }

    void AxesArea::addAxesLine(const std::string& axesName, AxesDirection axesDirection) {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Create Axes
        std::shared_ptr<AxesLineTicks> newAxes = std::make_shared<AxesLineTicks>(axesDirection, newParentPointers);
        // Store Axes
        axesLines.insert(std::pair<std::string, std::shared_ptr<AxesLineTicks>>(axesName, newAxes));
        // Register as a child
        AxesArea::registerChild(newAxes);
    }

    void AxesArea::setAxesLimits(float newXMin, float newXMax, float newYMin, float newYMax) {
        // Check max limits
        if (std::isinf(newXMin)) {
            newXMin = -1e15;
        }
        if (std::isinf(newXMax)) {
            newXMax = 1e15;
        }
        if (std::isinf(newYMin)) {
            newYMin = -1e15;
        }
        if (std::isinf(newYMax)) {
            newYMax = 1e15;
        }

        // Set values
        xmin = newXMin;
        xmax = newXMax;
        ymin = newYMin;
        ymax = newYMax;
        // Update Axes Lines
        for(const auto& axesLine : axesLines) {
            axesLine.second->setMinMax(newXMin, newXMax, newYMin, newYMax);
        }
        // Update Grid
        grid->setMinMax(newXMin, newXMax, newYMin, newYMax);
        std::vector<float> xAxesPos = axesLines.at("x")->getAxesTickPos();
        std::vector<float> yAxesPos = axesLines.at("y")->getAxesTickPos();
        grid->setXLines(xAxesPos);
        grid->setYLines(yAxesPos);
        // Update Axes Area
        AxesArea::updateAxesViewportTransform();
    }

    void AxesArea::setLogScale(bool logOn, unsigned int newLogBase, LogAxes logAxes) {
        switch(logAxes) {
            case X_AXES: {
                axesLines.at("x")->setLogScale(logOn, newLogBase);
                break;
            }
            case Y_AXES: {
                axesLines.at("y")->setLogScale(logOn, newLogBase);
                break;
            }
            case BOTH: {
                axesLines.at("x")->setLogScale(logOn, newLogBase);
                axesLines.at("y")->setLogScale(logOn, newLogBase);
                break;
            }
            default: {
                std::cout << "Unknown LogAxes Enum!" << std::endl;
            }
        }

        setPlotableLogModes();
    }

    std::vector<float> AxesArea::calculateScissor(glm::mat4 axesLimitsViewportTrans) {
        // Calculate corners of axes limits area
        glm::vec4 a = axesLimitsViewportTrans * glm::vec4(xmin,ymin,0,1); // -1 to 1
        glm::vec4 b = axesLimitsViewportTrans * glm::vec4(xmax,ymax,0,1); // -1 to 1
        // Transform back to 0 to 1
        float x1 = 0.5*a[0] + 0.5;
        float y1 = 0.5*a[1] + 0.5;
        float x2 = 0.5*b[0] + 0.5;
        float y2 = 0.5*b[1] + 0.5;
        // Form vector
        std::vector<float> xyVec = {x1,y1,x2,y2};

        return xyVec;
    }

    std::shared_ptr<ILine2D> AxesArea::addLine(std::vector<float> *dataPtX, std::vector<float> *dataPtY,
            LineType lineType, glm::vec3 colour, float opacityRatio) {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Create Line
        std::shared_ptr<Plotable> lineObj;
        std::shared_ptr<ILine2D> linePt;

        switch(lineType) {
            case SINGLE_LINE: {
                lineObj = std::make_shared<Line2D2Vecs>(dataPtX, dataPtY, newParentPointers);
                linePt = std::dynamic_pointer_cast<Line2D2Vecs>(lineObj);
                break;
            }
            case SHADED_LINE: {
                lineObj = std::make_shared<ShadedLine2D2CircularVecs>(dataPtX, dataPtY, newParentPointers);
                linePt = std::dynamic_pointer_cast<ShadedLine2D2CircularVecs>(lineObj);
                break;
            }
            default: {
                lineObj = std::make_shared<ShadedLine2D2CircularVecs>(dataPtX, dataPtY, newParentPointers);
                linePt = std::dynamic_pointer_cast<ShadedLine2D2CircularVecs>(lineObj);
            }
        }
        // Set Attributes
        linePt->setLineColour(colour);
        linePt->setOpacityRatio(opacityRatio);

        // Register Children
        AxesArea::registerChild(lineObj);
        // Set axes area transform
        linePt->setAxesViewportTransform(axesViewportTransformation);
        // Store line
        linePt->setPlotableId(nextPlotableId);
        plotableMap.insert(std::pair<unsigned int, std::shared_ptr<ILine2D>>(nextPlotableId, linePt));
        nextPlotableId += 1;

        // Update log bools
        setPlotableLogModes();

        // Update limits for axes
        AxesArea::updateAxesLimits();

        return linePt;
    }

    std::shared_ptr<Plotable> AxesArea::getPlotable(int plotableId) {
        if (plotableMap.count(plotableId) > 0) {
            return plotableMap.at(plotableId);
        } else {
            std::cout << "Plotable " << plotableId << " does not exist!" << std::endl;
            return nullptr;
        }
    }

    void AxesArea::removePlotable(int plotableId) {
        if (plotableMap.count(plotableId) > 0) {
            std::shared_ptr<Plotable> line2Remove = plotableMap.at(plotableId);
            std::shared_ptr<IDrawable> drawable2Remove = std::dynamic_pointer_cast<GLPL::IDrawable>(line2Remove);
            // Remove child
            IDrawable::removeChild(drawable2Remove);
            // Remove axes
            plotableMap.erase(plotableId);
        } else {
            std::cout << "Cannot remove Plotable " << plotableId << ", Plotable does not exist!" << std::endl;
        }
    }

    std::shared_ptr<IScatterPlot> AxesArea::addScatterPlot(std::vector<float> *dataPtX, std::vector<float> *dataPtY,
                                                           glm::vec3 colour, float opacityRatio, MarkerType markerType) {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Create Scatter Plot
        std::shared_ptr<Plotable> scatterObj;
        std::shared_ptr<IScatterPlot> scatterPt;

        scatterObj = std::make_shared<Scatter2D2Vecs>(dataPtX, dataPtY, newParentPointers);
        scatterPt = std::dynamic_pointer_cast<Scatter2D2Vecs>(scatterObj);

        // Set Attributes
        scatterPt->setMarkerColour(colour);
        scatterPt->setMarkerOutlineColour(colour);
        scatterPt->setOpacityRatio(opacityRatio);
        scatterPt->setOutlineOpacityRatio(1.25f*opacityRatio);
        scatterPt->setMarkerType(markerType);

        // Register Children
        AxesArea::registerChild(scatterObj);
        // Set axes area transform
        scatterPt->setAxesViewportTransform(axesViewportTransformation);
        // Store line
        scatterPt->setPlotableId(nextPlotableId);
        plotableMap.insert(std::pair<unsigned int, std::shared_ptr<IScatterPlot>>(nextPlotableId, scatterPt));
        nextPlotableId += 1;

        // Update log bools
        setPlotableLogModes();

        // Update limits for axes
        AxesArea::updateAxesLimits();

        return scatterPt;
    }

    std::shared_ptr<TextString> AxesArea::addText(std::string textString, std::string stringId, float x, float y, float fontSize, AttachLocation attachLocation) {
        if (textStringMap.count(stringId) == 0) {
            // Create Parent Dimensions
            std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
            // Register Child
            std::shared_ptr<IDrawable> textStringObj = std::make_shared<TextString>(textString, x, y, fontSize,
                                                                                    newParentPointers);
            std::shared_ptr<TextString> textStringPt = std::dynamic_pointer_cast<TextString>(textStringObj);
            // Set pin position
            textStringPt->setAttachLocation(attachLocation);
            // Register Child
            AxesArea::registerChild(textStringObj);
            // Store Text String
            textStringMap.insert(std::pair<std::string, std::shared_ptr<TextString>>(stringId, textStringPt));

            return textStringPt;
        } else {
            std::cout << "String " << stringId << "already exists!" << std::endl;
        }

        return nullptr;
    }

    std::shared_ptr<TextString> AxesArea::getText(std::string textStringId) {
        if (textStringMap.count(textStringId) > 0) {
            return textStringMap.at(textStringId);
        } else {
            std::cout << "TextString " << textStringId << " does not exist!" << std::endl;
            return nullptr;
        }
    }

    void AxesArea::setText(std::string stringId, std::string newTextString) {
        if (textStringMap.count(stringId) > 0) {
            textStringMap.at(stringId)->setTextString(newTextString);
        } else {
            std::cout << "TextString " << stringId << " does not exist!" << std::endl;
        }
    }

    void AxesArea::setTextRotation(std::string stringId, TextRotation newTextRotation) {
        if (textStringMap.count(stringId) > 0) {
            textStringMap.at(stringId)->setTextRotation(newTextRotation);
        } else {
            std::cout << "TextString " << stringId << " does not exist!" << std::endl;
        }
    }

    float AxesArea::convertMouseX2AxesX(float mouseXVal) {
        // Calculate mouse position in x axes
        // x interpolation
        float mouseXAx = 0;
        float xminVal = 2*getLeft() - 1;
        float xmaxVal = 2*getRight() - 1;
        float sx1 = (parentTransform * glm::vec4(xminVal, 0.0f, 0.5f, 1.0f))[0];;
        float sx2 = (parentTransform * glm::vec4(xmaxVal, 0.0f, 0.5f, 1.0f))[0];
        if (!axesLines.at("x")->getLogState()) {
            float ax1 = xmin;
            float ax2 = xmax;
            float mx = (ax2 - ax1) / (sx2 - sx1);
            float cx = ax2 - (mx * sx2);
            mouseXAx = (mx * (float) mouseXVal) + cx;
        } else {
            // Log Scale
            // TODO - make work for any log base, not just 10
            float ax1 = std::log10(xmin);
            float ax2 = std::log10(xmax);
            float mx = (ax2 - ax1) / (sx2 - sx1);
            float cx = ax2 - (mx * sx2);
            unsigned int logBase = axesLines.at("x")->getLogBase();
            mouseXAx = std::pow(logBase, (mx * (float) mouseXVal) + cx);
        }
        return mouseXAx;
    }

    float AxesArea::convertMouseY2AxesY(float mouseYVal) {
        // Calculate mouse position in y axes
        // y interpolation
        float mouseYAx = 0;
        float yminVal = 2*getBottom() - 1;
        float ymaxVal = 2*getTop() - 1;
        float sx1 = (parentTransform * glm::vec4(0.0f, yminVal, 0.5f, 1.0f))[1];
        float sx2 = (parentTransform * glm::vec4(0.0f, ymaxVal, 0.5f, 1.0f))[1];
        if (!axesLines.at("y")->getLogState()) {
            float ax1 = ymin;
            float ax2 = ymax;
            float mx = (ax2 - ax1) / (sx2 - sx1);
            float cx = ax2 - (mx * sx2);
            mouseYAx = (mx * (float) mouseYVal) + cx;
        } else {
            // Log Scale
            // TODO - make work for any log base, not just 10
            float ax1 = std::log10(ymin);
            float ax2 = std::log10(ymax);
            float mx = (ax2 - ax1) / (sx2 - sx1);
            float cx = ax2 - (mx * sx2);
            unsigned int logBase = axesLines.at("y")->getLogBase();
            mouseYAx = std::pow(logBase, (mx * (float) mouseYVal) + cx);
        }

        return mouseYAx;
    }

    float AxesArea::convertMouseX2RelativeX() {
        // Calculate mouse position in relative x
        // x interpolation
        float rx1 = getLeft();
        float rx2 = getRight();
        float ax1 = (overallTransform * glm::vec4(getLeft(), 0.0f, 0.5f, 1.0f))[0];
        float ax2 = (overallTransform * glm::vec4(getRight(), 0.0f, 0.5f, 1.0f))[0];
        float mx = (rx2 - rx1) / (ax2 - ax1);
        float cx = rx2 - (mx*ax2);
        float mouseRelX = (mx*(float)mouseX) + cx;
        mouseRelX = (mouseRelX + 1) / 2.0f;

        return mouseRelX;
    }

    float AxesArea::convertMouseY2RelativeY() {
        // Calculate mouse position in relative y
        // y interpolation
        float ry1 = getLeft();
        float ry2 = getRight();
        float ay1 = (overallTransform * glm::vec4(0.0f,getBottom(), 0.5f, 1.0f))[1];
        float ay2 = (overallTransform * glm::vec4(0.0f,getTop(), 0.5f, 1.0f))[1];
        float mx = (ry2 - ry1) / (ay2 - ay1);
        float cx = ry2 - (mx*ay2);
        float mouseRelY = (mx*(float)mouseY) + cx;
        mouseRelY = (mouseRelY + 1) / 2.0f;

        return mouseRelY;
    }

    int AxesArea::getHoverCursor() {
        if (buttonMap["Interactor"]->isActive()) {
            return GLFW_CROSSHAIR_CURSOR;
        } else {
            return 0;
        }
    }

    void AxesArea::removeTextString(std::string textStringId) {
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

    void AxesArea::addButton(const std::string& buttonName, float x, float y, float width, float height,
                             AttachLocation attachLocation, bool activeState) {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Register Child
        std::shared_ptr<IDrawable> buttonObj = std::make_shared<PressButton>(buttonName, x, y, width, height, newParentPointers);
        std::shared_ptr<PressButton> buttonObjPt = std::dynamic_pointer_cast<PressButton>(buttonObj);
        // Set pin position
        buttonObjPt->setAttachLocation(attachLocation);
        // Set state
        buttonObjPt->setActive(activeState);
        // Register Child
        AxesArea::registerChild(buttonObj);
        // Store button
        buttonMap.insert(std::pair<std::string, std::shared_ptr<PressButton>>(buttonName, buttonObjPt));
    }

    std::shared_ptr<PressButton> AxesArea::addButtonWithTexture(const std::string& buttonName, const std::string& textureName, float x, float y, float width, float height,
                             AttachLocation attachLocation, bool activeState, const std::string& tooltipText) {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Register Child
        std::shared_ptr<IDrawable> buttonObj = std::make_shared<PressButtonWithImage>(buttonName, x, y, width, height, textureName, newParentPointers, tooltipText);
        std::shared_ptr<PressButton> buttonObjPt = std::dynamic_pointer_cast<PressButton>(buttonObj);
        // Set pin position
        buttonObjPt->setAttachLocation(attachLocation);
        // Set state
        buttonObjPt->setActive(activeState);
        // Register Child
        AxesArea::registerChild(buttonObj);
        // Store button
        buttonMap.insert(std::pair<std::string, std::shared_ptr<PressButton>>(buttonName, buttonObjPt));

        return buttonObjPt;
    }

    void AxesArea::setButtonState(const std::string& buttonName, bool activeState) {
        if (buttonMap.count(buttonName) > 0) {
            buttonMap.at(buttonName).get()->setActive(activeState);
        } else {
            std::cout << "PressButton " << buttonName << " does not exist!" << std::endl;
        }
    }

    void AxesArea::updateAxesViewportTransform() {
        // Update Transform
        glm::mat4 viewportTransform = GLPL::Transforms::viewportTransform(x, y, width, height);
        glm::mat4 axesLimitsTransform = AxesArea::scale2AxesLimits();
        axesViewportTransformation = std::make_shared<glm::mat4>(parentTransform * viewportTransform * axesLimitsTransform);
        // Update Children Lines, Scatter Plots
        for(auto & i : plotableMap) {
            i.second->setAxesViewportTransform(axesViewportTransformation);
        }

        // Update Grid
        if (grid != nullptr) {
            grid->setAxesViewportTransform(axesViewportTransformation);
        }
    }

    void AxesArea::setPosition(float newX, float newY) {
        // Set position
        this->x = newX;
        this->y = newY;
        // Set position in pixels
        IDrawable::updatePositionPx();
        // Update Transforms
        IDrawable::updateTransforms();
        AxesArea::updateAxesViewportTransform();
        // Update Children
        IDrawable::updateChildren();
    }

    void AxesArea::setSize(float newWidth, float newHeight) {
        // New width and height relative to their parent
        // Update size
        this->width = newWidth;
        this->height = newHeight;
        // Update in pixels
        updateSizePx();
        // Update Transforms
        IDrawable::updateTransforms();
        AxesArea::updateAxesViewportTransform();
        // Update Children
        this->updateChildren();
    }

    void GLPL::AxesArea::updateSizePx() {
        this->widthPx = (int) (this->width * (float)parentWidthPx);
        this->heightPx = (int) (this->height * (float)parentHeightPx);
    }

    void GLPL::AxesArea::setParentDimensions(glm::mat4 newParentTransform,
                                                          int newParentXPx,
                                                          int newParentYPx,
                                                          int newParentWidthPx,
                                                          int newParentHeightPx) {
        this->parentXPx = newParentXPx;
        this->parentYPx = newParentYPx;
        this->parentWidthPx = newParentWidthPx;
        this->parentHeightPx = newParentHeightPx;
        this->parentTransform = newParentTransform;
        updatePositionPx();
        updateSizePx();
        updateTransforms();
        AxesArea::updateAxesViewportTransform();
        // Update Children
        updateChildren();
    }

    void GLPL::AxesArea::setParentDimensions(std::shared_ptr<ParentDimensions> parentDimensions) {
        this->parentTransform = parentDimensions->parentTransform;
        this->parentXPx = parentDimensions->parentXPx;
        this->parentYPx = parentDimensions->parentYPx;
        this->parentWidthPx = parentDimensions->parentWidthPx;
        this->parentHeightPx = parentDimensions->parentHeightPx;
        this->shaderSetPt = parentDimensions->shaderSet;
        updatePositionPx();
        updateSizePx();
        updateTransforms();
        AxesArea::updateAxesViewportTransform();
        // Update Children
        updateChildren();
    }

    glm::mat4 AxesArea::scale2AxesLimits() {
        // Creates a transformation matrix to scale points to the axes limits
        // Check log states
        bool logX = (axesLines.find("x") == axesLines.end()) ? false : axesLines.at("x")->getLogState();
        bool logY = (axesLines.find("y") == axesLines.end()) ? false : axesLines.at("y")->getLogState();

        // Calculate x transform
        float xShift, scaleX;
        if (logX) {
            // Calculate center of current limits
            xShift = ((std::log10(xmin) + std::log10(xmax)) / 2.0f) / (std::log10(xmax) - std::log10(xmin)) * 2.0f; // xavg/width * 2.0, *2 to take it to -1 to 1
            // Scale to limits
            scaleX = 2.0f/(std::log10(xmax)-std::log10(xmin)); // Inverted due to -1 to 1 mapping (less than abs(1) region)
        } else {
            // Calculate center of current limits
            xShift = (float)((xmin + xmax) / 2.0) / (xmax - xmin) * 2.0f; // xavg/width * 2.0, *2 to take it to -1 to 1
            // Scale to limits
            scaleX = 2.0f/(xmax-xmin); // Inverted due to -1 to 1 mapping (less than abs(1) region)
        }

        // Calculate y transform
        float yShift, scaleY;
        if (logY) {
            // Calculate center of current limits
            yShift = ((std::log10(ymin)+std::log10(ymax))/2.0f)/(std::log10(ymax)-std::log10(ymin)) * 2.0f; // yavg/height * 2.0, *2 to take it to -1 to 1
            // Scale to limits
            scaleY = 2.0f/(std::log10(ymax)-std::log10(ymin)); // Inverted due to -1 to 1 mapping (less than abs(1) region)
        } else {
            // Calculate center of current limits
            yShift = ((ymin+ymax)/2.0f)/(ymax-ymin) * 2.0f; // yavg/height * 2.0, *2 to take it to -1 to 1
            // Scale to limits
            scaleY = 2.0f/(ymax-ymin); // Inverted due to -1 to 1 mapping (less than abs(1) region)
        }

        // Translate by offset and scale
        glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(-xShift, -yShift,0));
        glm::mat4 scale = glm::scale(trans, glm::vec3(scaleX,scaleY,1));

        return scale;
    }

    void GLPL::AxesArea::drawAxesBox() {
        // Draw bounding box
        std::shared_ptr<Shader> shader = shaderSetPt->getPlot2dShader();
        shader->Use();
        glUniformMatrix4fv(glGetUniformLocation(shader->Program,"transformViewport"), 1, GL_FALSE, glm::value_ptr(overallTransform));
        glUniform4fv(glGetUniformLocation(shader->Program,"inColor"),1,glm::value_ptr(axesBoxColor));
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_LOOP,0,4);
        glBindVertexArray(0);
    }

    void GLPL::AxesArea::drawGrid() {
        if (buttonMap["Grid"]->isActive()) {
            grid->Draw();
        }
    }

    void GLPL::AxesArea::updateInteractor() {
        if (buttonMap["Interactor"]->isActive()) {
            if (isHovered() && isMouseOver(mouseX, mouseY, false)) {
                // Calculate mouse position in x
                float mouseXAx = convertMouseX2AxesX((float)mouseX);
                float mouseYAx = convertMouseY2AxesY((float)mouseY);
                float mouseRelX = convertMouseX2RelativeX();
                float mouseRelY = convertMouseY2RelativeY();

                for (auto &i : plotableMap) {
                    if (i.second->isSelected()) {
                        std::tuple<float, float> pt = i.second->getClosestPoint(mouseXAx, xmin, xmax, ymin, ymax);
                        if (pt != std::make_tuple(0.0, 0.0)) {
                            float x1 = std::get<0>(pt);
                            float y1 = std::get<1>(pt);
                            float x2 = mouseXAx;
                            float y2 = mouseYAx;
                            // Update line
                            interactorLine->clearData();
                            interactorLine->dataPtX->push_back(x1);
                            interactorLine->dataPtX->push_back(x2);
                            interactorLine->dataPtY->push_back(y1);
                            interactorLine->dataPtY->push_back(y2);
                            interactorLine->updateInternalData();
                            // Update Text String
                            char textBuf[50];
                            const char *formatStr;
                            if (abs(x) > 1000 || abs(y) > 1000) {
                                formatStr = "(%.2e, %.2e)";
                            } else {
                                formatStr = "(%.2f, %.2f)";
                            }
                            sprintf(textBuf, formatStr, x, y1);
                            interactorText->setTextString(textBuf);
                            // Check if the text string should be above or below
                            if (y1 > y2) {
                                // Require offset to account for the height of the cursor
                                float mouseRelYOffset = convertVerticalPx2ObjRel(mouseHeightPx);
                                interactorText->setPosition((float) mouseRelX, (float) mouseRelY - mouseRelYOffset);
                                interactorText->setAttachLocation(CENTRE_TOP);
                            } else {
                                interactorText->setPosition((float) mouseRelX, (float) mouseRelY);
                                interactorText->setAttachLocation(CENTRE_BOTTOM);
                            }
                        }
                    }
                }
            } else {
                interactorLine->clearData();
                interactorText->setTextString("");
            }
        } else {
            interactorLine->clearData();
            interactorText->setTextString("");
        }
    }

    void GLPL::AxesArea::updateZoomDragBox() {
        if (leftShiftMouseHeld || middleMouseHeld) {
            // Calculate mouse position in x
            float dragXAx = convertMouseX2AxesX((float)mouseHeldX);
            float dragYAx = convertMouseY2AxesY((float)mouseHeldY);
            float mouseXAx = convertMouseX2AxesX((float)mouseX);
            float mouseYAx = convertMouseY2AxesY((float)mouseY);
            // Clip mouse to axes limits
            mouseXAx = clip(mouseXAx, xmin, xmax);
            mouseYAx = clip(mouseYAx, ymin, ymax);


            // Update line
            zoomBoxLine->clearData();
            // Pt 1 - (dragX, dragY)
            zoomBoxLine->dataPtX->push_back(dragXAx);
            zoomBoxLine->dataPtY->push_back(dragYAx);
            // Pt 2 - (mouseX, dragY)
            zoomBoxLine->dataPtX->push_back(mouseXAx);
            zoomBoxLine->dataPtY->push_back(dragYAx);
            // Pt 3 - (mouseX, mouseY)
            zoomBoxLine->dataPtX->push_back(mouseXAx);
            zoomBoxLine->dataPtY->push_back(mouseYAx);
            // Pt 4 - (dragX, mouseY)
            zoomBoxLine->dataPtX->push_back(dragXAx);
            zoomBoxLine->dataPtY->push_back(mouseYAx);
            // Pt 1 - (dragX, dragY)
            zoomBoxLine->dataPtX->push_back(dragXAx);
            zoomBoxLine->dataPtY->push_back(dragYAx);

            zoomBoxLine->updateInternalData();
        } else {
            zoomBoxLine->clearData();
            zoomBoxLine->dataPtX->push_back(0.0f);
            zoomBoxLine->dataPtY->push_back(0.0f);
        }
    }

    void GLPL::AxesArea::updateAxesLimits() {
        if (buttonMap["Axes Limits Scaling"]->isActive()) {
            // Get the overall maximum and minimum from all lines
            float newXmin = -0.0;
            float newXmax = 0.0;
            float newYmin = -0.0;
            float newYmax = 0.0;
            if (axesLines.at("x")->getLogState()) {
                newXmin = 1.0;
                newXmax = 1.0;
            }
            if (axesLines.at("y")->getLogState()) {
                newYmin = 1.0;
                newYmax = 1.0;
            }
            for (std::pair<unsigned int, std::shared_ptr<Plotable>> lineInfo : plotableMap) {
                std::vector<float> minMax = lineInfo.second->getMinMax(true);
                if (minMax.size() == 4) {
                    if (minMax[0] < newXmin) { newXmin = minMax[0]; };
                    if (minMax[1] > newXmax) { newXmax = minMax[1]; };
                    if (minMax[2] < newYmin) { newYmin = minMax[2]; };
                    if (minMax[3] > newYmax) { newYmax = minMax[3]; };
                }
            }

            // Set axes limits
            float absLim = 1e-10;
            if ((abs(xmin - newXmin) > absLim) ||
                (abs(xmax - newXmax) > absLim) ||
                (abs(ymin - newYmin) > absLim) ||
                (abs(ymax - newYmax) > absLim)) {
                AxesArea::setAxesLimits(newXmin, newXmax, newYmin, newYmax);
            }

        }

        // Match axes lines sizing
        float xFontSize = axesLines.at("x")->getFontSize();
        float yFontSize = axesLines.at("y")->getFontSize();
        float minFontSize = std::min(xFontSize, yFontSize);
        if (xFontSize < yFontSize) {
            axesLines.at("y")->setMajorTickFontSize(minFontSize);
        } else if (yFontSize < xFontSize) {
            axesLines.at("x")->setMajorTickFontSize(minFontSize);
        }
    }

    std::pair<float, float>
    GLPL::AxesArea::calcScrolledVals(float minVal, float maxVal, float currVal, float zoomFrac, bool dir) {
        // a, b are before scroll
        // c, d are after scroll
        // a + b = Ro
        // c + d = Rn
        // Ro = xmax - xmin
        // Rn = (1+r)*Ro
        // a/(a+b) = c/(c+d) => c = a*Rn/(a+b)
        // d = Rn - c
        float oldRange = maxVal - minVal;
        float a = currVal - minVal;
        float b = maxVal - currVal;

        float newRange;
        if (dir) {
            newRange = (1 + zoomFrac) * oldRange;
        } else {
            newRange = (1 - zoomFrac) * oldRange;
        }
        // Cap max range
        float newMin, newMax;
        if (abs(a) < 1e15) {
            // Calculate new limits
            float c = a * newRange / (a + b);
            float d = newRange - c;
            newMin = currVal - c;
            newMax = currVal + d;
        } else {
            newMin = minVal;
            newMax = maxVal;
        }

        return std::pair<float, float>(newMin, newMax);
    }

    void GLPL::AxesArea::zoomAxes(float zoomDir) {
        // Zoom centered around the current mouse position
        float mouseXAx = convertMouseX2AxesX(mouseX);
        float mouseYAx = convertMouseY2AxesY(mouseY);

        std::pair<float, float> newX, newY;
        // Account for log values
        float inXmin = xmin;
        float inXmax = xmax;
        float inYmin = ymin;
        float inYmax = ymax;
        if (axesLines.at("x")->getLogState()) {
            inXmin = std::log10(xmin);
            inXmax = std::log10(xmax);
            mouseXAx = std::log10(mouseXAx);
        }
        if (axesLines.at("y")->getLogState()) {
            inYmin = std::log10(ymin);
            inYmax = std::log10(ymax);
            mouseYAx = std::log10(mouseYAx);
        }

        if (zoomDir > 0) {
            newX = calcScrolledVals(inXmin, inXmax, mouseXAx, zoomRatio, true);
            newY = calcScrolledVals(inYmin, inYmax, mouseYAx, zoomRatio, true);
        } else {
            newX = calcScrolledVals(inXmin, inXmax, mouseXAx, zoomRatio, false);
            newY = calcScrolledVals(inYmin, inYmax, mouseYAx, zoomRatio, false);
        }

        // Undo log if required
        if (axesLines.at("x")->getLogState()) {
            unsigned int logBase = axesLines.at("x")->getLogBase();
            newX.first = std::pow(logBase, newX.first);
            newX.second = std::pow(logBase, newX.second);
        }
        if (axesLines.at("y")->getLogState()) {
            unsigned int logBase = axesLines.at("y")->getLogBase();
            newY.first = std::pow(logBase, newY.first);
            newY.second = std::pow(logBase, newY.second);
        }

        AxesArea::setAxesLimits(newX.first, newX.second, newY.first, newY.second);
    }

    void GLPL::AxesArea::zoomAxesByDragging() {
        // Zoom centered around the current mouse position by dragging the mouse
        // The change in zoom is proportional to the drag distance in x and y
        float xDiff = mouseX - mouseHeldX;
        float yDiff = mouseY - mouseHeldY;
        float xFrac = abs(dragZoomFactor * xDiff / 2.0f);
        float yFrac = abs(dragZoomFactor * yDiff / 2.0f);

        float mouseXAx = convertMouseX2AxesX(mouseX);
        float mouseYAx = convertMouseY2AxesY(mouseY);

        // Account for log values if required
        float inXmin = xMinDrag;
        float inXmax = xMaxDrag;
        float inYmin = yMinDrag;
        float inYmax = yMaxDrag;
        if (axesLines.at("x")->getLogState()) {
            inXmin = std::log10(xMinDrag);
            inXmax = std::log10(xMaxDrag);
            mouseXAx = std::log10(mouseXAx);
        }
        if (axesLines.at("y")->getLogState()) {
            inYmin = std::log10(yMinDrag);
            inYmax = std::log10(yMaxDrag);
            mouseYAx = std::log10(mouseYAx);
        }

        std::pair<float, float> newX, newY;
        newX = calcScrolledVals(inXmin, inXmax, mouseXAx, xFrac, xDiff < 0);
        newY = calcScrolledVals(inYmin, inYmax, mouseYAx, yFrac, yDiff < 0);

        // Undo log if required
        if (axesLines.at("x")->getLogState()) {
            unsigned int logBase = axesLines.at("x")->getLogBase();
            newX.first = std::pow(logBase, newX.first);
            newX.second = std::pow(logBase, newX.second);
        }
        if (axesLines.at("y")->getLogState()) {
            unsigned int logBase = axesLines.at("y")->getLogBase();
            newY.first = std::pow(logBase, newY.first);
            newY.second = std::pow(logBase, newY.second);
        }

        AxesArea::setAxesLimits(newX.first, newX.second, newY.first, newY.second);
    }

    void GLPL::AxesArea::moveAxesByDragging() {
        // Move the axes proportional to the distance the mouse is dragged
        float mouseXAx = convertMouseX2AxesX(mouseX);
        float mouseYAx = convertMouseY2AxesY(mouseY);
        float mouseHeldXAx = convertMouseX2AxesX(mouseHeldX);
        float mouseHeldYAx = convertMouseY2AxesY(mouseHeldY);
        float xDiff = mouseXAx - mouseHeldXAx;
        float yDiff = mouseYAx - mouseHeldYAx;
        float xRange = xMaxDrag - xMinDrag;
        float yRange = yMaxDrag - yMinDrag;
        float xRatio = xDiff / xRange;
        float yRatio = yDiff / yRange;

        float newXMin = xMinDrag + (xRatio * xRange);
        float newXMax = xMaxDrag + (xRatio * xRange);
        float newYMin = yMinDrag + (yRatio * yRange);
        float newYMax = yMaxDrag + (yRatio * yRange);


        AxesArea::setAxesLimits(newXMin, newXMax, newYMin, newYMax);
    }

    void GLPL::AxesArea::createInteractor() {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();

        // Create Line
        interactorLine = std::make_shared<Line2D2Vecs>(&interactorDataX, &interactorDataY, newParentPointers);
        // Register Children
        AxesArea::registerChild(interactorLine);
        // Set axes area transform
        interactorLine->setAxesViewportTransform(axesViewportTransformation);
        // Set not hoverable
        interactorLine->setHoverable(false);
        // Store line
        interactorLine->setPlotableId(nextPlotableId);
        plotableMap.insert(std::pair<unsigned int, std::shared_ptr<ILine2D>>(nextPlotableId, interactorLine));
        nextPlotableId += 1;

        // Create text label
        // Create label
        interactorText = std::make_shared<TextString>("", x, y, 8, newParentPointers);
        interactorText->setAttachLocation(CENTRE_BOTTOM);
        interactorText->setHoverable(false);
        // Register Child
        AxesArea::registerChild(interactorText);
        // Store Text String
        textStringMap.insert(std::pair<std::string, std::shared_ptr<TextString>>("interactor-text", interactorText));

    }

    void GLPL::AxesArea::createGrid() {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Create grid
        grid = std::make_shared<Grid>(newParentPointers);
        // Register Children
        AxesArea::registerChild(grid);
        // Set axes area transform
        grid->setAxesViewportTransform(axesViewportTransformation);
        // Set not hoverable
        grid->setHoverable(false);
    }

    void GLPL::AxesArea::createZoomBox() {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Create Line
        zoomBoxLine = std::make_shared<Line2D2Vecs>(&zoomBoxX, &zoomBoxY, newParentPointers);
        // Set Properties
        zoomBoxLine->setLineColour(glm::vec3(0.0f, 1.0f, 0.0f));
        zoomBoxLine->setLineWidth(2);
        // Register Children
        AxesArea::registerChild(zoomBoxLine);
        // Set axes area transform
        zoomBoxLine->setAxesViewportTransform(axesViewportTransformation);
        // Set not hoverable
        zoomBoxLine->setHoverable(false);
        // Store line
        zoomBoxLine->setPlotableId(nextPlotableId);
        plotableMap.insert(std::pair<unsigned int, std::shared_ptr<ILine2D>>(nextPlotableId, zoomBoxLine));
        nextPlotableId += 1;

    }

    void AxesArea::setPlotableLogModes() {
        // Set the current log modes for all the lines
        bool logX = axesLines.at("x")->getLogState();
        bool logY = axesLines.at("y")->getLogState();
        for(auto & i : plotableMap) {
            i.second->setLogModes(logX, logY);
        }
        grid->setLogModes(logX, logY);
    }

    void AxesArea::showTitle(bool show) {
        auto title = getText("axes-title");

        if (show && !title->getVisible())
        {
            title->setVisible(true);
        }
        else if (!show && title->getVisible())
        {
            title->setVisible(false);
        }
    }
}