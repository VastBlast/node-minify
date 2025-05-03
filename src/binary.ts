import path from 'path';
import fs from 'fs';
import os from 'os';

/**
 * Get the file path for the appropriate minify binary based on the current platform and architecture.
 * @returns The file path to the minify binary.
 * @throws Error if the platform or architecture is not supported.
 */
export function getBinaryFilePath(): string {
    const platform = os.platform();
    const arch = os.arch();

    let binaryName = `minify-${platform}-${arch}`;
    if (platform === 'win32') binaryName += '.exe';
    const binaryPath = path.join(__dirname, '..', 'bin', binaryName);

    if (!fs.existsSync(binaryPath)) throw new Error(`Binary not found ${binaryName} for platform ${platform} and architecture ${arch}`);

    if (platform !== 'win32') fs.chmodSync(binaryPath, 0o755);

    return binaryPath;
}