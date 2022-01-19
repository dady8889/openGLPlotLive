//
// Created by tbatt on 19/06/2020.
//

#ifndef OPENGLPLOTLIVE_PROJ_AXESAREA_H
#define OPENGLPLOTLIVE_PROJ_AXESAREA_H

// Project Includes
#include "../lines/ILine2D.h"
#include "AxesLineTicks.h"
#include "../lines/LineType.h"
#include "../interaction/PressButton.h"
#include "../lines/Line2D2Vecs.h"
#include "Grid.h"
#include "../scatterPlot/IScatterPlot.h"
#include <cmath>


namespace GLPL {

    enum LogAxes {
        X_AXES,
        Y_AXES,
        BOTH
    };

    class AxesArea : public IDrawable {
    public:
        // Constructor
        AxesArea(float x, float y, float width, float height, std::shared_ptr<ParentDimensions> parentDimensions);

        // Functions
        std::string getID();
        void Draw();
        void onScroll(double xoffset, double yoffset) override;
        void onRightDrag(bool dragging, double origXPos, double origYPos) override;
        void onMiddleDrag(bool dragging, double origXPos, double origYPos) override;
        void onLeftDrag(bool dragging, double origXPos, double origYPos) override;
        void onLeftShiftDrag(bool dragging, double origXPos, double origYPos) override;
        void setLastMousePos(double lastMouseX, double lastMouseY) override;
        // Axes
        void showTitle(bool show);
        void setAxesBoxOn(bool axesBoxOnBool);
        void setAxesBoxColor(glm::vec4 newAxesBoxColour);
        void addAxesLine(const std::string& axesName, AxesDirection axesDirection);
        void setAxesLimits(float newXMin, float newXMax, float newYMin, float newYMax);
        void setLogScale(bool logOn, unsigned int newLogBase, LogAxes logAxes);
        // Plotables
        std::shared_ptr<ILine2D> addLine(std::shared_ptr<std::span<float>> dataPtX, std::shared_ptr<std::span<float>> dataPtY,
                LineType lineType=SINGLE_LINE, glm::vec3 colour=LC_WHITE, float opacityRatio=1.0);
        std::shared_ptr<IScatterPlot> addScatterPlot(std::vector<float> *dataPtX, std::vector<float> *dataPtY,
                glm::vec3 colour = LC_WHITE, float opacityRatio=1.0, MarkerType markerType=CIRCLE);
        std::shared_ptr<Plotable> getPlotable(int plotableId);
        void removePlotable(int plotableId);
        // Text
        std::shared_ptr<TextString> addText(std::string textString, std::string stringId, float x, float y, float fontSize, AttachLocation attachLocation=BOTTOM_LEFT);
        std::shared_ptr<TextString> getText(std::string textStringId);
        void setText(std::string stringId, std::string newTextString);
        void setTextRotation(std::string stringId, TextRotation newTextRotation);
        void removeTextString(std::string textStringId);
        // PressButton
        void addButton(const std::string& buttonName, float x, float y, float width, float height, AttachLocation attachLocation, bool activeState = true);
        std::shared_ptr<PressButton> addButtonWithTexture(const std::string& buttonName, const std::string& textureName, float x, float y, float width, float height, AttachLocation attachLocation,
                bool activeState = true, const std::string& tooltipText = "");
        void setButtonState(const std::string& buttonName, bool activeState);
        // Point Interactor
        float convertMouseX2AxesX(float mouseXVal);
        float convertMouseY2AxesY(float mouseYVal);
        float convertMouseX2RelativeX();
        float convertMouseY2RelativeY();
        int getHoverCursor();
        // Other
        void updateAxesViewportTransform();
        void setPosition(float newX, float newY);
        void setSize(float newWidth, float newHeight);
        void updateSizePx();
        void setParentDimensions(glm::mat4 newParentTransform,
                                 int newParentXPx,
                                 int newParentYPx,
                                 int newParentWidthPx,
                                 int newParentHeightPx);
        void setParentDimensions(std::shared_ptr<ParentDimensions> parentDimensions);


    private:
        // Data
        std::shared_ptr<glm::mat4> axesViewportTransformation = std::make_shared<glm::mat4>(1.0f);
        int nextPlotableId = 0;
        std::unordered_map<int, std::shared_ptr<Plotable>> plotableMap;
        std::vector<std::shared_ptr<IDrawable>> axesItems;
        // Axes Limits
        float xmin = -1.0;
        float xmax = 1.0;
        float ymin = -1.0;
        float ymax = 1.0;
        // Axes Settings
        glm::vec4 axesBoxColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        // Text String
        std::unordered_map<std::string, std::shared_ptr<TextString>> textStringMap;
        // Axes Lines
        std::unordered_map<std::string, std::shared_ptr<AxesLineTicks>> axesLines;
        // Buttons
        std::unordered_map<std::string, std::shared_ptr<PressButton>> buttonMap;
        // Interactor
        std::shared_ptr<std::vector<float>> interactorDataX = std::make_shared<std::vector<float>>();
        std::shared_ptr<std::vector<float>> interactorDataY = std::make_shared<std::vector<float>>();
        std::shared_ptr<Line2D2Vecs> interactorLine;
        std::shared_ptr<TextString> interactorText;
        // Grid
        std::shared_ptr<Grid> grid;
        // Zoom Interaction
        float zoomRatio = 0.2;  // 10% on each scroll
        // Drag Zoom Box
        std::shared_ptr<std::vector<float>> zoomBoxX = std::make_shared<std::vector<float>>();
        std::shared_ptr<std::vector<float>> zoomBoxY = std::make_shared<std::vector<float>>();
        std::shared_ptr<Line2D2Vecs> zoomBoxLine;
        // Mouse
        bool rightMouseHeld = false;
        bool rightShiftMouseHeld = false;
        bool middleMouseHeld = false;
        bool leftMouseHeld = false;
        bool leftShiftMouseHeld = false;
        double mouseHeldX = 0;
        double mouseHeldY = 0;
        float dragZoomFactor = 5.0;
        float xMinDrag = 0;
        float xMaxDrag = 0;
        float yMinDrag = 0;
        float yMaxDrag = 0;


        // Functions
        std::vector<float> calculateScissor(glm::mat4 axesLimitsViewportTrans);
        glm::mat4 scale2AxesLimits();
        void drawAxesBox();
        void drawGrid();
        void updateInteractor();
        void updateZoomDragBox();
        void updateAxesLimits();
        std::pair<float, float> calcScrolledVals(float minVal, float maxVal, float currVal, float zoomFrac, bool dir);
        void zoomAxes(float zoomDir);
        void zoomAxesByDragging();
        void moveAxesByDragging();
        void createInteractor();
        void createGrid();
        void createZoomBox();
        void setPlotableLogModes();

    };


}


#endif //OPENGLPLOTLIVE_PROJ_AXESAREA_H
