const addon = require('bindings')('addon')

const sem = new addon.Semaphore("salut00")
//sem.close()
console.log('before acquire')
sem.acquire()
console.log('acquired')

setTimeout(() => {}, 500000000)

process.on('SIGINT', () => {
  //sem.close()
  //process.exit()
  console.log('sigint')
  sem.release()
  sem.acquire()
})