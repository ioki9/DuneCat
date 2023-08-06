#import "OSXProcessObserver.h"
#include "processinfo.h"
#include <sys/sysctl.h>
#include <sys/types.h>
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
    DuneCat::ProcessInfo proc;
    proc.name = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationName"]);
    proc.file_path = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationPath"]);
    proc.pid = [[[note userInfo] objectForKey:@"NSApplicationProcessIdentifier"] intValue];
    const int pid = proc.pid;
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, pid };
    struct kinfo_proc proc_info;
    size_t size = sizeof(proc_info);
    sysctl(mib, 4, &proc_info, &size, NULL, 0);
    proc.creation_time = QDateTime::fromSecsSinceEpoch(proc_info.kp_proc.p_starttime.tv_sec);
    qDebug()<<proc.name<<"launched time:"<<proc.creation_time;
    emit tracker->process_created(proc);
    // You've been notified!
}

-(void)appTerminated:(NSNotification *)note
{
    DuneCat::ProcessInfo proc;
    proc.name = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationName"]);
    proc.file_path = QString::fromNSString([[note userInfo] objectForKey:@"NSApplicationPath"]);
    proc.pid = [[[note userInfo] objectForKey:@"NSApplicationProcessIdentifier"] intValue];
    proc.termination_time = QDateTime::currentDateTime();
    std::vector<DuneCat::ProcessInfo> proc_list{};
    DuneCat::ProcessTracker::get_instance()->get_process_list(proc_list);
    for(const auto& elem : proc_list)
    {
        if(elem.name == proc.name && elem.pid == proc.pid)
        {
            proc.creation_time = elem.creation_time;
            break;
        }
    }
    qDebug()<<proc.name<<"termination time:"<<proc.creation_time;
    emit tracker->process_deleted(proc);
    // You've been notified!
}
@end
