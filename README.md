crashfirefox-intentionally
==========================

Tool for QA/testers to intentionally crash Firefox in a way that should cause the crash reporter to start.

Possible usages:

* `crashfirefox.exe [<processname.exe>]` - Finds a process with the specified name and causes it to crash. If no name is specified, it defaults to firefox.exe. Note that if there are multiple instances of that process name running, this will crash the first one it finds; this is usually the one most recently launched, but don't rely on that!
* `crashfirefox.exe <pid> ` - Finds the process with the specified process ID and causes it to crash
