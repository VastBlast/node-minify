# @vastblast/node-minify

A Node.js wrapper for the [minify](https://github.com/tdewolff/minify/) command-line tool by tdewolff. This package provides a simple and type-safe way to use the minify tool in your Node.js projects.

## Installation

```bash
npm install @vastblast/node-minify
```

## Usage

```typescript
import { minify } from '@vastblast/node-minify';

async function example() {
  try {
    // Minify CSS files
    const result = await minify({
      all: true,
      cssPrecision: 2,
      inputs: ['styles.css', 'theme.css'],
      output: 'minified.css'
    });
    console.log('Minification successful:', result);

    // Minify HTML using stdin
    const htmlResult = await minify({
      type: 'html',
      stdin: '<html><body><h1>Hello, World!</h1></body></html>',
      htmlKeepComments: true
    });
    console.log('HTML minification result:', htmlResult);
  } catch (error) {
    console.error('Minification failed:', error);
  }
}

example();
```

## API

### `minify(options: MinifyOptions): Promise<string>`

Minifies files or input data using the specified options.

- `options`: An object containing the minification options. See the `MinifyOptions` interface for all available options.
- Returns: A promise that resolves with the stdout of the minify command.

## Supported Platforms

This package includes pre-built binaries for the following platforms:

- Windows (x64, arm64)
- macOS (x64, arm64)
- Linux (x64, arm64)

The appropriate binary will be automatically selected based on your system's platform and architecture.

## Credits

This package is a wrapper for the [minify](https://github.com/tdewolff/minify/) tool by Taco de Wolff. All credit for the core minification functionality goes to the original author and contributors of that project.

## License

[MIT License](LICENSE)