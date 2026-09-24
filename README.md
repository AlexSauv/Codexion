*This project has been created as part of the 42 curriculum by alsauvan.*

# Codexion

## Description

`Codexion` is a multithreaded simulation of a set of `coder` threads competing
for shared USB "dongles" in order to compile "quantum code".

Coders are arranged in a ring, each seated between two dongles (its
`left_dongle` and `right_dongle`, shared with its neighbors). To compile, a
coder must acquire **both** of its adjacent dongles at once. If it cannot get
them fast enough, it eventually "burns out". The simulation stops as soon as
one coder burns out, or once every coder has completed the required number of
compiles.

Each dongle also enforces a cooldown period after being released, during which
it cannot be taken again, and coders are served in either FIFO or EDF
(Earliest Deadline First) order depending on the requested scheduling policy.

## Instructions

### Build

```sh
make
```

This produces a `codexion` binary, compiled with `-Wall -Wextra -Werror -pthread`.

### Run

```sh
./codexion nb_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor nb_compiles_required dongle_cooldown scheduler
```

| Argument                | Description                                                              |
|--------------------------|--------------------------------------------------------------------------|
| `nb_coders`              | Number of coder threads (and dongles) in the simulation                  |
| `time_to_burnout`        | Time (ms) since a coder's last compile before it burns out               |
| `time_to_compile`        | Time (ms) a coder spends compiling once it has both dongles              |
| `time_to_debug`          | Time (ms) a coder spends debugging after compiling                       |
| `time_to_refactor`       | Time (ms) a coder spends refactoring after debugging                     |
| `nb_compiles_required`   | Number of compiles each coder must complete for the simulation to stop successfully |
| `dongle_cooldown`        | Time (ms) a dongle stays unavailable after being released                |
| `scheduler`              | Scheduling policy: `fifo` or `edf`                                       |

Example:

```sh
./codexion 4 800 200 100 100 6 100 fifo
```

### Output format

Every event is printed on its own line as:

```
<timestamp_ms> <coder_id> <event>
```

where `<event>` is one of: `has taken a dongle`, `is compiling`, `is debugging`,
`is refactoring`, `burned out`. Access to the output is serialized under a mutex,
so lines are never interleaved or corrupted.

## Resources

- POSIX threads documentation (`man pthread_create`, `man pthread_mutex_init`,
  `man pthread_cond_init`, `man pthread_cond_timedwait`)

## Blocking cases handled

- **Deadlock avoidance**: a coder never holds one dongle while blocking forever
  on the other. Waiting on a dongle uses `pthread_cond_timedwait` with a short,
  bounded timeout (5ms) instead of an unbounded `pthread_cond_wait`, so a coder
  can never be stuck holding a lock if a wakeup signal is lost or arrives before
  it starts waiting.
- **Single coder**: with only one coder, it can never acquire two distinct
  dongles, so it simply waits (without attempting to compile) until it burns out
  or the simulation is otherwise stopped.
- **Simulation shutdown**: when the simulation stops (burnout, all compiles
  done), the monitor thread broadcasts on every dongle's condition variable so
  that every coder blocked in a wait wakes up immediately instead of waiting out
  its timeout.
- **Allocation / thread-creation failures**: every `malloc` and every
  `pthread_create` return value is checked. On failure, everything already
  allocated/created is cleanly freed/stopped and the program exits with an error
  instead of crashing or leaking.
- **Resource cleanup on early exit**: whatever the failure point during
  initialization (argument parsing, allocation, mutex/condvar init, heap
  creation, thread creation), only the resources that were actually initialized
  are destroyed/freed, and each exactly once.

## Thread synchronization mechanisms

- **`events_mutex`**: a single mutex protecting all shared simulation state that
  isn't per-dongle (each coder's last compile time and compile count, the
  simulation-stopped flag, dongle cooldown timestamps, and console output). This
  is the only lock ever used to guard `printf`, guaranteeing atomic, non-
  interleaved log lines.
- **Per-dongle mutex + condition variable** (`dongles[i]` / `condi[i]`): each
  dongle has its own mutex and condition variable. A coder wanting a dongle locks
  it, registers its request in that dongle's priority heap, and waits on the
  condition variable (bounded `pthread_cond_timedwait`) until it is next in line
  and the dongle is out of cooldown. Releasing a dongle broadcasts on its
  condition variable to wake up every coder waiting for it.
- **Priority heap per dongle**: each dongle keeps a small binary heap of pending
  requests, ordered by the active scheduling policy (FIFO: request time, then
  lowest coder id; EDF: earliest deadline, then highest coder id). This is what
  decides, among several coders waiting on the same dongle, who gets to take it
  next.
- **Monitor thread** (`events_checker`): a dedicated thread periodically checks
  whether any coder has burned out or whether every coder has completed the
  required number of compiles, and sets the shared stop flag (under
  `events_mutex`) accordingly, then wakes every dongle's condition variable so
  all coder threads can observe the stop and exit.