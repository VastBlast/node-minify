import nodeminify from '../bindings';
import type { MinifyOptions } from './types';
export type * from './types';

export function minify(data: string, opts: MinifyOptions): Promise<any> {
    return new Promise((resolve, reject) => {
        nodeminify.minifyStringAsync(data, opts, (res: any) => {
            if (res.error) {
                reject(new Error(res.error));
            } else {
                resolve(res.data);
            }
        });
    });
}
