#ifndef OSXPROCESSOBSERVER_H
#define OSXPROCESSOBSERVER_H
#import <Cocoa/Cocoa.h>

class DCProcessTracker;
@interface OSXProcessObserver : NSObject
  -(void)setCallback:(DCProcessTracker*)callback;
  -(void)enableNotification;
  -(void)appLaunched:(NSNotification *)note;
  -(void)appTerminated:(NSNotification *)note;
@end

#endif // OSXPROCESSOBSERVER_H
