[![Build Status](https://travis-ci.org/dbousque/posix-semaphore.svg?branch=master)](https://travis-ci.org/dbousque/posix-semaphore)

# posix-semaphore
Blocking POSIX semaphores for Node.js

## Installation
`npm install posix-semaphore`

## Example
```javascript
const sem = new Semaphore("mySemaphore")

sem.acquire()
// here, we acquired the lock

/* my code using shared ressources 👌 */

sem.release()
// other processes are free to use the ressources

// we are done with the semaphore
sem.close()
```
