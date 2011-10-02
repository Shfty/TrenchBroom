/*
This file is part of TrenchBroom.

TrenchBroom is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TrenchBroom is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
*/

#import "ClipLineFeedbackFigure.h"

@implementation ClipLineFeedbackFigure

- (id)initWithStartPoint:(TVector3i *)theStartPoint endPoint:(TVector3i *)theEndPoint {
    if (self = [self init]) {
        startPoint = *theStartPoint;
        endPoint = *theEndPoint;
    }
    
    return self;
}

- (void)render {
    glColor4f(0, 1, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(startPoint.x, startPoint.y, startPoint.z);
    glVertex3f(endPoint.x, endPoint.y, endPoint.z);
    glEnd();
}

@end
