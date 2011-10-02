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

#import "MapTokenizer.h"
#import "MapToken.h"

@implementation MapTokenizer

- (id)initWithInputStream:(NSInputStream *)aStream {
    if (self = [self init]) {
        stream = [aStream retain];
        [stream open];
        bufferIndex = 0;
        bufferSize = [stream read:buffer maxLength:1024];
        pushBuffer = [[NSMutableString alloc] init];
        charsRead = 0;
        line = 1;
        column = 0;
        state = TS_DEF;
        token = [[MapToken alloc]init];
    }
    
    return self;
}

- (char)readChar {
    if (state == EOF)
        [NSException raise:@"Invalid State" format:@"read past end of file"];
    
    if ([pushBuffer length] > 0) {
        char c = [pushBuffer characterAtIndex:0];
        [pushBuffer deleteCharactersInRange:NSMakeRange(0, 1)];
        return c;
    }
    
    if (bufferIndex == bufferSize) {
        bufferIndex = 0;
        bufferSize = [stream read:buffer maxLength:1024];
        if (bufferSize <= 0) {
            state = TS_EOF;
            return 0;
        }
    }
    
    char c = (char)buffer[bufferIndex++];
    if (c == '\n') {
        line++;
        column = 0;
    } else {
        column++;
    }
    
    return c;
}

- (void)pushBack:(char)c {
    [pushBuffer appendFormat:@"%c", c];
}

- (MapToken *)nextToken {
    char c;
    while ((c = [self readChar]) != 0) {
        charsRead++;
        switch (state) {
            case TS_DEF:
                switch (c) {
                    case '/': {
                        char d = [self readChar];
                        if (d == '/') {
                            state = TS_COM;
                            string = [[NSMutableString alloc] init];
                            startLine = line;
                            startColumn = column;
                            break;
                        } else {
                            [self pushBack:d];
                        }
                    }
                    case '\r':
                    case '\n':
                    case '\t':
                    case ' ':
                        break; // ignore whitespace in boundaries
                    case '{':
                        return [token setType:TT_CB_O data:nil line:line column:column charsRead:charsRead];
                    case '}':
                        return [token setType:TT_CB_C data:nil line:line column:column charsRead:charsRead];
                    case '(':
                        return [token setType:TT_B_O data:nil line:line column:column charsRead:charsRead];
                    case ')':
                        return [token setType:TT_B_C data:nil line:line column:column charsRead:charsRead];
                    case '[':
                        return [token setType:TT_SB_O data:nil line:line column:column charsRead:charsRead];
                    case ']':
                        return [token setType:TT_SB_C data:nil line:line column:column charsRead:charsRead];
                    case '"':
                        state = TS_Q_STR;
                        string = [[NSMutableString alloc] init];
                        startLine = line;
                        startColumn = column;
                        break;
                    case '-':
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        state = TS_DEC;
                        string = [[NSMutableString alloc] init];
                        [string appendFormat:@"%c", c];
                        startLine = line;
                        startColumn = column;
                        break;
                    default:
                        state = TS_STR;
                        string = [[NSMutableString alloc] init];
                        [string appendFormat:@"%c", c];
                        startLine = line;
                        startColumn = column;
                        break;
                }
                break;
            case TS_Q_STR:
                switch (c) {
                    case '"': {
                        [token setType:TT_STR data:string line:startLine column:startColumn charsRead:charsRead];
                        [string release];
                        string = nil;
                        state = TS_DEF;
                        return token;
                    default:
                        [string appendFormat:@"%c", c];
                        break;
                    }
                }
                break;
            case TS_STR: {
                BOOL comment = NO;
                switch (c) {
                    case '/': {
                        char d = [self readChar];
                        if (d == '/')
                            comment = YES;
                        else
                            [self pushBack:d];
                    }
                    case '\r':
                    case '\n':
                    case '\t':
                    case ' ': {
                        [token setType:TT_STR data:string line:startLine column:startColumn charsRead:charsRead];
                        [string release];
                        string = nil;
                        state = comment ? TS_COM : TS_DEF;
                        return token;
                    default:
                        [string appendFormat:@"%c", c];
                        break;
                    }
                }
                break;
            }
            case TS_DEC:
                if (c == '.')
                    state = TS_FRAC;
            case TS_FRAC: {
                BOOL comment = NO;
                switch (c) {
                    case '/': {
                        char d = [self readChar];
                        if (d == '/')
                            comment = YES;
                        else
                            [self pushBack:d];
                    }
                    case '\r':
                    case '\n':
                    case '\t':
                    case ' ': {
                        NSNumber* number;
                        if (state == TS_DEC) {
                            number = [NSNumber numberWithInt:[string intValue]];
                            [string release];
                            string = nil;
                            state = comment ? TS_COM : TS_DEF;
                            return [token setType:TT_DEC data:number line:startLine column:startColumn charsRead:charsRead];
                        } else {
                            number = [NSNumber numberWithFloat:[string floatValue]];
                            [string release];
                            string = nil;
                            state = comment ? TS_COM : TS_DEF;
                            return [token setType:TT_FRAC data:number line:startLine column:startColumn charsRead:charsRead];
                        }
                        break;
                    }
                    default: {
                        if ((c < '0' || c > '9') && (c != '.'))
                            state = TS_STR;
                        [string appendFormat:@"%c", c];
                        break;
                    }
                }
                break;
            }
            case TS_COM:
                switch (c) {
                    case '\r':
                    case '\n':
                        [token setType:TT_COM data:string line:startLine column:startColumn charsRead:charsRead];
                        [string release];
                        string = nil;
                        state = TS_DEF;
                        return token;
                    default:
                        [string appendFormat:@"%c", c];
                        break;
                    }
                break;
            default:
                break;
        }
    }
    
    return nil;
}

- (void)dealloc {
    [pushBuffer release];
    [token release];
    [stream release];
    [string release];
    [super dealloc];
}

@end
