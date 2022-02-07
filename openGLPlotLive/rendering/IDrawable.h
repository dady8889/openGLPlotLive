//
// Created by tbatt on 19/04/2020.
//

#ifndef OPENGLPLOTLIVE_PROJ_IDRAWABLE_H
#define OPENGLPLOTLIVE_PROJ_IDRAWABLE_H

// Standard Includes
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <array>

// GLAD - Multi Language GL Loader-Generator
#include <glad/glad.h>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>
#include <set>

// Project Includes
#include "transforms.h"
#include "ShaderSet.h"
#include "../interaction/IClickable.h"


namespace GLPL {

    struct ParentDimensions {
        glm::mat4 parentTransform;
        int parentXPx;
        int parentYPx;
        int parentWidthPx;
        int parentHeightPx;
        std::shared_ptr<ShaderSet> shaderSet;
    };

    enum AttachLocation {
        BOTTOM_LEFT,
        BOTTOM_RIGHT,
        TOP_LEFT,
        TOP_RIGHT,
        CENTRE_TOP,
        CENTRE_BOTTOM,
        CENTRE_LEFT,
        CENTRE_RIGHT,
        CENTRE
    };

    class IDrawable : public IClickable {
        // Anything that is draw must implement the following
    public:
        // Constructor
        IDrawable();

        // Functions
        std::array<float, 2> getPosition();
        float getWidth();
        float getHeight();
        int getWidthPx();
        int getHeightPx();
        float getLeft();
        float getTop();
        float getRight();
        float getBottom();
        virtual void onLeftClick();
        virtual void onLeftShiftClick();
        virtual void onMiddleClick();
        virtual void onRightClick();
        virtual void onRightShiftClick();
        virtual void onScroll(double xoffset, double yoffset);
        virtual void onRightDrag(bool dragging, double origXPos, double origYPos);
        virtual void onMiddleDrag(bool dragging, double origXPos, double origYPos);
        virtual void onLeftDrag(bool dragging, double origXPos, double origYPos);
        virtual void onRightShiftDrag(bool dragging, double origXPos, double origYPos);
        virtual void onLeftShiftDrag(bool dragging, double origXPos, double origYPos);
        // Dimension Functions
        virtual void setPosition(float newX, float newY) = 0;
        virtual void setSize(float newWidth, float newHeight) = 0;
        virtual void updateSizePx() = 0;
        // Parent Dimension Functions
        virtual void setParentDimensions(glm::mat4 newParentTransform,
                                         int newParentXPx,
                                         int newParentYPx,
                                         int newParentWidthPx,
                                         int newParentHeightPx) = 0;
        virtual void setParentDimensions(std::shared_ptr<ParentDimensions> parentDimensions) = 0;
        std::shared_ptr<ParentDimensions> createParentDimensions();
        // Conversions
        float convertHorizontalPx2ObjRel(float horSizePx);
        float convertVerticalPx2ObjRel(float vertSizePx);
        // Identifier
        virtual std::string getID() = 0;
        // Mouse Interaction
        bool canMouseOver();
        bool isHoverable();
        void setHoverable(bool isHoverable);
        bool isHovered();
        virtual void setHovered(bool newHovered);
        bool isSelected();
        void setSelected(bool isSelected);
        std::vector<GLfloat> calcMouseOverVertsWithChildren();
        std::vector<GLfloat> calcMouseOverVertsNoChildren();
        bool isMouseOver(double xpos, double ypos, bool withChildren = true);
        void getMousedOverChildren(double xpos, double ypos,
                                   const std::shared_ptr<std::vector<std::shared_ptr<GLPL::IDrawable>>>& mousedOverObjs);
        virtual void setLastMousePos(double lastMouseX, double lastMouseY);
        std::tuple<double, double> getLastMousePos();

        virtual int getHoverCursor();

        virtual void Draw() = 0;
        void registerChild(const std::shared_ptr<IDrawable>& newChildPt);
        void removeChild(const std::shared_ptr<IDrawable>& childPt);
        void drawBoundingBox();
        void drawMouseOverBox();
        void setZDepthValue(int newZDepthValue);
        int getZDepthValue();
        void setAttachLocation(AttachLocation newAttachLocation);
        void setVisible(bool isVisible);
        bool getVisible();

        virtual void setOffset(int left, int right, int top, int bottom);
        glm::vec4 getOffset();

    protected:
        // Dimensions
        float x = 0;			// Location of bottom left corner x position of the drawable relative to its parent in 0 to 1
        float y = 0;			// Location of bottom left corner y position of the drawable relative to its parent in 0 to 1
        int xPx = 0;            // Location of bottom left corner x position of the drawable, in pixels
        int yPx = 0;            // Location of bottom left corner y position of the drawable, in pixels
        float width = 1.0;      // Width of drawable as a proportion of the parents width
        float height = 1.0;		// Height of drawable as a proportion of the parents height
        int widthPx = 0;        // Width in pixels
        int heightPx = 0;       // Height in pixels
        // Parent Dimensions
        int parentXPx = 0;        // Parent x location in pixels
        int parentYPx = 0;        // Parent y location in pixels
        int parentWidthPx = 0;    // Parent width in pixels
        int parentHeightPx = 0;   // Parent height in pixels
        int zDepthValue = 0;      // The z buffer value
        AttachLocation attachLocation = BOTTOM_LEFT; // The location to position the object by, used to locate the object by different corners
        // Mouse Interaction
        bool mouseOverable = true;
        bool hoverable = true;
        bool hovered = false;
        bool selected = false;
        double mouseX = 0;
        double mouseY = 0;
        float mouseHeightPx = 64;

        // Children
        std::vector<std::shared_ptr<IDrawable>> children; // List of child drawables

        // Buffers
        GLuint VAO, VBO;
        GLuint mouseVAO, mouseVBO;
        // Area
        std::vector<GLfloat> boxVerts = { -1, -1,    1, -1,    1,  1,    -1, 1};
        std::vector<GLfloat> mouseOverVertsWithChildren = { -1, -1,    1, -1,    1,  1,    -1, 1};
        std::vector<GLfloat> mouseOverVertsNoChildren =  { -1, -1,    1, -1,    1,  1,    -1, 1};
        glm::vec4 boundingBoxColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        bool visible = true;
        glm::vec4 offsetsPx = glm::vec4(0, 0, 0, 0); // Margin/Padding in pixels (left, right, top, bottom)
        glm::vec4 offsetsLocal = glm::vec4(0, 0, 0, 0); // Margin/Padding in scaled units [-1, 1] (left, right, top, bottom)

        // Transforms
        glm::mat4 parentTransform;                      // Transform of the parent
        glm::mat4 viewportTransform;                    // Transform only with the current drawable bounding box
        glm::mat4 overallTransform;                     // The overall transform for drawing relative to the window
        // Shader access
        std::shared_ptr<ShaderSet> shaderSetPt = nullptr;

        // Functions
        void createAndSetupBuffers();
        void updateTransforms();
        void updatePositionPx();
        void updateChildren();
        void sortChildren();
        static bool compareZDepthValue(const std::shared_ptr<IDrawable>& left, const std::shared_ptr<IDrawable>& right);
        std::array<float, 2> generateXYPositionFromPin();
    };
}


#endif //OPENGLPLOTLIVE_PROJ_IDRAWABLE_H
