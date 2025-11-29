const path = require('path');
const { platformIdentifier } = require('./platform');

const buildId = platformIdentifier();
const addonPath = path.join(__dirname, 'prebuilds', buildId, 'nodeminify.node');
const addon = require(addonPath);

// JS call API
module.exports = { 
  minifyString : addon.minify_string,
  minifyStringAsync : addon.minify_string_async
};
