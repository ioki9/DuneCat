#import <Cocoa/Cocoa.h>

class DuneCat::ProcessTracker;
@interface OSXProcessObserver : NSObject
-(void)setCallback:(DuneCat::ProcessTracker*)callback;
-(void)enableNotification;
-(void)appLaunched:(NSNotification *)note;
-(void)appTerminated:(NSNotification *)note;
@end

