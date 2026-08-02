*This project has been created as part of the 42 curriculum by mn-khili.*

## Description

Codexion is a concurrency simulation inspired by the classic Dining Philosophers problem. Multiple coders sit in a circular co-working hub, each needing two USB dongles (one left, one right) to compile -> debug (no dongle needed)-> refactor (no dongle needed).

The challenge is to implement this using POSIX threads, mutexes, and condition variables so that no coder ever burns out while respecting dongle cooldowns and a configurable scheduling policy (FIFO or EDF).

The simulation stops either when every coder has compiled the required number of times, or when a coder burns out.

--- 
## Instructions

**Compilation:**

```sh
make
```

Produces the `codexion` binary at the project root. `make re` forces a full rebuild,  `make clean`/`make fclean` remove object files and the binary respectively.

**Usage:**

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument                      | Meaning                                                                        |
| ----------------------------- | ------------------------------------------------------------------------------ |
| `number_of_coders`            | Number of coders (and dongles). Must be a positive integer.                    |
| `time_to_burnout`             | Milliseconds a coder may go without starting a compile before burning out.     |
| `time_to_compile`             | Milliseconds spent compiling, holding both dongles.                            |
| `time_to_debug`               | Milliseconds spent debugging, no dongles held.                                 |
| `time_to_refactor`            | Milliseconds spent refactoring, no dongles held.                               |
| `number_of_compiles_required` | Simulation succeeds once every coder reaches this many compiles.               |
| `dongle_cooldown`             | Milliseconds a released dongle stays unavailable before it can be taken again. |
| `scheduler`                   | `fifo` or `edf` how contested dongle access is arbitrated.                     |

All arguments except `scheduler` are integers, all must be non-negative, and `number_of_coders` must additionally be strictly positive.

**Example:**

```sh
./codexion 4 3000 200 200 200 10 400 edf
```


---

## Blocking cases handled

| Concern                                          | How it's handled                                                                                                                                                                                                                                                                                                     |
| ------------------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Deadlock (Coffman's circular-wait condition)** | A coder only ever claims both of its dongles **together**, under a single lock, or neither, there is no window in which a coder can be observed holding one dongle while waiting on the other, so no cyclic wait-for chain can ever form.                                                                            |
| **Starvation**                                   | Dongle access is arbitrated by a fixed-size (max 2 entries) priority heap per dongle, ordered by FIFO arrival order or EDF deadline (with arrival-order as a deterministic tie-break), so among the only two coders who could ever want a given dongle, priority is always resolved consistently.                    |
| **Dongle cooldown correctness**                  | A dongle's `free_at` cooldown-expiry timestamp is part of the same eligibility check used to decide whether a coder may claim it a coder can never take a dongle before its cooldown has elapsed.                                                                                                                    |
| **Busy-spin livelock**                           | The wait loop's wake-up deadline is clamped to never be scheduled in the past, without this, a stale cooldown timestamp caused `pthread_cond_timedwait` to return instantly and repeat forever, pegging a CPU core. This was found and fixed during development.                                                     |
| **Spurious wakeups**                             | Every wait is a `while` loop that fully re-checks its condition from live shared state after every wake-up, never assuming that waking up means the condition is now true.                                                                                                                                           |
| **Burnout precision (≤10ms)**                    | A dedicated monitor thread polls every coder's deadline at 1ms granularity, independent of the coder threads themselves.                                                                                                                                                                                             |
| **Log interleaving**                             | Every log line is wrapped in a single dedicated mutex, so two coders' state changes can never interleave mid-line.                                                                                                                                                                                                   |
| **Cooperative, race-free shutdown**              | A shared `stop` flag is checked at every blocking point, additionally, each coder independently checks its *own* deadline directly (not just the shared flag) before claiming dongles, closing a race where the monitor thread hadn't yet flagged a burnout that had, from that coder's own clock, already happened. |
| **Tool false positives**                         | One Helgrind warning persisted after all real races were fixed, traced to glibc's own internal condition-variable implementation rather than the project's code, and confirmed as a false positive by cross-checking with ThreadSanitizer, which reports zero races across repeated runs of the same scenarios.      |


---

### Thread synchronization mechanisms

The project uses `pthread_mutex_t` for mutual exclusion and `pthread_cond_t` for condition-variable-based waiting, structured around a single shared lock:

- **`sim_state->lock`** protects nearly everything touched by more than one thread: every coder's `last_compile_start`/`compile_count`, every dongle's `available`/`free_at`/`waiters`, and the simulation's `stop`/`finished_count` flags.

- **`sim_state->cond`** is what a coder blocks on while waiting for both its dongles, and what any state-changing thread (dongle release, burnout) broadcasts on. `broadcast` is used instead of `signal` because one condition variable covers every coder's wait, signaling only one risks waking the wrong coder while the actually-eligible one sleeps on, each thread's own `while` loop re-checks its eligibility before acting.

- **`t_ticket_counter`'s** own mutex is the one deliberate exception, protecting a request-ticket counter.

- **`t_logger`'s mutex** is kept separate on purpose, since it protects something unrelated to simulation state, serializing `printf` calls so two coders' log lines can never interleave mid-line.

**Example: race prevention on `last_compile_start`:** this field is written by a coder's own thread when it starts compiling, and read both by that coder's thread (to compute its own deadline before waiting) and independently by the monitor thread (to check for burnout). Both the write and both reads go through `sim_state->lock`, so there is no window in which the monitor could stale value.

**Example: thread-safe coordination between coders and the monitor:** the monitor thread never touches a coder's dongles or waiters directly, it only reads `last_compile_start` under the shared lock and, on detecting burnout, sets the shared `stop` flag and broadcasts `sim_state->cond`. Every coder thread's wait loop is already listening on that same condition variable, so a burnout detected by the monitor propagates to every blocked coder thread without any thread needing to poll the others directly.

## Resources

- [C for dummies Blogs](https://c-for-dummies.com/blog/?p=5365)
- [The Dining Philosophers Problem - youtube video](https://youtu.be/FYUi-u7UWgw?si=7clzCpXW8xXFcx9_)
- [2.6.3 Heap - Heap Sort - Heapify - Priority Queues - youtube video](https://www.youtube.com/watch?v=HqPJF2L5h9U&t=1860s)
- man pthread_create
- man pthread_join
- man pthread_cond_timedwait
- man pthread_cond_broadcast
- and last but not least GOOGLE (The goat)
 

**AI usage:** 

AI was used as a collaborative peer for the following tasks:

- **Debugging:** Tracing Helgrind/ThreadSanitizer output to real lock-order bugs.

- **Refactoring:** Consolidating per-coder/per-dongle locks into a single shared lock after two AB-BA deadlock hazards were found.

- **Documentation:** Aided in the structuring and drafting of this README.md file.
