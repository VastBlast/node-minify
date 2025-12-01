import koffi, { type KoffiFunc } from 'koffi'
import { resolveLibPath } from './libPath.js'

const libPath = resolveLibPath(String(koffi.extension))
const lib = koffi.load(libPath)

const freeCString: KoffiFunc<(ptr: unknown) => void> = lib.func('void FreeCString(void *ptr)')
koffi.disposable('minify_result', koffi.types.str, (ptr: unknown) => freeCString(ptr))

const minifyFn: KoffiFunc<(data: string, optionsJson: string) => string> = lib.func(
  'minify_result MinifyString(char *data, char *options)'
)

export function minifyString(data: string, optionsJson: string): string {
  return minifyFn(data, optionsJson)
}

export function minifyStringAsync(data: string, optionsJson: string): Promise<string> {
  return new Promise((resolve, reject) => {
    minifyFn.async(data, optionsJson, (err, res) => {
      if (err) {
        reject(err)
      } else {
        resolve(res)
      }
    })
  })
}
