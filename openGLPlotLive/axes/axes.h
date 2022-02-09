//
// Created by tbatt on 19/04/2020.
//

#ifndef OPENGLPLOTLIVE_PROJ_AXES_H
#define OPENGLPLOTLIVE_PROJ_AXES_H

// Project Includes
#include "../texts/TextString.h"
#include "AxesArea.h"
#include "../rendering/ConstantXYDrawable.h"
#include "AxesLineTicks.h"
#include "../scatterPlot/IScatterPlot.h"


namespace GLPL {
    class Axes : public ConstantXYDrawable {
    public:
        // Constructor
        Axes(float x, float y, float width, float height, std::shared_ptr<ParentDimensions> parentDimensions);

        // Functions
        // Text
        void addText(const char* textString, float x, float y, float fontSize, AttachLocation attachLocation=BOTTOM_LEFT);
        std::shared_ptr<TextString> getText(unsigned int textStringId);
        void removeTextString(unsigned int textStringId);
        // Line
        std::shared_ptr<ILine2D> addLine(std::vector<float> *dataPtX, std::vector<float> *dataPtY, LineType lineType = SINGLE_LINE,
                     glm::vec3 colour = LC_WHITE, float opacityRatio=1.0);
        // Fast Line
        std::shared_ptr<ILine2D> addFastLine(glm::vec3 colour = LC_WHITE, float opacityRatio = 1.0);
        std::shared_ptr<ILine2D> addFastLine(float* dataPtX, float* dataPtY, size_t dataSize, glm::vec3 colour = LC_WHITE, float opacityRatio = 1.0);
        // Scatter Plots
        std::shared_ptr<IScatterPlot> addScatterPlot(std::vector<float> *dataPtX, std::vector<float> *dataPtY,
                                                     glm::vec3 colour = LC_WHITE, float opacityRatio=1.0, MarkerType markerType=CIRCLE);
        // Axes
        void setAxesBoxOn(bool axesBoxOnBool);
        void setLogScale(bool logOn, unsigned int newLogBase, LogAxes logAxes);
        // Labels
        void showTitle(bool show);
        void setTitle(std::string newTitle);
        void setXLabel(std::string newXLabel);
        void setYLabel(std::string newYLabel);
        void setXLabelRotation(TextRotation newTextRotation);
        void setYLabelRotation(TextRotation newTextRotation);
        // PressButton
        void setButtonState(const std::string& buttonName, bool activeState);

        void Draw();
        std::string getID();

        void setOffset(int left, int right, int top, int bottom) override;
        void resetAxes();

    private:
        // Functions
        void createAxesArea();
        void createAxesLines();

        // Data
        // Axes Area
        std::shared_ptr<AxesArea> axesArea;
        // Text String
        unsigned int textStringCount = 0;
        std::unordered_map<unsigned int, std::shared_ptr<TextString>> textStringMap;
    };
}


#endif //OPENGLPLOTLIVE_PROJ_AXES_H
