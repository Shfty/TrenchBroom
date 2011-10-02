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

#import <Cocoa/Cocoa.h>

typedef enum {
    TT_FRAC = 1 << 0, // fractional number
    TT_DEC  = 1 << 1, // decimal number
    TT_STR  = 1 << 2, // string
    TT_B_O  = 1 << 3, // opening parenthesis
    TT_B_C  = 1 << 4, // closing parenthesis
    TT_CB_O = 1 << 5, // opening curly bracket
    TT_CB_C = 1 << 6, // closing curly bracket
    TT_SB_O = 1 << 7, // opening square bracket
    TT_SB_C = 1 << 8, // closing square bracket
    TT_COM  = 1 << 9 // comment
} ETokenType;


@interface MapToken : NSObject {
    @private
    ETokenType type;
    id data;
    int line;
    int column;
    int charsRead;
}

+ (NSString *)typeName:(int)aType;

- (id)initWithToken:(MapToken *)theToken;

- (ETokenType)type;
- (id)data;

- (int)line;
- (int)column;

- (int)charsRead;

- (MapToken *)setType:(ETokenType)theType data:(id)theData line:(int)theLine column:(int)theColumn charsRead:(int)theCharsRead;

@end
