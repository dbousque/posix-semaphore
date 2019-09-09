

const Semaphore = require('../srcs/addon')

const sem = Semaphore('salut02', { strict: false, silent: true })
sem.acquire()
sem.acquire()
console.log('should not appear')
