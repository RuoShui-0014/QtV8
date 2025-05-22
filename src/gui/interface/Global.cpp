#include "Global.h"

#include <v8.h>

#include <QFile>
#include <QRunnable>
#include <QString>
#include <QThreadPool>

#include "src/v8/V8Isolate.h"

namespace rs::binding {

Global::Global() {}
Global::~Global() {}

void ConstructorCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  isolate->ThrowException(v8::Exception::TypeError(
      v8::String::NewFromUtf8Literal(isolate, "Illegal constructor")));
}

void GlobalExposedConstructCallback(
    v8::Local<v8::Name> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  info.GetReturnValue().Set(isolate->GetCurrentContext()->Global());
}

void AtobOperatorCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  if (info.Length() < 1 || !info[0]->IsString()) {
    isolate->ThrowException(
        v8::Exception::TypeError(v8::String::NewFromUtf8Literal(
            isolate,
            "Failed to execute 'atob' on 'Global': 1 "
            "argument required, but only 0 present.")));
    return;
  }

  QByteArray byteArray =
      *v8::String::Utf8Value(isolate, info[0].As<v8::String>());
  QString result = QString::fromLatin1(QByteArray::fromBase64(byteArray));
  info.GetReturnValue().Set(
      v8::String::NewFromUtf8(isolate, result.toUtf8().data())
          .ToLocalChecked());
}

void BtoaOperatorCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  if (info.Length() < 1 || !info[0]->IsString()) {
    isolate->ThrowException(
        v8::Exception::TypeError(v8::String::NewFromUtf8Literal(
            isolate,
            "Failed to execute 'btoa' on 'Global': 1 "
            "argument required, but only 0 present.")));
    return;
  }

  QByteArray byteArray =
      *v8::String::Utf8Value(isolate, info[0].As<v8::String>());

  QString result = QString::fromUtf8(byteArray.toBase64());
  info.GetReturnValue().Set(
      v8::String::NewFromUtf8(isolate, result.toUtf8().data())
          .ToLocalChecked());
}

class PromiseTask : public v8::Task {
 public:
  PromiseTask(v8::Isolate* isolate,
              v8::Local<v8::Context> context,
              v8::Local<v8::Promise::Resolver> resolver,
              bool resolve = false,
              QString data = QString())
      : m_isolate{isolate},
        m_context{isolate, context},
        m_resolver{isolate, resolver},
        m_resolve{resolve},
        m_data(std::move(data)) {}
  ~PromiseTask() override = default;

  void Run() override {
    // v8::HandleScope handle_scope(m_isolate);
    v8::Local<v8::Context> context = m_context.Get(m_isolate);
    // v8::Context::Scope context_scope(context);

    if (m_resolve) {
      m_resolver.Get(m_isolate)->Resolve(
          context, v8::String::NewFromUtf8(m_isolate, m_data.toUtf8().data())
                       .ToLocalChecked());
    } else {
      m_resolver.Get(m_isolate)->Reject(
          context,
          v8::Exception::TypeError(
              v8::String::NewFromUtf8(m_isolate, "Failed to read file!")
                  .ToLocalChecked()));
    }
  }

 private:
  v8::Isolate* m_isolate;
  v8::Global<v8::Context> m_context;
  v8::Global<v8::Promise::Resolver> m_resolver;
  bool m_resolve = false;
  QString m_data;
};

class ReadFileTask : public QRunnable {
 public:
  explicit ReadFileTask(v8::Isolate* isolate,
                        v8::Local<v8::Context> context,
                        QString file_name,
                        v8::Local<v8::Promise::Resolver> resolver)
      : m_isolate{isolate},
        m_context{isolate, context},
        m_file_name(file_name),
        m_resolver{resolver->GetIsolate(), resolver} {
    setAutoDelete(true);
  }
  ~ReadFileTask() override = default;

  void run() override {
    QFile file(m_file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      std::unique_ptr<PromiseTask> task = std::make_unique<PromiseTask>(
          m_isolate, m_context.Get(m_isolate), m_resolver.Get(m_isolate));
      std::shared_ptr<v8::TaskRunner> task_runner =
          V8Initializer::GetInstance().GetTaskRunner(m_isolate);
      task_runner->PostTask(std::move(task));
      return;
    }
    QString data = file.readAll();
    file.close();

    std::unique_ptr<PromiseTask> task = std::make_unique<PromiseTask>(
        m_isolate, m_context.Get(m_isolate), m_resolver.Get(m_isolate), true,
        std::move(data));
    std::shared_ptr<v8::TaskRunner> task_runner =
        V8Initializer::GetInstance().GetTaskRunner(m_isolate);
    task_runner->PostTask(std::move(task));
  }

