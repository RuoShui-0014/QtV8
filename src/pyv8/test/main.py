from pyv8 import V8Isolate

isolate = V8Isolate()
isolate.notify_gc()
a = [isolate]
a.append(a)

print(V8Isolate)
print(isolate)