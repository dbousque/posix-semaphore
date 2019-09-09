

// worker_threads support only starting from node 11
if (Number(process.version.match(/^v(\d+\.\d+)/)[1]) >= 11) {
  const wt = require('worker_threads')
  const Semaphore = require('../srcs/addon')

  if (wt.isMainThread) {
    // test if require works in worker thread
    new wt.Worker(__filename)
  }
}