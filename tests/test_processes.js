

function noStderr (output) {
  return output.stderr.length === 0
}

function hasStderr (output) {
  return output.stderr.length > 0
}

function noStdout (output) {
  return output.stdout.length === 0
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
]

module.exports = processes