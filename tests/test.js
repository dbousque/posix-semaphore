

const Semaphore = require('../addon')

const sem = Semaphore('salut00', { strict: true, debug: true })
sem.acquire()
const sem2 = Semaphore('salut00', { strict: true, debug: true })
console.log('before sem2 acquire')
sem2.acquire()