

const Semaphore = require('../srcs/addon')

const sem = Semaphore('salut04')
sem.acquire()
sem.release()
sem.close()
