import { exec } from 'child_process';
import { getBinaryFilePath } from './binary';

const BINARY_PATH = getBinaryFilePath();

/**
 * Options for the minify function
 */
export interface MinifyOptions {
    /** Number of significant digits to preserve in numbers, 0 is all */
    cssPrecision?: number;
    /** Preserve all comments in HTML */
    htmlKeepComments?: boolean;
    /** Preserve all IE conditional comments (DEPRECATED) */
    htmlKeepConditionalComments?: boolean;
    /** Preserve default attribute values in HTML */
    htmlKeepDefaultAttrvals?: boolean;
    /** Preserve html, head and body tags */
    htmlKeepDocumentTags?: boolean;
    /** Preserve all end tags in HTML */
    htmlKeepEndTags?: boolean;
    /** Preserve quotes around attribute values in HTML */
    htmlKeepQuotes?: boolean;
    /** Preserve all IE conditionals and SSI tags in HTML */
    htmlKeepSpecialComments?: boolean;
    /** Preserve whitespace characters but still collapse multiple into one in HTML */
    htmlKeepWhitespace?: boolean;
    /** Path inclusion pattern, includes paths previously excluded */
    include?: string[];
    /** Preserve original variable names in JavaScript */
    jsKeepVarNames?: boolean;
    /** Number of significant digits to preserve in numbers, 0 is all in JavaScript */
    jsPrecision?: number;
    /** ECMAScript version to toggle supported optimizations (e.g. 2019, 2020), by default 0 is the latest version */
    jsVersion?: number;
    /** Preserve original numbers instead of minifying them in JSON */
    jsonKeepNumbers?: boolean;
    /** Number of significant digits to preserve in numbers, 0 is all in JSON */
    jsonPrecision?: number;
    /** Recursively minify directories */
    recursive?: boolean;
    /** Preserve all comments in SVG */
    svgKeepComments?: boolean;
    /** Number of significant digits to preserve in numbers, 0 is all in SVG */
    svgPrecision?: number;
    /** Filetype (eg. css or text/css), optional when specifying inputs */
    type?: string;
    /** Preserve whitespace characters but still collapse multiple into one in XML */
    xmlKeepWhitespace?: boolean;
}

/**
 * Minify files or input data using the specified options
 * @param options - The options for minification
 * @returns A promise that resolves with the stdout of the minify command
 */
export function minify(options: MinifyOptions): Promise<string> {
    const args: string[] = [];

    // Convert options to command line arguments
    for (const [key, value] of Object.entries(options)) {
        if (key === 'inputs' || key === 'stdin') continue;

        const argKey = key.replace(/[A-Z]/g, letter => `-${letter.toLowerCase()}`);

        if (typeof value === 'boolean') {
            if (value) args.push(`--${argKey}`);
        } else if (Array.isArray(value)) {
            value.forEach(v => args.push(`--${argKey}`, v.toString()));
        } else if (value !== undefined) {
            args.push(`--${argKey}`, value.toString());
        }
    }

    // Add input files if provided
    if (options.inputs) {
        args.push(...options.inputs);
    }

    const command = `"${BINARY_PATH}" ${args.join(' ')}`;

    return new Promise((resolve, reject) => {
        const childProcess = exec(command, (error, stdout) => {
            if (error) {
                reject(error);
            } else {
                resolve(stdout);
            }
        });

        if (options.stdin) {
            childProcess.stdin?.write(options.stdin);
            childProcess.stdin?.end();
        }
    });
}