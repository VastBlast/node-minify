# Depreciated, [use minify](https://github.com/tdewolff/minify/tree/master/bindings/js)

This was a proof-of-concept module which has been written for [tdewolff/minify](https://github.com/tdewolff/minify/tree/master/bindings/js). My PR based on this was successfully merged and an NPM package has been published at `@tdewolff/minify` so all maintenance will be done there.

# @vastblast/node-minify

Native bindings for the [tdewolff/minify](https://github.com/tdewolff/minify) Go minifier powered by [koffi](https://github.com/Koromix/koffi). Minification runs in-process against a Go-built shared library, so you get the Go implementation's speed without shelling out to a CLI.

## Features

- Go-powered minifier loaded through koffi; no child processes.
- Promise-based `minify(data, options)` API that returns the minified string.
- Supports CSS, HTML, JS, JSON, SVG, XML, and import maps with the same tuning flags as the Go minifier.
- Cross-compile the Go shared library with `GOOS/GOARCH` or point to a custom build via `NODE_MINIFY_LIB_PATH`.

## Installation

```bash
npm install @vastblast/node-minify
```

The install step builds a small Go shared library for your host platform. Ensure Go is available in `PATH` (or set `GOOS/GOARCH` to cross-compile). You can also point to a prebuilt library by setting `NODE_MINIFY_LIB_PATH`.

### Shipping/using prebuilts

- Run `GOOS=<target> GOARCH=<target> npm run build:go` (or `npm run build:go -- --goos <target> --goarch <target>`) to emit `build/<goos>-<goarch>/nodeminify.<ext>`.
- Keep `build/` in your npm package so installs on those platforms can reuse the prebuilt.
- `postinstall` skips building when a matching prebuilt exists. Set `NODE_MINIFY_FORCE_BUILD=1`/`NODE_MINIFY_SKIP_BUILD=1` (or pass `--force-build`/`--skip-build`) to override; use `NODE_MINIFY_DEBUG_BUILD=1` or `--debug-build` to keep symbols for native debugging.
- CI workflow `.github/workflows/prebuilds.yml` builds prebuilts per platform and publishes when `publish=true` on `workflow_dispatch`. Publishing expects the package to be set up as an npm trusted publisher (OIDC/id-token) or for an automation token to be provided via `NODE_AUTH_TOKEN`.

## Usage

```typescript
import { minify } from '@vastblast/node-minify';

const html = `
<!doctype html>
<html>
  <head><title>Demo</title></head>
  <body><h1>Hello, world!</h1></body>
</html>`;

const minified = await minify(html, {
  type: 'text/html',
  htmlKeepComments: false,
  htmlKeepWhitespace: false,
});

console.log(minified);
```

```typescript
const js = await minify('const sum = (a, b) => a + b;', {
  type: 'application/javascript',
  jsPrecision: 3,
});
```

`minify` returns a `Promise<string>` and rejects with an `Error` if the input or options are invalid.

## API

### `minify(data: string, options: MinifyOptions): Promise<string>`

- `data`: the string to minify (required).
- `options.type`: media type (required).
- Remaining `MinifyOptions` map directly to [tdewolff/minify](https://github.com/tdewolff/minify/) flags.

Supported media types:

`'text/css'`, `'text/html'`, `'image/svg+xml'`, `'application/javascript'`, `'application/json'`, `'application/rss+xml'`, `'application/manifest+json'`, `'application/xhtml-xml'`, `'text/xml'`, `'importmap'`

Common options (all optional unless noted):

- `cssPrecision`: decimal precision for CSS numbers.
- `htmlKeepComments`, `htmlKeepDefaultAttrvals`, `htmlKeepDocumentTags`, `htmlKeepEndTags`, `htmlKeepQuotes`, `htmlKeepSpecialComments`, `htmlKeepWhitespace`: control how much HTML is preserved.
- `jsKeepVarNames`, `jsPrecision`, `jsVersion`: adjust JS minification.
- `jsonKeepNumbers`, `jsonPrecision`: JSON minification tuning.
- `svgKeepComments`, `svgPrecision`: SVG-specific controls.
- `xmlKeepWhitespace`: preserve XML whitespace.
- `url`: base URL used by the underlying minifier.

## Native bindings and performance

The package loads a Go-built shared library via koffi. All work happens inside that library (no spawned processes), keeping throughput close to the original Go project and avoiding CLI overhead.

## Credits

Powered by [tdewolff/minify](https://github.com/tdewolff/minify/).

## License

MIT
