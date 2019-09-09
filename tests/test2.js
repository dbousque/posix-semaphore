

const Semaphore = require('../srcs/addon')

const sem = Semaphore('salut01', { silent: true })
sem.acquire()
const sem2 = Semaphore('salut01', { silent: true })
console.log('should not appear')
sem2.acquire()