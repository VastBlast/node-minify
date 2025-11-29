#define NAPI_EXPERIMENTAL
#include <napi.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <assert.h>
#include <functional>

#include "nodeminify.h"

using namespace Napi;
using namespace std;
// [common]++++++++++++++++++++++++++++++++++++++ start
//---------- genWgAddonArg ----------
typedef struct {
  int type; // [1]char [2]int [3]float [4]double [5]bool
  int len;
  void* value;
} WgAddonArgInfo;
//---------- genBuildGoString ----------
GoString wg_build_go_string(const char* p, size_t n){
  return {p, static_cast<ptrdiff_t>(n)};
}
//---------- genBuildGoString ----------
GoSlice wg_build_go_slice(void *data, int len, int cap){
  return { data, len, cap };
}
// ------------- genStringToArray2 -----------
string wg_array_to_string(Array arr, Env env) {
  Value wg_json_val = env.Global().Get("JSON");
  if (!wg_json_val.IsObject()) return "[]";
  Object wg_json = wg_json_val.As<Object>();
  Value wg_stringify_val = wg_json.Get("stringify");
  if (!wg_stringify_val.IsFunction()) return "[]";
  Function wg_stringify = wg_stringify_val.As<Function>();
  Value wg_res = wg_stringify.Call(wg_json, { arr });
  if (wg_res.IsString()) {
    return wg_res.As<String>().Utf8Value();
  }
  return "[]";
}
// ------------- genStringToArray -----------
Array wg_string_to_array(string str, Env env) {
  if (str == "") return Array::New(env);
  Value wg_json_val = env.Global().Get("JSON");
  if (!wg_json_val.IsObject()) return Array::New(env);
  Object wg_json = wg_json_val.As<Object>();
  Value wg_parse_val = wg_json.Get("parse");
  if (!wg_parse_val.IsFunction()) return Array::New(env);
  Function wg_parse = wg_parse_val.As<Function>();
  Value wg_res = wg_parse.Call(wg_json, { String::New(env, str) });
  if (wg_res.IsArray()) {
    return wg_res.As<Array>();
  }
  return Array::New(env);
}
// ------------- genObjectToString -----------
string wg_object_to_string(Object objs, Env env) {
  Value wg_json_val = env.Global().Get("JSON");
  if (!wg_json_val.IsObject()) return "{}";
  Object wg_json = wg_json_val.As<Object>();
  Value wg_stringify_val = wg_json.Get("stringify");
  if (!wg_stringify_val.IsFunction()) return "{}";
  Function wg_stringify = wg_stringify_val.As<Function>();
  Value wg_res = wg_stringify.Call(wg_json, { objs });
  if (wg_res.IsString()) {
    return wg_res.As<String>().Utf8Value();
  }
  return "{}";
}
// ------------- genStringToObject -----------
 Object wg_string_to_object(string str, Env env) {
  if (str == "") return Object::New(env);
  Value wg_json_val = env.Global().Get("JSON");
  if (!wg_json_val.IsObject()) return Object::New(env);
  Object wg_json = wg_json_val.As<Object>();
  Value wg_parse_val = wg_json.Get("parse");
  if (!wg_parse_val.IsFunction()) return Object::New(env);
  Function wg_parse = wg_parse_val.As<Function>();
  Value wg_res = wg_parse.Call(wg_json, { String::New(env, str) });
  if (wg_res.IsObject()) {
    return wg_res.As<Object>();
  }
  return Object::New(env);
}
// ------------- genCatchErr -----------
static void wg_catch_err(napi_env env, napi_status status) {
  if (status != napi_ok) {
    const napi_extended_error_info* error_info = NULL;
    napi_get_last_error_info(env, &error_info);
    printf("addon >>>>> %s\n", error_info->error_message);
    exit(0);
  }
}
// [common]++++++++++++++++++++++++++++++++++++++ end
// ---------- GenCode ---------- 
Value _MinifyString(const CallbackInfo& wg_info) {
  Env wg_env = wg_info.Env();
  if(wg_info.Length() < 1){
    TypeError::New(wg_env, "The 1 parameter must be passed!").ThrowAsJavaScriptException();
    return wg_env.Null();
  }
  if(wg_info.Length() < 2){
    TypeError::New(wg_env, "The 2 parameter must be passed!").ThrowAsJavaScriptException();
    return wg_env.Null();
  }
  if(wg_info.Length() > 0 && !wg_info[0].IsString()){
    TypeError::New(wg_env, "The 1 parameter must be of string!").ThrowAsJavaScriptException();
    return wg_env.Null();
  }
  if(wg_info.Length() > 1 && !wg_info[1].IsObject()){
    TypeError::New(wg_env, "The 2 parameter must be of object!").ThrowAsJavaScriptException();
    return wg_env.Null();
  }
  string wg_data = "";
  if (wg_info.Length() > 0) {
    wg_data = wg_info[0].As<String>().Utf8Value();
  }
  char *data = new char[wg_data.length() + 1];
  strcpy(data, wg_data.c_str());
  Object wg__options = Object::New(wg_env);
  if (wg_info.Length() > 1) {
    wg__options = wg_info[1].As<Object>();
  }
  string wg_options = wg_object_to_string(wg__options, wg_env);
  char *options = new char[wg_options.length() + 1];
  strcpy(options, wg_options.c_str());
  string wg_res_ = MinifyString(data,options);
  Object wg_obj_ = wg_string_to_object(wg_res_, wg_env);
  delete [] data;
  delete [] options;
  return wg_obj_;
}
// [MinifyStringAsync] +++++++++++++++++++++++++++++++++ start
typedef struct{
  napi_async_work work;
  napi_threadsafe_function tsfn;
  int argc;
  WgAddonArgInfo *args[3];
} WgAddonDataMinifyStringAsync;
// ------------ genJsCallbackCode
static void wg_js_callback_minifystringasync(napi_env wg_env, napi_value wg_js_cb, void* wg_context, void* wg_data) {
  (void)wg_context;
  const string wg_res_ = (char*)wg_data;
  if (wg_env != NULL) {
    Object wg_obj = wg_string_to_object(wg_res_, wg_env);
    napi_value wg_argv[] = { wg_obj };
    napi_value wg_global;
	napi_get_global(wg_env, &wg_global);
    wg_catch_err(wg_env, napi_call_function(wg_env, wg_global, wg_js_cb, 1, wg_argv, NULL));
  }
}
// -------- genExecuteworkCode
static void wg_execute_workminifystringasync(napi_env wg_env, void* wg_data) {
  WgAddonDataMinifyStringAsync* wg_addon = (WgAddonDataMinifyStringAsync*)wg_data;
  wg_catch_err(wg_env, napi_acquire_threadsafe_function(wg_addon->tsfn));
  // arg - data
  WgAddonArgInfo * wg_dataInfo = wg_addon->args[0];
  char * data = new char[wg_dataInfo->len];
  strcpy(data, (char *)wg_dataInfo->value);
  // ---- 
  WgAddonArgInfo * wg_optionsInfo = wg_addon->args[1];
  char *options = new char[wg_optionsInfo->len];
  strcpy(options, (char *)wg_optionsInfo->value);
  // arg - cb
  WgAddonArgInfo * wg_cbInfo = wg_addon->args[2];
  char * cb = new char[wg_cbInfo->len];
  strcpy(cb, (char *)wg_cbInfo->value);
  // ---- 
  // -------- genHandlerCode
  const char* wg_res_ = MinifyStringAsync(data,options,cb);
  wg_catch_err(wg_env, napi_call_threadsafe_function(wg_addon->tsfn, (void*)(wg_res_), napi_tsfn_blocking));
  wg_catch_err(wg_env, napi_release_threadsafe_function(wg_addon->tsfn, napi_tsfn_release));
  delete [] data;  delete [] options;
  delete [] cb;
}
// -------- genworkThreadCompleteCode
static void wg_work_complete_minifystringasync(napi_env wg_env, napi_status wg_status, void* wg_data) {
  WgAddonDataMinifyStringAsync* wg_addon = (WgAddonDataMinifyStringAsync*)wg_data;
  wg_catch_err(wg_env, napi_release_threadsafe_function(wg_addon->tsfn, napi_tsfn_release));
  wg_catch_err(wg_env, napi_delete_async_work(wg_env, wg_addon->work));
  wg_addon->work = NULL;
  wg_addon->tsfn = NULL;
  for (int i = 0; i < wg_addon->argc; i++) {
    if (wg_addon->args[i] != NULL && wg_addon->args[i]->type == 1) {
      WgAddonArgInfo* info = (WgAddonArgInfo*)wg_addon->args[i];
      delete [] (char *)info->value;
    }
    if (wg_addon->args[i] != NULL) {
      free(wg_addon->args[i]);
      wg_addon->args[i] = NULL;
    }
  }
  free(wg_addon);
}
// ---------- genworkThreadCode
static napi_value wg_work_minifystringasync(napi_env wg_env, napi_callback_info wg_info) {
  size_t wg_argc = 3;
  size_t wg_cb_arg_index = 2;
  napi_value wg_args[3];
  napi_value wg_work_name;
  napi_status wg_sts;
  WgAddonDataMinifyStringAsync* wg_addon = (WgAddonDataMinifyStringAsync*)malloc(sizeof(*wg_addon));
  wg_addon->work = NULL;
  wg_addon->argc = wg_argc;
  for (size_t i = 0; i < wg_argc; i++) {
    wg_addon->args[i] = NULL;
  }
  wg_sts = napi_get_cb_info(wg_env, wg_info, &wg_argc, wg_args, NULL, NULL);
  wg_catch_err(wg_env, wg_sts);
  napi_value wg_js_cb = wg_args[wg_cb_arg_index];
  Value wg_v0 = Value(wg_env, wg_args[0]);
  if(wg_v0.IsUndefined()){
    TypeError::New(wg_env, "The 1 parameter must be passed!").ThrowAsJavaScriptException();
    return NULL;
  }
  if(!wg_v0.IsUndefined() && !wg_v0.IsString()){
    TypeError::New(wg_env, "The 1 parameter must be of string!").ThrowAsJavaScriptException();
    return NULL;
  }
  // arg - data
  string wg_data = "";
  if(!wg_v0.IsUndefined() && wg_v0.IsString()){
    wg_data = wg_v0.As<String>().Utf8Value();
  }
  char *data = new char[wg_data.length() + 1];
  strcpy(data, wg_data.c_str());
  wg_addon->args[0] = (WgAddonArgInfo*)malloc(sizeof(*wg_addon->args[0]));
  wg_addon->args[0]->type=1;
  wg_addon->args[0]->len=wg_data.length() + 1;
  wg_addon->args[0]->value=(void *)data;
  // ---- 
  Value wg_v1 = Value(wg_env, wg_args[1]);
  if(wg_v1.IsUndefined()){
    TypeError::New(wg_env, "The 2 parameter must be passed!").ThrowAsJavaScriptException();
    return NULL;
  }
  if(!wg_v1.IsUndefined() && !wg_v1.IsObject()){
    TypeError::New(wg_env, "The 2 parameter must be of object!").ThrowAsJavaScriptException();
    return NULL;
  }
  // arg - options
  Object wg__options = Object::New(wg_env);
  if(!wg_v1.IsUndefined() && wg_v1.IsObject()){
    wg__options = wg_v1.As<Object>();
  }
  string wg_options = wg_object_to_string(wg__options, wg_env);
  char *options = new char[wg_options.length() + 1];
  strcpy(options, wg_options.c_str());
  wg_addon->args[1] = (WgAddonArgInfo*)malloc(sizeof(*wg_addon->args[1]));
  wg_addon->args[1]->type=1;
  wg_addon->args[1]->len=wg_options.length() + 1;
  wg_addon->args[1]->value=(void *)options;
  // ---- 
  Value wg_v2 = Value(wg_env, wg_args[2]);
  if(wg_v2.IsUndefined()){
    TypeError::New(wg_env, "The 3 parameter must be passed!").ThrowAsJavaScriptException();
    return NULL;
  }
  if(!wg_v2.IsUndefined() && !wg_v2.IsFunction()){
    TypeError::New(wg_env, "The 3 parameter must be of function!").ThrowAsJavaScriptException();
    return NULL;
  }
  // arg - cb
  string wg__cb = "cb(){}";
  if(!wg_v2.IsUndefined() && wg_v2.IsFunction()){
    wg__cb = wg_v2.ToString();
    size_t pos = wg__cb.find("{");
    if (pos > 0) {
      wg__cb = wg__cb.substr(0, pos);
      wg__cb += "{}";
    }
  }
  char * cb = new char[wg__cb.length() + 1];
  strcpy(cb, wg__cb.c_str());
  wg_addon->args[2] = (WgAddonArgInfo*)malloc(sizeof(*wg_addon->args[2]));
  wg_addon->args[2]->type=1;
  wg_addon->args[2]->len=wg__cb.length() + 1;
  wg_addon->args[2]->value=(void *)cb;
  // ---- 
  assert(wg_addon->work == NULL && "Only one work item must exist at a time");
  wg_catch_err(wg_env, napi_create_string_utf8(wg_env, "N-API Thread-safe Call from Async Work Item", NAPI_AUTO_LENGTH, &wg_work_name));
  wg_sts = napi_create_threadsafe_function(wg_env, wg_js_cb, NULL, wg_work_name, 0, 1, NULL, NULL, NULL, wg_js_callback_minifystringasync, &(wg_addon->tsfn));
  wg_catch_err(wg_env, wg_sts);
  wg_sts = napi_create_async_work(wg_env, NULL, wg_work_name, wg_execute_workminifystringasync, wg_work_complete_minifystringasync, wg_addon, &(wg_addon->work));
  wg_catch_err(wg_env, wg_sts);
  wg_sts = napi_queue_async_work(wg_env, wg_addon->work);
  wg_catch_err(wg_env, wg_sts);
  return NULL;
}
// ---------- GenRegisterAsyncCode ---------- 
void register_minifystringasync(Env env, Object exports){
  napi_property_descriptor desc = {"minify_string_async",NULL,wg_work_minifystringasync,NULL,NULL,NULL,napi_default,NULL};
  napi_define_properties(env, exports, 1, &desc);
}
// [MinifyStringAsync]+++++++++++++++++++++++++++++++++ end
Object Init(Env env, Object exports) {
  exports.Set(String::New(env, "minify_string"), Function::New(env, _MinifyString));
  register_minifystringasync(env, exports);
  return exports;
}

NODE_API_MODULE(nodeminify, Init)