 private:
  QString m_file_name;
  v8::Isolate* m_isolate;
  v8::Global<v8::Context> m_context;
  v8::Global<v8::Promise::Resolver> m_resolver;
};

void ReadFileSyncOperatorCallback(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1 || !info[0]->IsString()) {
    return;
  }

  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Promise::Resolver> resolver =
      v8::Promise::Resolver::New(context).ToLocalChecked();
  info.GetReturnValue().Set(resolver);

  v8::String::Utf8Value data(isolate, info[0].As<v8::String>());

  ReadFileTask* task = new ReadFileTask(isolate, context, *data, resolver);
  QThreadPool::globalInstance()->start(task);
}

void LoadModuleOperatorCallback(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    return;
  }

  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::String> file_name = info[0].As<v8::String>();
  v8::Local<v8::String> code = info[1].As<v8::String>();
  v8::ScriptCompiler::Source source(
      code, v8::ScriptOrigin(file_name, 0, 0, false, -1, v8::Local<v8::Value>(),
                             false, false, true));
  v8::Local<v8::Module> module =
      v8::ScriptCompiler::CompileModule(isolate, &source).ToLocalChecked();
  if (module->InstantiateModule(context, nullptr).ToChecked()) {
    v8::MaybeLocal<v8::Value> maybe_result = module->Evaluate(context);
    if (!maybe_result.IsEmpty()) {
      info.GetReturnValue().Set(maybe_result.ToLocalChecked());
    }
  }
}

v8::Local<v8::Template> Global::GetInterface(v8::Isolate* isolate) {
  v8::Local<v8::FunctionTemplate> interface =
      v8::FunctionTemplate::New(isolate, ConstructorCallback);
  interface->SetClassName(v8::String::NewFromUtf8Literal(isolate, "Global"));
  interface->SetLength(0);
  return interface.As<v8::Template>();
}

void Global::InstallInterface(v8::Isolate* isolate,
                              v8::Local<v8::Template> interface) {
  v8::Local<v8::FunctionTemplate> function_template =
      interface.As<v8::FunctionTemplate>();
  v8::Local<v8::ObjectTemplate> prototype_template =
      function_template->PrototypeTemplate();
  v8::Local<v8::ObjectTemplate> instance_template =
      function_template->InstanceTemplate();

  prototype_template->Set(v8::Symbol::GetToStringTag(isolate),
                          v8::String::NewFromUtf8Literal(isolate, "Global"));
  instance_template->SetImmutableProto();
  instance_template->SetInternalFieldCount(1);

  v8::Local<v8::Signature> signature =
      v8::Signature::New(isolate, function_template);
  v8::Local<v8::FunctionTemplate> atob_function_template =
      v8::FunctionTemplate::New(isolate, AtobOperatorCallback,
                                v8::Local<v8::Value>(), signature, 1,
                                v8::ConstructorBehavior::kThrow);
  atob_function_template->SetClassName(
      v8::String::NewFromUtf8(isolate, "atob").ToLocalChecked());
  instance_template->Set(isolate, "atob", atob_function_template);

  v8::Local<v8::FunctionTemplate> btoa_function_template =
      v8::FunctionTemplate::New(isolate, BtoaOperatorCallback,
                                v8::Local<v8::Value>(), signature, 1,
                                v8::ConstructorBehavior::kThrow);
  btoa_function_template->SetClassName(
      v8::String::NewFromUtf8(isolate, "btoa").ToLocalChecked());
  instance_template->Set(isolate, "btoa", btoa_function_template);

  v8::Local<v8::FunctionTemplate> read_file_sync_function_template =
      v8::FunctionTemplate::New(isolate, ReadFileSyncOperatorCallback,
                                v8::Local<v8::Value>(), signature, 1,
                                v8::ConstructorBehavior::kThrow);
  read_file_sync_function_template->SetClassName(
      v8::String::NewFromUtf8(isolate, "readFileSync").ToLocalChecked());
  instance_template->Set(isolate, "readFileSync",
                         read_file_sync_function_template);

  v8::Local<v8::FunctionTemplate> load_module_function_template =
      v8::FunctionTemplate::New(isolate, LoadModuleOperatorCallback,
                                v8::Local<v8::Value>(), signature, 1,
                                v8::ConstructorBehavior::kThrow);
  load_module_function_template->SetClassName(
      v8::String::NewFromUtf8(isolate, "loadModule").ToLocalChecked());
  instance_template->Set(isolate, "loadModule", load_module_function_template);

  // instance_template->SetLazyDataProperty(v8::String::NewFromUtf8(isolate,
  // "Global").ToLocalChecked(), )
}
}  // namespace rs::binding
