

// worker_threads support only starting from node 11
if (Number(process.version.match(/^v(\d+\.\d+)/)[1]) >= 11) {
  const { Worker, isMainThread } = require('worker_threads')
  const Semaphore = require('../srcs/addon')

  if (isMainThread) {
    // test if require works in worker thread
    new Worker(__filename)
  }
}