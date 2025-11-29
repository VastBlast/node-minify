const nodeminify = require('bindings')('nodeminify');
// JS call API
module.exports = { 
  minifyString : nodeminify.minify_string,
  minifyStringAsync : nodeminify.minify_string_async
};
