//
// Created by tbatt on 19/06/2020.
//

#ifndef OPENGLPLOTLIVE_PROJ_AXESAREA_H
#define OPENGLPLOTLIVE_PROJ_AXESAREA_H

// Project Includes
#include "../rendering/ConstantScaleDrawable.h"
#include "../lines/ILine2D_OLD.h"
#include "IPlotable.h"
#include "../shadedLines/IShadedLine2D.h"
#include "../lines/LineType.h"
#include "../texts/textString.h"


namespace GLPL {

    class AxesArea : public IDrawable {
    public:
        // Constructor
        AxesArea(float x, float y, float width, float height, std::shared_ptr<ParentDimensions> parentDimensions);

        // Functions
        void Draw();
        // Axes
        void setAxesBoxOn(bool axesBoxOnBool);
        void setAxesBoxColor(glm::vec4 newAxesBoxColour);
        // Lines
        std::shared_ptr<ILine2D> addLine(std::vector<float> *dataPtX, std::vector<float> *dataPtY,
                LineType lineType=SINGLE_LINE, glm::vec3 colour=LC_WHITE, float opacityRatio=1.0);
        std::shared_ptr<IPlotable> getLine(unsigned int lineId);
        void removeLine(unsigned int lineId);
        // Text
        void addText(const char* textString, float x, float y, float fontSize, AttachLocation attachLocation=BOTTOM_LEFT);
        std::shared_ptr<TextString> getText(unsigned int textStringId);
        void removeTextString(unsigned int textStringId);
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
        unsigned int lineCount = 0;
        std::unordered_map<unsigned int, std::shared_ptr<ILine2D>> lineMap;
        std::vector<std::shared_ptr<IDrawable>> axesItems;
        // Axes Limits
        float xmin = -1.0;
        float xmax = 1.0;
        float ymin = -1.0;
        float ymax = 1.0;
        // Axes Settings
        bool axesBoxOn = true;
        glm::vec4 axesBoxColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        // Text String
        unsigned int textStringCount = 0;
        std::unordered_map<unsigned int, std::shared_ptr<TextString>> textStringMap;

        // Functions
        std::vector<float> calculateScissor(glm::mat4 axesLimitsViewportTrans);
        glm::mat4 scale2AxesLimits();
        void drawAxesBox();

    };


}


#endif //OPENGLPLOTLIVE_PROJ_AXESAREA_H
