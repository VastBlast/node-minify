import { spawnSync } from 'child_process'
import { existsSync, mkdirSync } from 'fs'
import { dirname, join } from 'path'
import { fileURLToPath } from 'url'

if (typeof Bun !== "undefined") console.log(`Running build script with Bun`);

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

const cliArgs = process.argv.slice(2)

const hostOs = toGoOS(process.platform)
const hostArch = toGoArch(process.arch)

const rawTargetOs = getArgValue('goos') || process.env.GOOS
const rawTargetArch = getArgValue('goarch') || process.env.GOARCH

const targetOs = rawTargetOs ? toGoOS(rawTargetOs) : hostOs
const targetArch = rawTargetArch ? toGoArch(rawTargetArch) : hostArch

const ext = getExt(targetOs)

const repoRoot = join(__dirname, '..')
const goRoot = join(repoRoot, 'minify')
const buildRoot = join(repoRoot, 'build')
const outputDir = join(buildRoot, `${targetOs}-${targetArch}`)
const cacheDir = process.env.GOCACHE || join(repoRoot, '.cache', `${targetOs}-${targetArch}`, 'go-build')

const outputLib = join(outputDir, `nodeminify${ext}`)
const skipBuild = process.env.NODE_MINIFY_SKIP_BUILD === '1' || hasArg('skip-build')
const forceBuild = process.env.NODE_MINIFY_FORCE_BUILD === '1' || hasArg('force-build')
const isDebugBuild = process.env.NODE_MINIFY_DEBUG_BUILD === '1' || hasArg('debug-build')

console.log(`Building nodeminify for ${targetOs}/${targetArch} -> ${outputLib}`)

if (skipBuild) {
  console.log('Skipping Go build because NODE_MINIFY_SKIP_BUILD=1 or --skip-build was provided (ensure the library exists at the expected path)')
  process.exit(0)
}

if (!forceBuild && existsSync(outputLib)) {
  console.log('Prebuilt library already present; skipping Go build. Set NODE_MINIFY_FORCE_BUILD=1 or pass --force-build to rebuild.')
  process.exit(0)
}

ensureDir(buildRoot)
ensureDir(outputDir)
ensureDir(cacheDir)

const goArgs = ['build', '-buildmode=c-shared', '-o', outputLib]

if (!isDebugBuild) {
  goArgs.push('-trimpath', '-ldflags=-s -w', '-buildvcs=false')
} else {
  console.log('Debug build requested (--debug-build or NODE_MINIFY_DEBUG_BUILD=1); skipping production strip flags.')
}

goArgs.push('.')

const env = {
  ...process.env,
  GOOS: targetOs,
  GOARCH: targetArch,
  GOCACHE: cacheDir
}

const result = spawnSync('go', goArgs, {
  cwd: goRoot,
  env,
  stdio: 'inherit'
})

if (result.status !== 0) {
  process.exit(result.status ?? 1)
}

function ensureDir(pathname) {
  if (!existsSync(pathname)) {
    mkdirSync(pathname, { recursive: true })
  }
}

function hasArg(name) {
  return cliArgs.some(arg => arg === `--${name}`)
}

function getArgValue(name) {
  for (let i = 0; i < cliArgs.length; i++) {
    const arg = cliArgs[i]
    if (arg === `--${name}`) {
      const next = cliArgs[i + 1]
      if (next && !next.startsWith('-')) {
        return next
      }
    }
    if (arg.startsWith(`--${name}=`)) {
      return arg.slice(name.length + 3)
    }
  }
  return undefined
}

function toGoOS(platform) {
  return platform === 'win32' ? 'windows' : platform
}

function toGoArch(arch) {
  switch (arch) {
    case 'x64':
      return 'amd64'
    case 'ia32':
      return '386'
    default:
      return arch
  }
}

function getExt(os) {
  switch (os) {
    case 'windows':
      return '.dll'
    case 'darwin':
      return '.dylib'
    default:
      return '.so'
  }
}
