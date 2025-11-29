import path from 'path';
import { defineConfig } from 'tsdown'

export default defineConfig({
    entry: ['./src/index.ts'],
    platform: 'node',
    clean: true,
    dts: true,
    format: ['esm', 'cjs'],
    external: [
        path.resolve(import.meta.dirname, './generated-bindings/index.js')
    ],
});