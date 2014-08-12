// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

/** the rendering context used by all GL canvases */
class Brush
{
protected:
    /// Brush position (UV, depends on terrain position)
    vec2 Position;

    /// Height on which center of brush currently are
    float Height;

    /// Brush radius
    float Radius;
    
    /// brush mode. 0 - additive, 1 - subtracting, 2 - smooth
    int Mode;

public:
    /// Standard constructors
    Brush():Radius(10.0f), Mode(0) {};
    Brush(vec2 InitialBrushPosition);

    /// Setters
    void SetMode(int NewMode) {Mode = NewMode;};
    void SetPosition(vec3 NewBrushPosition);
    void ModifyRadius(float Modifier);

    /// Getters
    int GetMode() {return Mode;};
    vec2 GetPosition() {return Position;};
    vec2 GetRenderPosition() {return Position + vec2(-Radius, -Radius);};
    float GetRadius() {return Radius;};
    float GetHeight() {return Height;};
};