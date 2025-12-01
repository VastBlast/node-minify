import { minifyStringAsync } from './koffiBindings.js'
import type { MinifyOptions } from './types'
export type * from './types'

type NativeResult = {
  error?: string
  data?: string
}

function parseNativeResult(raw: string): NativeResult {
  try {
    return JSON.parse(raw || '{}')
  } catch (err) {
    const message = err instanceof Error ? err.message : 'Failed to parse native response'
    throw new Error(message)
  }
}

export async function minify(data: string, opts: MinifyOptions): Promise<string> {
  const raw = await minifyStringAsync(data, JSON.stringify(opts ?? {}))
  const result = parseNativeResult(raw)
  if (result.error) {
    throw new Error(result.error)
  }
  if (typeof result.data !== 'string') {
    throw new Error('Native response missing data')
  }
  return result.data
}
