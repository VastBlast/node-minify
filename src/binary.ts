import path from 'path';
import fs from 'fs';
import os from 'os';
import { binPath } from './constants';


function filterNoEmpty(arr: string[], fn: (x: string) => boolean) {
    const filtered = arr.filter(fn);
    if (filtered.length == 0) return arr;
    return filtered;
}

function detectPlatformFolder(): string {
    const platform = os.platform();
    const arch = os.arch();

    // get folders
    let folders = fs.readdirSync(binPath);

    folders = filterNoEmpty(folders, x => x.includes(platform));

    if (platform == 'win32') {
        folders = filterNoEmpty(folders, x => x.includes('windows'));
        folders = filterNoEmpty(folders, x => x.includes('.exe'));
    }

    folders = filterNoEmpty(folders, x => x.includes(arch));

    if (arch == 'x64') {
        folders = filterNoEmpty(folders, x => x.includes('amd64'));
        folders = filterNoEmpty(folders, x => x.includes('x86_64'));
        folders = filterNoEmpty(folders, x => x.includes('64'));
    } else if (arch == 'x32') {
        folders = filterNoEmpty(folders, x => x.includes('386'));
        folders = filterNoEmpty(folders, x => x.includes('x86'));
        folders = filterNoEmpty(folders, x => x.includes('32'));
    } else if (arch == 'arm64') {
        folders = filterNoEmpty(folders, x => x.includes('arm64'));
        folders = filterNoEmpty(folders, x => x.includes('64'));
    } else if (arch == 'arm') {
        folders = filterNoEmpty(folders, x => x.includes('armv7'));
    }

    const file = folders[0];

    if (!file) throw new Error(`Unsupported platform: ${platform} ${arch}`);

    return file;
}


/**
 * Get the file path for the appropriate minify binary based on the current platform and architecture.
 * @returns The file path to the minify binary.
 * @throws Error if the platform or architecture is not supported.
 */
export function getBinaryFilePath() {
    const folder = detectPlatformFolder();

    let binaryName = 'minify';
    if (os.platform() === 'win32') binaryName += '.exe';

    const binaryPath = path.join(binPath, folder, binaryName);

    if (!fs.existsSync(binaryPath)) throw new Error(`Binary not found ${binaryName} for platform ${folder}`);

    if (os.platform() !== 'win32') fs.chmodSync(binaryPath, 0o755);

    return binaryPath;
}