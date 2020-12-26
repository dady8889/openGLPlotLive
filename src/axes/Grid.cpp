//
// Created by bcub3d-desktop on 4/7/20.
//

#include "Grid.h"



namespace GLPL {

    Grid::Grid(std::shared_ptr<ParentDimensions> parentDimensions) :
            ConstantXYDrawable(0.0f, 0.0f, 1.0f, 1.0f, CONSTANT_SCALE, CONSTANT_SCALE, std::move(parentDimensions)) {
        // Set Bounding Box Color
        boundingBoxColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        // Set Not Hoverable
        setHoverable(false);

        // Setup Buffers
        Grid::createAndSetupAxesLineBuffers();

    }

    void Grid::setMinMax(float newXMin, float newXMax, float newYMin, float newYMax) {
        if ((newXMin != newXMax) && (newYMin != newYMax)) {
            xMin = newXMin;
            xMax = newXMax;
            yMin = newYMin;
            yMax = newYMax;
            // Regenerate axes lines
            genLines();
        }
    }

    void Grid::setXLines(std::vector<float> xVals) {
        xAxesPos = std::move(xVals);
        Grid::genLines();
    }

    void Grid::setYLines(std::vector<float> yVals) {
        yAxesPos = std::move(yVals);
        Grid::genLines();
    }

    void Grid::genLines() {
        verts.clear();
        for(float xAxesPo : xAxesPos) {
            // Store relative position
            verts.push_back(xAxesPo);    // x1
            verts.push_back(yMin);       // y1
            verts.push_back(xAxesPo);    // x2
            verts.push_back(yMax);       // y2
        }
        for(float yAxesPo : yAxesPos) {
            // Store relative position
            verts.push_back(xMin);       // x1
            verts.push_back(yAxesPo);    // y1
            verts.push_back(xMax);       // x2
            verts.push_back(yAxesPo);    // y2
        }
        updateAxesLineBuffers();
    }



    void Grid::createAndSetupAxesLineBuffers() {
        // Major Tick Buffers
        // Create Buffers
        glGenVertexArrays(1,&vertsVAO);
        glGenBuffers(1,&vertsVBO);

        // Setup Buffers
        glBindVertexArray(vertsVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vertsVBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW);

        // Position Attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

        glBindVertexArray(0);

    }

    void Grid::updateAxesLineBuffers() {
        // Major Ticks
        glBindBuffer(GL_ARRAY_BUFFER, vertsVBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), &verts[0], GL_STATIC_DRAW);
    }

    void Grid::Draw() {
        // Draw Major Ticks
        std::shared_ptr<Shader> shader = shaderSetPt->getPlot2dShader();
        shader->Use();
        glUniformMatrix4fv(glGetUniformLocation(shader->Program, "transformViewport"), 1, GL_FALSE,
                           glm::value_ptr(*axesViewportTransform));
        glUniform4fv(glGetUniformLocation(shader->Program, "inColor"), 1, glm::value_ptr(axesLineColor));
        glBindVertexArray(vertsVAO);
        glDrawArrays(GL_LINES, 0, (int)verts.size() / 2.0);
        glBindVertexArray(0);
    }

    std::vector<float> Grid::getMinMax() {
        return std::vector<float> {0.0,0.0,0.0,0.0};
    }

    std::string Grid::getID() {
        return "Grid:" + std::to_string(x) + ":" + std::to_string(y);
    }

    std::tuple<float, float> Grid::getClosestPoint(float xVal) {
        return std::make_tuple(0.0, 0.0);
    }


}

