## In-browser JVM

Goal is to make sufficient infrastructure to interpret and partially JIT-compile an OpenJDK 8 environment in the browser.

Name undecided.

### Parse most of classfile

- Attributes left
  - StackMapTable
  - RuntimeVisibleAnnotations 
  - RuntimeInvisibleAnnotations 
  - RuntimeVisibleParameterAnnotations 
  - RuntimeInvisibleParameterAnnotations 
  - RuntimeVisibleTypeAnnotations 
  - RuntimeInvisibleTypeAnnotations 
  - AnnotationDefault
  - MethodParameters

### Basic interpreter

- Operand stack (64 bits per entry)
- Local variables (64 bits per entry)
- Implement each instruction except for function calls etc.
- "Shadow stack" that can be walked to get the stack trace
- Each function's actual return m_value is some info on its execution (e.g. whether it completed abruptly)

### Bootstrap class loader


### Possible WASM64 incompatibilities

- Layout difference in `HeapObject`