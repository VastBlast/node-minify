import nodeminify from '../generated-bindings';
import type { MinifyOptions } from './types';
export type * from './types';

export function minify(data: string, opts: MinifyOptions): Promise<any> {
    return new Promise((resolve, reject) => {
        nodeminify.minifyStringAsync(data, opts, (error: any, res: any) => {
            if (error || res.error) {
                reject(new Error(error || res.error));
            } else {
                resolve(res.data);
            }
        });
    });
}
