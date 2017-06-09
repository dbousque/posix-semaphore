

const cluster = require('cluster')
const Semaphore = require('../srcs/addon')
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
  console.log(bufChild.toString().slice(0, 11))
}

if (cluster.isMaster) {
  parentProcess()
} else if (cluster.isWorker) {
  childProcess()
}