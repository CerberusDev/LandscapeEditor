// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------

#include "Brush.h"
#include "LandscapeEditor.h"

// --------------------------------------------------------------------
Brush::Brush(vec2 InitialBrushPosition):
Position(InitialBrushPosition), Radius(0.5f), Mode(0)
{
}

// --------------------------------------------------------------------
void Brush::ModifyRadius(float Modifier)
 {
     if ((Radius * Modifier) > 0.25f) 
         Radius *= Modifier;
};

// --------------------------------------------------------------------
void Brush::SetPosition(vec3 NewBrushPosition)
{
    Position = vec2(NewBrushPosition.x, NewBrushPosition.z);
    Height = NewBrushPosition.y;
}

