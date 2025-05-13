import path from 'path';
import fs from 'fs';
import { x as extractTar } from 'tar';
import zlib from 'zlib';
import { Readable } from 'stream';
import { pipeline } from 'stream/promises';
import yauzl from 'yauzl-promise';
import { binPath, binVersion } from './src/constants';

async function tarBufferExtract(tarBuffer: Buffer, extractDir: string) {
    // Create a read stream from the buffer
    const bufferStream = new Readable();
    bufferStream.push(tarBuffer);
    bufferStream.push(null);

    // Pipe the buffer stream through the gunzip stream
    const gunzip = zlib.createGunzip();

    // Create a writable stream for the extracted files
    const extractStream = extractTar({ cwd: extractDir });

    // Use the pipeline utility to handle streams and error handling
    await pipeline(bufferStream, gunzip, extractStream);
}

async function downloadAllBinariesFromGH(ghrepo: string, saveDir: string) {
    const res = await fetch(`https://github.com/${ghrepo}/releases/download/v${binVersion}/checksums.txt`);
    if (!res.ok) throw new Error(`Failed to fetch checksums: ${res.status} ${res.statusText}`);
    const checksums = await res.text();
    let files = checksums.split('\n').map(x => x.trim()).map(x => x.split(' ')).map(x => x[x.length - 1]).filter(x => x.length > 0);

    console.log('files:', files);

    // delete saveDir if it exists
    if (fs.existsSync(saveDir)) {
        fs.rmSync(saveDir, { recursive: true, force: true });
    }

    fs.mkdirSync(saveDir, { recursive: true });

    for (const fileName of files) {
        console.log('Downloading:', fileName);
        const url = `https://github.com/${ghrepo}/releases/download/v${binVersion}/${fileName}`;
        const fileData: Buffer = Buffer.from(await (await fetch(url)).arrayBuffer());

        const resDir = path.join(saveDir, fileName.replace('.tar.gz', '')).replace('.zip', '');
        fs.mkdirSync(resDir, { recursive: true });
        if (fileName.endsWith('.tar.gz')) {
            await tarBufferExtract(fileData, resDir);
        } else if (fileName.endsWith('.zip')) {
            const zip = await yauzl.fromBuffer(fileData);
            for await (const entry of zip) {
                fs.mkdirSync(path.join(resDir, path.dirname(entry.filename)), { recursive: true });
                const fileStream = fs.createWriteStream(path.join(resDir, entry.filename));
                (await entry.openReadStream()).pipe(fileStream);
                await new Promise<void>((resolve, reject) => {
                    fileStream.on('finish', resolve);
                    fileStream.on('error', reject);
                });
            }
        }

        // go through resDir and delete anything *.md
        const files = fs.readdirSync(resDir);
        for (const file of files) {
            if (file.endsWith('.md')) fs.rmSync(path.join(resDir, file))
        }

        // if it doesn't contain minify or minify.exe, delete the folder
        if (!files.some(x => x.startsWith('minify'))) {
            fs.rmSync(resDir, { recursive: true, force: true });
            console.log('Deleted:', resDir);
        } else {
            console.log('Extracted:', resDir);
        }
    }
}

downloadAllBinariesFromGH('tdewolff/minify', binPath);