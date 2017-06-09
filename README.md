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

/* my code using shared ressources 👌 */

sem.release()
// other processes are now free to use the ressources

// remove the semaphore from the system
sem.close()
```
