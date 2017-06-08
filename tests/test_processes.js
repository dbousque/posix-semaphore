

function noStderr (output) {
  return output.stderr.length === 0
}

function hasStderr (output) {
  return output.stderr.length > 0
}

function noStdout (output) {
  return output.stdout.length === 0
}

function didTimedout (output) {
  return output.didTimedout
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

function lastLinesAre (strs) {
  const _lastLinesAre = ((output) => {
    if (strs.length > output.lines.length) {
      return false
    }
    const diff = output.lines.length - strs.length
    for (var i = output.lines.length - strs.length; i < output.lines.length; i++) {
      if (strs[i - diff] !== output.lines[i]) {
        return false
      }
    }
    return true
  })
  return _lastLinesAre
}

const processes = [
  {
    filename: 'tests/test1.js',
    validators: [noStderr, hasExitCode(0), lastLineIs('[posix-semaphore] done.')],
    timeout: 10 * 1000,
  },
  {
    filename: 'tests/test2.js',
    validators: [hasStderr, noStdout, hasExitCode(1)],
    timeout: 10 * 1000,
  },
  {
    filename: 'tests/test3.js',
    validators: [noStderr, noStdout, didTimedout, hasExitCode(0)],
    timeout: 2 * 1000,
    canTimeout: true,
  },
  {
    filename: 'tests/test4.js',
    validators: [noStderr, noStdout, hasExitCode(0)],
    timeout: 10 * 1000,
  },
  {
    filename: 'tests/test5.js',
    validators: [noStderr, hasExitCode(0), lastLineIs('[posix-semaphore] done.')],
    timeout: 10 * 1000,
  },
  {
    filename: 'tests/test6.js',
    validators: [noStderr, hasExitCode(0), lastLineIs('[posix-semaphore] done.')],
    timeout: 10 * 1000,
  },
  {
    filename: 'tests/test7.js',
    validators: [noStderr, hasExitCode(0), lastLinesAre(['first', 'second', 'third'])],
    timeout: 10 * 1000,
  },
]

module.exports = processes