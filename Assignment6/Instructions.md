Lab: locks
In this lab you'll gain experience in re-designing code to increase parallelism. A common symptom of poor parallelism on multi-core machines is high lock contention. Improving parallelism often involves changing both data structures and locking strategies in order to reduce contention. You'll do this for the xv6 memory allocator, and you'll add a readers/writers lock to the kernel.

Before writing code, make sure to read the following parts from the xv6 book :

Chapter 7: "Locking" and the corresponding code.
Section 3.5: "Code: Physical memory allocator"
  $ git fetch
  $ git checkout lock
  $ make clean
Memory allocator (moderate)
The program user/kalloctest stresses xv6's memory allocator: three processes grow and shrink their address spaces, resulting in many calls to kalloc and kfree. kalloc and kfree obtain kmem.lock. kalloctest prints (as "#test-and-set") the number of loop iterations in acquire due to attempts to acquire a lock that another core already holds, for the kmem lock and a few other locks. The number of loop iterations in acquire is a rough measure of lock contention. The output of kalloctest looks similar to this before you start the lab:

$ kalloctest
start test1
test1 results:
--- lock kmem stats
lock: kmem: #test-and-set 109387 #acquire() 433062
--- top 5 contended locks:
lock: proc: #test-and-set 175682 #acquire() 269903
lock: proc: #test-and-set 134313 #acquire() 269902
lock: virtio_disk: #test-and-set 114002 #acquire() 168
lock: kmem: #test-and-set 109387 #acquire() 433062
lock: proc: #test-and-set 51770 #acquire() 269904
tot= 109387
test1 FAIL
start test2
total free number of pages: 32465 (out of 32768)
..........
test2 OK
start test3
..........child done 10000

test3 OK
start test4
............................child done 100000
.child done 100000
.child done 100000
--- lock kmem stats
lock: kmem: #test-and-set 501926 #acquire() 3129957
--- top 5 contended locks:
lock: wait_lock: #test-and-set 17670749 #acquire() 40004
lock: proc: #test-and-set 2471506 #acquire() 2552213
lock: kmem: #test-and-set 501926 #acquire() 3129957
lock: proc: #test-and-set 305810 #acquire() 2000185
lock: proc: #test-and-set 181696 #acquire() 1130037
tot= 501926
test4 FAIL m 153305 n 501926
$ 
You'll likely see different counts than shown here, and a different order for the top 5 contended locks.

acquire maintains, for each lock, the count of calls to acquire for that lock, and the number of times the loop in acquire tried but failed to set the lock. kalloctest calls a system call that causes the kernel to print those counts for the kmem locks (which are the focus of this lab) and for the 5 most contended locks. If there is lock contention the number of acquire loop iterations will be large. The system call returns the sum of the number of loop iterations for the kmem locks.

For this lab, you must use a dedicated unloaded machine with multiple cores. If you use a machine that is doing other things, the counts that kalloctest prints will be nonsense.

The root cause of lock contention in kalloctest is that kalloc() has a single free list, protected by a single lock. To remove lock contention, you will have to redesign the memory allocator to avoid a single lock and list. The basic idea is to maintain a free list per CPU, each list with its own lock. Allocations and frees on different CPUs can run in parallel, because each CPU will operate on a different list. The main challenge will be to deal with the case in which one CPU's free list is empty, but another CPU's list has free memory; in that case, the one CPU must "steal" part of the other CPU's free list. Stealing may introduce lock contention, but that will hopefully be infrequent.

Your job is to implement per-CPU freelists, and stealing when a CPU's free list is empty. You must give all of your locks names that start with "kmem". That is, you should call initlock for each of your locks, and pass a name that starts with "kmem". Run kalloctest to see if your implementation has reduced lock contention. To check that it can still allocate all of memory, run usertests sbrkmuch. Your output will look similar to that shown below, with much-reduced contention in total on kmem locks, although the specific numbers will differ. Make sure all tests in usertests -q pass. make grade should say that the kalloctests pass.

$ kalloctest
start test1
test1 results:
--- lock kmem stats
lock: kmem: #test-and-set 0 #acquire() 117911
lock: kmem: #test-and-set 0 #acquire() 169464
lock: kmem: #test-and-set 0 #acquire() 145786
--- top 5 contended locks:
lock: virtio_disk: #test-and-set 93747 #acquire() 183
lock: proc: #test-and-set 44010 #acquire() 526316
lock: proc: #test-and-set 24347 #acquire() 526309
lock: wait_lock: #test-and-set 11726 #acquire() 12
lock: pr: #test-and-set 4579 #acquire() 5
tot= 0
test1 OK
start test2
total free number of pages: 32463 (out of 32768)
..........
test2 OK
start test3
..........child done 10000

