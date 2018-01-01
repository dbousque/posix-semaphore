

const cluster = require('cluster')
const Semaphore = require('../srcs/addon')

if (cluster.isMaster) {
  const worker = cluster.fork()
  setTimeout(() => worker.kill('SIGINT'), 2500)
  const semMaster = Semaphore('salut00', { silent: true })
  semMaster.acquire()
  setTimeout(() => console.log('second'), 1000)
  setTimeout(() => semMaster.release(), 1500)

} else if (cluster.isWorker) {
  setTimeout(() => {
    const semChild = Semaphore('salut00', { silent: true })
      console.log('first')
    	semChild.acquire()
    	console.log('third')
  }, 300)
}
