
## HOW TO DO

### Parse most of classfile

TODO
- Necessary attributes
  - StackMapTable
  - RuntimeVisibleAnnotations 
  - RuntimeInvisibleAnnotations 
  - RuntimeVisibleParameterAnnotations 
  - RuntimeInvisibleParameterAnnotations 
  - RuntimeVisibleTypeAnnotations 
  - RuntimeInvisibleTypeAnnotations 
  - AnnotationDefault
  - MethodParameters


- Constant pool
- Instructions
- Resolve branch targets

### Basic interpreter

- Operand stack (64 bits per entry)
- Local variables (64 bits per entry)
- Implement each instruction except for function calls etc.
- "Shadow stack" that can be walked to get the stack trace
- Each function's actual return m_value is some info on its execution (e.g. whether it completed abruptly)

### Bootstrap class loader