test3 OK
start test4
..........child done 100000
--- lock kmem stats
lock: kmem: #test-and-set 3673 #acquire() 827384
lock: kmem: #test-and-set 3449 #acquire() 1215152
lock: kmem: #test-and-set 1924 #acquire() 1236349
lock: kmem: #test-and-set 0 #acquire() 1014
lock: kmem: #test-and-set 0 #acquire() 1014
lock: kmem: #test-and-set 0 #acquire() 1014
lock: kmem: #test-and-set 0 #acquire() 1014
lock: kmem: #test-and-set 0 #acquire() 1014
--- top 5 contended locks:
lock: wait_lock: #test-and-set 39121537 #acquire() 40020
lock: proc: #test-and-set 6853704 #acquire() 1672258
lock: proc: #test-and-set 214194 #acquire() 1614201
lock: uart: #test-and-set 195773 #acquire() 1459
lock: virtio_disk: #test-and-set 93747 #acquire() 183
tot= 9046

test4 OK
$ usertests sbrkmuch
usertests starting
test sbrkmuch: OK
ALL TESTS PASSED
$ usertests -q
...
ALL TESTS PASSED
$
Hints
Read-write lock (moderate)
This half of the assignment is independent from the first half; you can work on this half (and pass the tests) whether or not you have completed the first half.

Consider xv6's sys_pause and sys_uptime functions, which read the global ticks variable. Since that variable might be concurrently updated by clockintr, these two functions acquire the tickslock spinlock before reading ticks. Importantly, this prevents clockintr from concurrently modifying ticks, but this also prevents multiple cores from reading ticks at the same time, which would have been just fine. In the latter case, the spinlock needlessly reduces performance.

A common solution to this problem is to use a read-write lock. Read-write locks introduce the notion of two kinds of lock holders: readers and writers. There can be at most one writer at a time (and when there's a writer, there can be no readers), but there can be many readers at the same time (as long as there's no writer). The typical read-write lock API is as follows (see kernel/defs.h):

void            initrwlock(struct rwspinlock*);
void            read_acquire(struct rwspinlock*);
void            read_release(struct rwspinlock*);
void            write_acquire(struct rwspinlock*);
void            write_release(struct rwspinlock*);
One subtle issue that can arise with read-write locks is that, if there are many readers, then writers might never get a chance to run. In other words, even though readers keep acquiring and releasing the lock, there's never a point when there are actually zero readers, so a writer might never be able to acquire the lock. To address this problem, read-write locks typically implement a writer priority scheme: once a writer tries to acquire the lock, subsequent readers must wait until the writer succeeds in acquiring and releasing the lock (of course, waiting for the current readers to release the lock before allowing the writer to acquire).

Your implementation of read_acquire must never wait for another CPU that's holding a read-lock, or in the process of acquiring or releasing a read-lock. (This property is technically called "wait-free".) This means that your implementation cannot acquire a spinlock to implement the read-write lock---that would mean that one core that wants to acquire a read-lock would have to potentially wait for other cores that are also acquiring a read-lock. Instead, you should directly use atomic operations.

Implement a read-write spinlock in xv6, following the API and semantics described above. Make sure that readers do not starve writers: if there are any pending writers, no more readers can acquire the lock.

You will need to fill in the stubbed-out functions for the read-write spinlock API in kernel/spinlock.c, and likely change the definition of a struct rwspinlock in kernel/spinlock.h.

When you are done, test your rwspinlock implementation by running rwlktest. You should see output similar to the following:

$ rwlktest
rwspinlock_test: step 1: initrwlock
rwspinlock_test: step 2: concurrent read_acquire
rwspinlock_test: step 3: concurrent read_release
rwspinlock_test: step 4: wait-free read acquire/release
rwspinlock_test: step 5: prepare read_acquire for writer priority test
rwspinlock_test: step 6: writer priority test
rwspinlock_test: step 7: checking for concurrent readers/writers
rwspinlock_test: step 8: checking for concurrent writers
rwspinlock_test: step 9: acquiring multiple locks
rwspinlock_test: step 10: releasing multiple locks
rwspinlock_test: step 11: prepare read_acquire for multiple writer priority test
rwspinlock_test: step 12: multiple writer priority test
...
rwspinlock_test: step 29: prepare read_acquire for multiple writer priority test
rwspinlock_test: step 30: multiple writer priority test
rwspinlock_test: step 31: done
rwspinlock_test(1): 0
rwspinlock_test(0): 0
rwspinlock_test(2): 0
rwspinlock_test(3): 0
rwlktest: 4/4 CPUs succeeded
$
Make sure usertests -q still passes. make grade should pass all tests when you are done.

Hints

Submit the lab
Time spent
Create a new file, time.txt, and put in a single integer, the number of hours you spent on the lab. git add and git commit the file.

Answers
If this lab had questions, write up your answers in answers-*.txt. git add and git commit these files.

Submit
You will turn in your assignments using Google Classroom.
When you're ready to submit, run make zipball, which will generate lab.zip. Upload this zip file to the corresponding Google Classroom assignment.

If you run make zipball and you have either uncomitted changes or untracked files, you will see output similar to the following:

 M hello.c
?? bar.c
?? foo.pyc
Untracked files will not be handed in.  Continue? [y/N]
Inspect the above lines and make sure all files that your lab solution needs are tracked, i.e., not listed in a line that begins with ??. You can cause git to track a new file that you create using git add {filename}.

Please run make grade to ensure that your code passes all of the tests.
Commit any modified source code before running make zipball.
