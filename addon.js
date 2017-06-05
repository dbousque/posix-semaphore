var addon = require('bindings')('addon');

const sem = new addon.Semaphore("salut00")
//sem.close()
sem.acquire()
console.log('acquired')
setTimeout(() => {}, 5000000)

process.on('SIGINT', () => {
  sem.close()
  process.exit()
})

//console.log('acquired')
//sem.release()
//sem.close()
//setTimeout(sem.release, 50000000)


/*var obj = new addon.MyObject(10);
console.log( obj.plusOne() ); // 11
console.log( obj.plusOne() ); // 12
console.log( obj.plusOne() ); // 13

console.log( obj.multiply().value ); // 13
console.log( obj.multiply(10).value ); // 130

var newobj = obj.multiply(-1);
console.log( newobj.value ); // -13
console.log( obj === newobj ); // false
*/