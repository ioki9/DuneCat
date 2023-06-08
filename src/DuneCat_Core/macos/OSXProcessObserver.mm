#import "OSXProcessObserver.h"
#include "processtracker.h"
#include "processinfo.h"
ProcessTracker* tracker;

@implementation OSXProcessObserver
-(void)setCallback:(ProcessTracker*)callback
{
    tracker = callback;
}
- (void)enableNotification
{
    NSNotificationCenter*  center;

    NSLog(@"-[AppDelegate testNSWorkspace:]");

    // Get the custom notification center.

    center = [[NSWorkspace sharedWorkspace] notificationCenter];

    // Install the notifications.

    [center addObserver:self
        selector:@selector(appLaunched:)
        name:NSWorkspaceDidLaunchApplicationNotification
        object:nil
    ];
    [center addObserver:self
        selector:@selector(appTerminated:)
        name:NSWorkspaceDidTerminateApplicationNotification
        object:nil
    ];

    // Execution continues in -appLaunched: and -appTerminated:, below.
}

- (void)appLaunched:(NSNotification *)note
{
    NSLog(@"launched %@\n", [[note userInfo] objectForKey:@"NSApplicationName"]);
    qDebug()<<"launched";
    ProcessInfo proc;
    proc.name = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationName"]);
    proc.file_path = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationPath"]);
    proc.pid = [[[note userInfo] objectForKey:@"NSApplicationProcessIdentifier"] intValue];
    emit tracker->process_created(proc);
    // You've been notified!
}

-(void)appTerminated:(NSNotification *)note
{
    NSLog(@"terminated %@\n", [[note userInfo] objectForKey:@"NSApplicationName"]);
    qDebug()<<"terminated";
    ProcessInfo proc;
    proc.name = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationName"]);
    proc.file_path = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationPath"]);
    proc.pid = [[[note userInfo] objectForKey:@"NSApplicationProcessIdentifier"] intValue];
    emit tracker->process_deleted(proc);
    // You've been notified!
}
@end
