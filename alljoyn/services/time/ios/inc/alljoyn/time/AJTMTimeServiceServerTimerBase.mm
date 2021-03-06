/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

/**
 * This class is experimental, and as such has not been tested.
 * Please help make it more robust by contributing fixes if you find issues
 **/

#import "AJTMTimeServiceServerTimerBase.h"

@interface AJTMTimeServiceServerTimerBase ()
@property AJTMTimeServiceServerTimerAdapter* handle;
@end

@implementation AJTMTimeServiceServerTimerBase

-(id)init
{
    self = [super init];
    if (self) {
    }
    return self;
}

-(void)registerAdapter:(AJTMTimeServiceServerTimerAdapter*) adapter
{
    self.handle = adapter;
}

-(void)releaseObject
{
    if (self.handle == nil) {
        return NSLog(@"Timer handle is nil");
    }
    self.handle->release();
}

-(NSString*)objectPath
{
    if (self.handle == nil) {
        NSLog(@"Timer handle is nil");
        return @"";
    }
    return @((self.handle->getObjectPath()).c_str());
}

/**
 * Emit the signal when the Timer is reached
 */
-(QStatus)TimerEvent
{
    return self.handle->timerEvent();
}

/**
 * Emit the signal when run state changed
 */
-(QStatus)RunStateChanged:(bool) isRunning
{
        return self.handle->runStateChanged(isRunning);
}

-(id)getHandle
{
    return (id)self.handle;
}
@end
