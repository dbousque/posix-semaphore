

const { Worker, isMainThread } = require('worker_threads')
const Semaphore = require('../srcs/addon')

if (isMainThread) {
  // test if require works in worker thread
  new Worker(__filename)
}