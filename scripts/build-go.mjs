import { spawnSync } from 'child_process'
import { existsSync, mkdirSync } from 'fs'
import { dirname, join } from 'path'
import { fileURLToPath } from 'url'

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

const hostOs = toGoOS(process.platform)
const hostArch = toGoArch(process.arch)

const targetOs = process.env.GOOS || hostOs
const targetArch = process.env.GOARCH || hostArch

const ext = getExt(targetOs)

const repoRoot = join(__dirname, '..')
const goRoot = join(repoRoot, 'minify')
const buildRoot = join(repoRoot, 'build')
const outputDir = join(buildRoot, `${targetOs}-${targetArch}`)
const cacheDir = process.env.GOCACHE || join(repoRoot, '.cache', `${targetOs}-${targetArch}`, 'go-build')

const outputLib = join(outputDir, `nodeminify${ext}`)
const skipBuild = process.env.NODE_MINIFY_SKIP_BUILD === '1'
const forceBuild = process.env.NODE_MINIFY_FORCE_BUILD === '1'

console.log(`Building nodeminify for ${targetOs}/${targetArch} -> ${outputLib}`)

if (skipBuild) {
  console.log('Skipping Go build because NODE_MINIFY_SKIP_BUILD=1 (ensure the library exists at the expected path)')
  process.exit(0)
}

if (!forceBuild && existsSync(outputLib)) {
  console.log('Prebuilt library already present; skipping Go build. Set NODE_MINIFY_FORCE_BUILD=1 to rebuild.')
  process.exit(0)
}

ensureDir(buildRoot)
ensureDir(outputDir)
ensureDir(cacheDir)

const env = {
  ...process.env,
  GOOS: targetOs,
  GOARCH: targetArch,
  GOCACHE: cacheDir
}

const result = spawnSync('go', ['build', '-buildmode=c-shared', '-o', outputLib, '.'], {
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
