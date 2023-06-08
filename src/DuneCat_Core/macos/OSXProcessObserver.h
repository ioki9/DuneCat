#import <Cocoa/Cocoa.h>
namespace DuneCat
{
class ProcessTracker;
@interface OSXProcessObserver : NSObject
-(void)setCallback:(ProcessTracker*)callback;
-(void)enableNotification;
-(void)appLaunched:(NSNotification *)note;
-(void)appTerminated:(NSNotification *)note;
@end
}
