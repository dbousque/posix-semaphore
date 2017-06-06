const addon = require('bindings')('addon')

const sem = new addon.Semaphore("salut00")
sem.acquire()
console.log('acquired')
setTimeout(() => { sem.close() }, 5000)

process.on('SIGINT', () => {
  sem.close()
})