#import "OSXProcessObserver.h"
#include "processinfo.h"

DuneCat::ProcessTracker* tracker;

@implementation OSXProcessObserver
-(void)setCallback:(DuneCat::ProcessTracker*)callback
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
    DuneCat::ProcessInfo proc;
    proc.name = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationName"]);
    proc.file_path = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationPath"]);
    proc.pid = [[[note userInfo] objectForKey:@"NSApplicationProcessIdentifier"] intValue];
    tracker->process_created_handler(proc);
    // You've been notified!
}

-(void)appTerminated:(NSNotification *)note
{
    NSLog(@"terminated %@\n", [[note userInfo] objectForKey:@"NSApplicationName"]);
    qDebug()<<"terminated";
    DuneCat::ProcessInfo proc;
    proc.name = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationName"]);
    proc.file_path = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationPath"]);
    proc.pid = [[[note userInfo] objectForKey:@"NSApplicationProcessIdentifier"] intValue];
    proc.termination_time = QDateTime::currentDateTime();
    tracker->process_deleted_handler(proc);
    // You've been notified!
}
@end
