// Mirrors the Go-side minifyOptions struct in minify.go.
export type MinifyMediaType =
  | 'text/css'
  | 'text/html'
  | 'image/svg+xml'
  | 'application/javascript'
  | 'application/json'
  | 'application/rss+xml'
  | 'application/manifest+json'
  | 'application/xhtml-xml'
  | 'text/xml'
  | 'importmap';

export interface MinifyOptions {
  data?: string;
  type: MinifyMediaType;
  cssPrecision?: number;
  htmlKeepComments?: boolean;
  htmlKeepDefaultAttrvals?: boolean;
  htmlKeepDocumentTags?: boolean;
  htmlKeepEndTags?: boolean;
  htmlKeepQuotes?: boolean;
  htmlKeepSpecialComments?: boolean;
  htmlKeepWhitespace?: boolean;
  jsKeepVarNames?: boolean;
  jsPrecision?: number;
  jsVersion?: number;
  jsonKeepNumbers?: boolean;
  jsonPrecision?: number;
  svgKeepComments?: boolean;
  svgPrecision?: number;
  xmlKeepWhitespace?: boolean;
  url?: string;
}
