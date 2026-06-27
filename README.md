Codexion

Codexion is a concurrency simulation inspired by the classic Dining Philosophers problem. Multiple coders sit in a circular co-working hub, each needing two USB dongles (one left, one right) to compile their quantum code. The challenge is to orchestrate these coders using POSIX threads, mutexes, and condition variables so that no coder ever burns out while respecting dongle cooldowns and a configurable scheduling policy (FIFO or EDF).

The simulation stops either when every coder has compiled the required number of times, or when a coder burns out.
