

const Semaphore = require('../addon')

const sem = new Semaphore('salut00', { strict: true, debug: false })
sem.acquire()
sem.release()
//sem.close()