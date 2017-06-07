

const childProcess = require('child_process')

function launchProcess (options, cb) {
  var stdout = ''
  var stderr = ''
  var returned = false
  var timeout = null

  const proc = childProcess.spawn('node', [options.filename])

  const exitProc = (code, hasTimedout) => {
    if (returned) {
      return
    }
    returned = true
    if (timeout) {
      clearTimeout(timeout)
    }
    proc.kill()
    cb({
      options,
      stdout,
      stderr,
      exitCode: 0,
      lines: stdout.split('\n').filter(l => l.length > 0),
      hasTimedout: hasTimedout,
    })
  }

  timeout = setTimeout(() => { exitProc(0, true) }, options.timeout)
  
  proc.stdout.on('data', (data) => { stdout += data.toString() })
  proc.stderr.on('data', (data) => { stderr += data.toString() })
  proc.on('close', (code) => { exitProc(code, false) })
  proc.on('error', (code) => { exitProc(code, false) })
}

function checkValidOutput (output) {
  const testFailed = (output, msg) => {
    console.log(output, { depth: null })
    console.log('')
    console.log(`\x1b[31m\x1b[1m[test failed]\x1b[0m [${output.options.filename}] ${msg}`)
    console.log('')

    throw new Error('test failed')
  }
  if (output.hasTimedout) {
    return testFailed(output, 'Timed out')
  }
  const options = output.options
  options.validators.forEach(validator => {
    if (validator(output) !== true) {
      return testFailed(output, `Validator '${validator.name}' failed`)
    }
  })
}

function noError (output) {
  return output.stderr.length === 0
}

function hasExitCode (nb) {
  const _hasExitCode = ((output) => {
    return output.exitCode === nb
  })
  return _hasExitCode
}

function lastLineIs (str) {
  const _lastLineIs = ((output) => {
    return output.lines[output.lines.length - 1] === str
  })
  return _lastLineIs
}

function launchProcesses (processes, cb) {
  const _launchProcesses = (acc, proc) => {
    if (proc.length === 0) {
      return cb(acc)
    }
    const first = proc[0]
    const rest = proc.slice(1)
    launchProcess(first, (output) => {
      acc.push(output)
      _launchProcesses(acc, rest)
    })
  }
  _launchProcesses([], processes)
}

const processes = [
  {
    filename: 'tests/test1.js',
    validators: [noError, hasExitCode(0), lastLineIs('[posix-semaphore] done.')],
    timeout: 10 * 1000,
  },
]

launchProcesses(processes, (outputs) => {
  outputs.forEach(checkValidOutput)
  console.log('\x1b[32m\x1b[1m[all tests successful]\x1b[0m')
})
