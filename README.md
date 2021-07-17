# Model-Checker
A Bounded Stateless Explicit-State Model Checker for Finding Deadlock and Assertion Errors in Multi-Threaded Software

# Abstract
     Model Checking is a type of search algorithm that can be used to
    search for bugs in multi-threaded software.  (It also has many other
    applications, besides multi-threaded software.)  It converts the
    multi-threaded software into a model, in which each thread continues
    to execute until it reaches a POSIX thread primitive (for example,
    pthread_mutex_lock(&mutex).  When all threads have stopped at
    a POSIX thread primitive, then we say that we have reached a new
    global software _state_.  From that state, we can then choose which
    thread will execute next.  We then let that thread execute until
    it again reaches a POSIX thread primitive.  We say that the global
    software has made a _transition_ from the previous global state to a
    new global state.  The set of states and transitions can be thought
    of as a directed graph.  (This is the "model" in model checking.)
    We will explore the graph by exploring all possible transitions.
    Each sequence of transitions is a _schedule_.  If we choose a
    single schedule of transitions, then we have converted the original
    multi-threaded program into a deterministic program that follows the
    prescribed transitions.  Our model-checker will execute each of the
    schedules to see if it can find a schedule that leads to a bug.
