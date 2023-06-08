#import <Cocoa/Cocoa.h>
#include "processtracker.h"

@interface OSXProcessObserver : NSObject
-(void)setCallback:(DuneCat::ProcessTracker*)callback;
-(void)enableNotification;
-(void)appLaunched:(NSNotification *)note;
-(void)appTerminated:(NSNotification *)note;
@end

