# @vastblast/node-minify

Native Node bindings for the [tdewolff/minify](https://github.com/tdewolff/minify) Go minifier. Minification runs in-process via N-API, so you get the Go implementation's speed without shelling out to a CLI.

## Features

- Go-powered native addon; fast and no child processes.
- Promise-based `minify(data, options)` API that returns the minified string.
- Supports CSS, HTML, JS, JSON, SVG, XML, and import maps with the same tuning flags as the Go minifier.
- Ships with prebuilt binaries for common platforms and falls back to a local build when needed.

## Installation

```bash
npm install @vastblast/node-minify
```

If your platform is not covered by the included prebuilds, make sure you have Go and the usual `node-gyp` toolchain available so the native module can compile during install.

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

The package embeds the Go minifier in a native N-API addon. Work happens inside the addon (no spawned processes), which keeps throughput close to the original Go project and avoids the overhead of piping data through a CLI.

## Credits

Powered by [tdewolff/minify](https://github.com/tdewolff/minify/).

## License

MIT
