

const cluster = require('cluster')
const Semaphore = require('../srcs/addon')

if (cluster.isMaster) {
  const worker = cluster.fork()
  const semMaster = Semaphore('salut06')
  setTimeout(() => worker.kill('SIGINT'), 1000)

} else if (cluster.isWorker) {
  const semChild = Semaphore('salut06')
}