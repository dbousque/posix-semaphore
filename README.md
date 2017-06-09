[![Build Status](https://travis-ci.org/dbousque/posix-semaphore.svg?branch=master)](https://travis-ci.org/dbousque/posix-semaphore)

# posix-semaphore
Blocking POSIX semaphores for Node.js

### Installation
`npm install posix-semaphore`

### Example
```javascript
const Semaphore = require('posix-semaphore')

const sem = new Semaphore('mySemaphore')
sem.acquire()

/* my code using shared ressources 😎 */

sem.release()
// other processes are now free to use the ressources

// remove the semaphore from the system
sem.close()
```

### Inter-process communication example
```javascript
const cluster = require('cluster')
const Semaphore = require('posix-semaphore')
const shm = require('shm-typed-array')

function parentProcess () {
  const semParent = new Semaphore('mySemaphore', { silent: true })
  const bufParent = shm.create(4096)
  // we get the lock
  semParent.acquire()

  // we create the child process
  const child = cluster.fork({ SHM_KEY: bufParent.key })

  // we write some data to the shared memory segment
  bufParent.write('hi there :)')
  // we release the lock
  semParent.release()

  // we close the child after a second
  setTimeout(() => { child.kill('SIGINT') }, 1000)
}

function childProcess () {
  const semChild = new Semaphore('mySemaphore', { silent: true })
  const shmKey = parseInt(process.env.SHM_KEY)
  const bufChild = shm.get(shmKey)
  
  // we get the lock, will block until the parent has released
  semChild.acquire()
  // should print 'hi there :)'
  console.log(bufChild.toString())
}

if (cluster.isMaster) {
  parentProcess()
} else if (cluster.isWorker) {
  childProcess()
}
```
Output:
```
$ node test.js
hi there :)
shm segments destroyed: 1
$
```

### API

#### `new Semaphore(semName, options)`

Opens a new or an already existing semaphore with `sem_open`. Fails with an error if the semaphore could not be opened.
- `semName` : name of the semaphore
- `options` :
  - `strict` : If set to false, `acquire`, `release` and `close` won't fail if the semaphore is already acquired/released/closed. Default : true
  - `closeOnExit` : If true, the semaphore will be closed on process exit (uncaughtException, SIGINT, normal exit). Default : true 
  - `debug` : Prints useful information. Default : false
  - `silent` : Some information is printed with `closeOnExit`=true and when native calls fait. Allows you to disable that behavior. Default : false
  - `retryOnEintr` : If `sem_wait` fails with `EINTR` (usually it's due to a SIGINT signal being fired on CTRL-C), try to acquire the lock again. Default : false

#### `sem.acquire()`

The call will block until the semaphore is acquired by the process (will happen instantly if no other process acquired the lock). Calls `sem_wait` under the hood.

#### `sem.release()`

Releases the semaphore if it had been acquired, allowing other processes to acquire the lock. Calls `sem_post` under the hood.

#### `sem.close()`

Closes and unlinks the semaphore, meaning that other processes will no longer have access to it. Calls `sem_close` and `sem_unlink` under the hood.
