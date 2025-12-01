#ifndef NAPI_EXPERIMENTAL
#define NAPI_EXPERIMENTAL
#endif
#include <napi.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <assert.h>
#include <functional>
#include <memory>

#include "nodeminify.h"

using namespace Napi;
using namespace std;
// [common]++++++++++++++++++++++++++++++++++++++ start
//---------- genWgAddonArg ----------
typedef struct {
  int type; // [1]char [2]int [3]float [4]double [5]bool
  size_t len;
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
// ------------- genFreeCString -----------
static inline void wg_free_cstring(const char* wg_str) {
  if (wg_str != NULL) {
    FreeCString((char*)wg_str);
  }
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
  if (status == napi_ok) return;
  if (status == napi_pending_exception) {
    // Let the pending JS exception propagate without aborting the process.
    return;
  }
  const napi_extended_error_info* error_info = NULL;
  const char* wg_err_msg = "napi error";
  if (env != NULL && napi_get_last_error_info(env, &error_info) == napi_ok && error_info != NULL && error_info->error_message != NULL) {
    wg_err_msg = error_info->error_message;
  }
  if (env != NULL) {
    napi_throw_error(env, NULL, wg_err_msg);
  }
  fprintf(stderr, "addon >>>>> %s\n", wg_err_msg);
}

// ------------- genCatchErrBg -----------
static void wg_catch_err_bg(napi_status status, const char* label) {
  if (status == napi_ok) return;
  const char* wg_err_msg = label != NULL ? label : "napi async error";
  fprintf(stderr, "addon async >>>>> %s (status=%d)\n", wg_err_msg, status);
}
// [common]++++++++++++++++++++++++++++++++++++++ end
// ---------- GenCode ---------- 
Value _MinifyString(const CallbackInfo& wg_info) {
  Env wg_env = wg_info.Env();
#ifdef NAPI_CPP_EXCEPTIONS
  try {
#endif
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
  unique_ptr<char[]> wg_data_buf(new char[wg_data.length() + 1]);
  strcpy(wg_data_buf.get(), wg_data.c_str());
  char *data = wg_data_buf.get();
  Object wg__options = Object::New(wg_env);
  if (wg_info.Length() > 1) {
    wg__options = wg_info[1].As<Object>();
  }
  string wg_options = wg_object_to_string(wg__options, wg_env);
  unique_ptr<char[]> wg_options_buf(new char[wg_options.length() + 1]);
  strcpy(wg_options_buf.get(), wg_options.c_str());
  char *options = wg_options_buf.get();
  const char* wg_raw_res_ = MinifyString(data,options);
  string wg_res_ = wg_raw_res_ ? wg_raw_res_ : "";
  wg_free_cstring(wg_raw_res_);
  Object wg_obj_ = wg_string_to_object(wg_res_, wg_env);
  return wg_obj_;
#ifdef NAPI_CPP_EXCEPTIONS
  } catch (const Error& wg_ex) {
    wg_ex.ThrowAsJavaScriptException();
    return wg_env.Null();
  } catch (const std::exception& wg_ex) {
    napi_throw_error(wg_env, NULL, wg_ex.what());
    return wg_env.Null();
  } catch (...) {
    napi_throw_error(wg_env, NULL, "native exception");
    return wg_env.Null();
  }
#endif
}
// [MinifyStringAsync] +++++++++++++++++++++++++++++++++ start
typedef struct{
  napi_async_work work;
  napi_threadsafe_function tsfn;
  int argc;
  WgAddonArgInfo *args[3];
} WgAddonDataMinifyStringAsync;

typedef struct{
  bool is_error;
  void* data;
  char* err;
} WgAsyncResultMinifyStringAsync;
// ------------ genJsCallbackCode
static void wg_js_callback_minifystringasync(napi_env wg_env, napi_value wg_js_cb, void* wg_context, void* wg_data) {
  (void)wg_context;
#ifdef NAPI_CPP_EXCEPTIONS
  try {
#endif
  WgAsyncResultMinifyStringAsync* wg_async_res = (WgAsyncResultMinifyStringAsync*)wg_data;
  bool wg_is_error = wg_async_res != NULL && wg_async_res->is_error;
  const char* wg_err_msg = (wg_async_res != NULL) ? wg_async_res->err : NULL;
  std::string wg_err_str = wg_err_msg != NULL ? std::string(wg_err_msg) : std::string();
  string wg_res_ = "";
  if (!wg_is_error && wg_async_res != NULL) {
    const char* wg_raw_res_ = static_cast<const char*>(wg_async_res->data);
    wg_res_ = wg_raw_res_ ? wg_raw_res_ : "";
    if (wg_raw_res_ != NULL) {
      free((void*)wg_raw_res_);
    }
  }
  if (wg_async_res != NULL && wg_async_res->err != NULL) {
    free((void*)wg_async_res->err);
  }
  if (wg_async_res != NULL) {
    free(wg_async_res);
  }
  if (wg_env != NULL) {
    Env wg_env_scope = Env(wg_env);
    napi_value wg_err = wg_env_scope.Null();
    if (wg_is_error) {
      wg_err = Error::New(wg_env_scope, !wg_err_str.empty() ? wg_err_str.c_str() : "async error").Value();
    }
    napi_value wg_result = wg_env_scope.Null();
    if (!wg_is_error) {
      Object wg_obj = wg_string_to_object(wg_res_, wg_env_scope);
      wg_result = wg_obj;
    }
    napi_value wg_argv[] = { wg_err, wg_result };
    napi_value wg_global;
    napi_status wg_sts = napi_get_global(wg_env, &wg_global);
    if (wg_sts != napi_ok) {
      wg_catch_err(wg_env, wg_sts);
      return;
    }
    wg_sts = napi_call_function(wg_env, wg_global, wg_js_cb, 2, wg_argv, NULL);
    if (wg_sts != napi_ok) {
      wg_catch_err(wg_env, wg_sts);
      return;
    }
  }
#ifdef NAPI_CPP_EXCEPTIONS
  } catch (const Error& wg_ex) {
    wg_ex.ThrowAsJavaScriptException();
  } catch (const std::exception& wg_ex) {
    napi_throw_error(wg_env, NULL, wg_ex.what());
  } catch (...) {
    napi_throw_error(wg_env, NULL, "native exception");
  }
#endif
}
// -------- genExecuteworkCode
static void wg_execute_workminifystringasync(napi_env wg_env, void* wg_data) {
  WgAddonDataMinifyStringAsync* wg_addon = (WgAddonDataMinifyStringAsync*)wg_data;
  void* wg_worker_allocs[3] = {0};
  bool wg_tsfn_acquired = false;
  auto wg_send_async_error = [&](const char* wg_msg_) {
    WgAsyncResultMinifyStringAsync* wg_async_res_err = (WgAsyncResultMinifyStringAsync*)malloc(sizeof(*wg_async_res_err));
    if (wg_async_res_err == NULL) {
      wg_catch_err_bg(napi_generic_failure, "alloc async error result");
      return;
    }
    wg_async_res_err->is_error = true;
    wg_async_res_err->data = NULL;
    wg_async_res_err->err = NULL;
    if (wg_msg_ != NULL) {
      size_t wg_err_len_ = strlen(wg_msg_);
      wg_async_res_err->err = (char*)malloc(wg_err_len_ + 1);
      if (wg_async_res_err->err != NULL) {
        memcpy(wg_async_res_err->err, wg_msg_, wg_err_len_);
        wg_async_res_err->err[wg_err_len_] = '\0';
      }
    }
    napi_status wg_sts_err = napi_call_threadsafe_function(wg_addon->tsfn, (void*)(wg_async_res_err), napi_tsfn_blocking);
    if (wg_sts_err != napi_ok) {
      if (wg_async_res_err->err != NULL) {
        free((void*)wg_async_res_err->err);
      }
      free(wg_async_res_err);
      wg_catch_err_bg(wg_sts_err, "call threadsafe function");
    }
  };
  auto wg_cleanup = [&]() {
    if (wg_tsfn_acquired) {
      napi_status wg_rel_sts = napi_release_threadsafe_function(wg_addon->tsfn, napi_tsfn_release);
      wg_catch_err_bg(wg_rel_sts, "release threadsafe function");
      wg_tsfn_acquired = false;
    }
    for (size_t i = 0; i < 3; i++) {
      if (wg_worker_allocs[i] != NULL) {
        delete [] (char*)wg_worker_allocs[i];
        wg_worker_allocs[i] = NULL;
      }
    }
  };
  napi_status wg_sts = napi_acquire_threadsafe_function(wg_addon->tsfn);
  if (wg_sts != napi_ok) {
    wg_catch_err_bg(wg_sts, "acquire threadsafe function");
    wg_send_async_error("acquire threadsafe function");
    wg_cleanup();
    return;
  }
  wg_tsfn_acquired = true;
  void* wg_res_ = NULL;
#ifdef NAPI_CPP_EXCEPTIONS
  try {
#endif
  // arg - data
  WgAddonArgInfo * wg_dataInfo = wg_addon->args[0];
  char * data = new char[wg_dataInfo->len];
  strcpy(data, (char *)wg_dataInfo->value);
  wg_worker_allocs[0] = data;
  // ---- 
  WgAddonArgInfo * wg_optionsInfo = wg_addon->args[1];
  char *options = new char[wg_optionsInfo->len];
  strcpy(options, (char *)wg_optionsInfo->value);
  wg_worker_allocs[1] = options;
  // arg - cb
  WgAddonArgInfo * wg_cbInfo = wg_addon->args[2];
  char * cb = new char[wg_cbInfo->len];
  strcpy(cb, (char *)wg_cbInfo->value);
  wg_worker_allocs[2] = cb;
  // ---- 
  // -------- genHandlerCode
  WgAsyncResultMinifyStringAsync* wg_async_res_success = NULL;
  const char* wg_src_res_ = MinifyStringAsync(data,options,cb);
  size_t wg_src_len_ = wg_src_res_ ? strlen(wg_src_res_) : 0;
  char* wg_res_buf_ = (char*)malloc(wg_src_len_ + 1);
  if (wg_res_buf_ == NULL) {
    wg_free_cstring(wg_src_res_);
    wg_send_async_error("alloc async object result");
    goto wg_execute_workminifystringasync_cleanup;
  }
  if (wg_src_len_ > 0) {
    memcpy(wg_res_buf_, wg_src_res_, wg_src_len_);
  }
  wg_res_buf_[wg_src_len_] = '\0';
  wg_free_cstring(wg_src_res_);
  wg_async_res_success = (WgAsyncResultMinifyStringAsync*)malloc(sizeof(*wg_async_res_success));
  if (wg_async_res_success == NULL) {
    free(wg_res_buf_);
    wg_send_async_error("alloc async object result wrapper");
    goto wg_execute_workminifystringasync_cleanup;
  }
  wg_async_res_success->is_error = false;
  wg_async_res_success->data = (void*)wg_res_buf_;
  wg_async_res_success->err = NULL;
  wg_res_ = (void*)wg_async_res_success;
  if (wg_res_ != NULL) {
    wg_sts = napi_call_threadsafe_function(wg_addon->tsfn, (void*)(wg_res_), napi_tsfn_blocking);
    if (wg_sts != napi_ok) {
      wg_catch_err_bg(wg_sts, "call threadsafe function");
    if (wg_res_ != NULL) {
            WgAsyncResultMinifyStringAsync* wg_async_res_free = (WgAsyncResultMinifyStringAsync*)wg_res_;
      if (wg_async_res_free != NULL) {
      if (wg_async_res_free->data != NULL) {
        free(wg_async_res_free->data);
      }
        if (wg_async_res_free->err != NULL) {
          free(wg_async_res_free->err);
        }
        free(wg_async_res_free);
      }
    }
      goto wg_execute_workminifystringasync_cleanup;
    }
  }
wg_execute_workminifystringasync_cleanup:
  wg_cleanup();
#ifdef NAPI_CPP_EXCEPTIONS
  } catch (const std::exception& wg_ex) {
    wg_send_async_error(wg_ex.what());
    wg_cleanup();
  } catch (...) {
    wg_send_async_error("native exception");
    wg_cleanup();
  }
#endif
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
  const size_t wg_expected_argc = 3;
  size_t wg_argc = wg_expected_argc;
  size_t wg_cb_arg_index = 2;
  napi_value wg_args[3] = {0};
  napi_value wg_work_name;
  napi_status wg_sts;
  WgAddonDataMinifyStringAsync* wg_addon = (WgAddonDataMinifyStringAsync*)malloc(sizeof(*wg_addon));
  if (wg_addon == NULL) {
    napi_throw_error(wg_env, NULL, "alloc async data");
    return NULL;
  }
  wg_addon->work = NULL;
  wg_addon->argc = wg_expected_argc;
  for (size_t i = 0; i < wg_expected_argc; i++) {
    wg_addon->args[i] = NULL;
  }
  napi_value wg_undefined;
  wg_catch_err(wg_env, napi_get_undefined(wg_env, &wg_undefined));
  auto wg_cleanup = [&]() {
    for (size_t i = 0; i < wg_expected_argc; i++) {
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
  };
#ifdef NAPI_CPP_EXCEPTIONS
  try {
#endif
  wg_sts = napi_get_undefined(wg_env, &wg_undefined);
  if (wg_sts != napi_ok) {
    wg_catch_err(wg_env, wg_sts);
    wg_cleanup();
    return NULL;
  }
  wg_sts = napi_get_cb_info(wg_env, wg_info, &wg_argc, wg_args, NULL, NULL);
  if (wg_sts != napi_ok) {
    wg_catch_err(wg_env, wg_sts);
    wg_cleanup();
    return NULL;
  }
  for (size_t i = wg_argc; i < wg_expected_argc; i++) {
    wg_args[i] = wg_undefined;
  }
  napi_value wg_js_cb = wg_args[wg_cb_arg_index];
  Value wg_v0 = Value(wg_env, wg_args[0]);
  if(wg_v0.IsUndefined()){
    TypeError::New(wg_env, "The 1 parameter must be passed!").ThrowAsJavaScriptException();
    wg_cleanup();
    return NULL;
  }
  if(!wg_v0.IsUndefined() && !wg_v0.IsString()){
    TypeError::New(wg_env, "The 1 parameter must be of string!").ThrowAsJavaScriptException();
    wg_cleanup();
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
  if (wg_addon->args[0] == NULL) {
    delete [] data;
    napi_throw_error(wg_env, NULL, "alloc async arg info");
    wg_cleanup();
    return NULL;
  }
  wg_addon->args[0]->type=1;
  wg_addon->args[0]->len=wg_data.length() + 1;
  wg_addon->args[0]->value=(void *)data;
  // ---- 
  Value wg_v1 = Value(wg_env, wg_args[1]);
  if(wg_v1.IsUndefined()){
    TypeError::New(wg_env, "The 2 parameter must be passed!").ThrowAsJavaScriptException();
    wg_cleanup();
    return NULL;
  }
  if(!wg_v1.IsUndefined() && !wg_v1.IsObject()){
    TypeError::New(wg_env, "The 2 parameter must be of object!").ThrowAsJavaScriptException();
    wg_cleanup();
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
  if (wg_addon->args[1] == NULL) {
    delete [] options;
    napi_throw_error(wg_env, NULL, "alloc async arg info");
    wg_cleanup();
    return NULL;
  }
  wg_addon->args[1]->type=1;
  wg_addon->args[1]->len=wg_options.length() + 1;
  wg_addon->args[1]->value=(void *)options;
  // ---- 
  Value wg_v2 = Value(wg_env, wg_args[2]);
  if(wg_v2.IsUndefined()){
    TypeError::New(wg_env, "The 3 parameter must be passed!").ThrowAsJavaScriptException();
    wg_cleanup();
    return NULL;
  }
  if(!wg_v2.IsUndefined() && !wg_v2.IsFunction()){
    TypeError::New(wg_env, "The 3 parameter must be of function!").ThrowAsJavaScriptException();
    wg_cleanup();
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
  if (wg_addon->args[2] == NULL) {
    delete [] cb;
    napi_throw_error(wg_env, NULL, "alloc async arg info");
    wg_cleanup();
    return NULL;
  }
  wg_addon->args[2]->type=1;
  wg_addon->args[2]->len=wg__cb.length() + 1;
  wg_addon->args[2]->value=(void *)cb;
  // ---- 
  assert(wg_addon->work == NULL && "Only one work item must exist at a time");
  wg_sts = napi_create_string_utf8(wg_env, "N-API Thread-safe Call from Async Work Item", NAPI_AUTO_LENGTH, &wg_work_name);
  if (wg_sts != napi_ok) {
    wg_catch_err(wg_env, wg_sts);
    wg_cleanup();
    return NULL;
  }
  wg_sts = napi_create_threadsafe_function(wg_env, wg_js_cb, NULL, wg_work_name, 0, 1, NULL, NULL, NULL, wg_js_callback_minifystringasync, &(wg_addon->tsfn));
  if (wg_sts != napi_ok) {
    wg_catch_err(wg_env, wg_sts);
    wg_cleanup();
    return NULL;
  }
  wg_sts = napi_create_async_work(wg_env, NULL, wg_work_name, wg_execute_workminifystringasync, wg_work_complete_minifystringasync, wg_addon, &(wg_addon->work));
  if (wg_sts != napi_ok) {
    wg_catch_err(wg_env, wg_sts);
    napi_release_threadsafe_function(wg_addon->tsfn, napi_tsfn_abort);
    wg_cleanup();
    return NULL;
  }
  wg_sts = napi_queue_async_work(wg_env, wg_addon->work);
  if (wg_sts != napi_ok) {
    wg_catch_err(wg_env, wg_sts);
    napi_release_threadsafe_function(wg_addon->tsfn, napi_tsfn_abort);
    wg_catch_err(wg_env, napi_delete_async_work(wg_env, wg_addon->work));
    wg_cleanup();
    return NULL;
  }
  return NULL;
#ifdef NAPI_CPP_EXCEPTIONS
  } catch (const Error& wg_ex) {
    wg_ex.ThrowAsJavaScriptException();
  } catch (const std::exception& wg_ex) {
    napi_throw_error(wg_env, NULL, wg_ex.what());
  } catch (...) {
    napi_throw_error(wg_env, NULL, "native exception");
  }
  wg_cleanup();
  return NULL;
#endif
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