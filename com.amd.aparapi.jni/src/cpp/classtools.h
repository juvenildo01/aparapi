#ifndef CLASSTOOLS_H
#define CLASSTOOLS_H

#include <stdio.h>
#include <stdlib.h>

enum ConstantPoolType {
   EMPTY, //0
   UTF8, //1
   UNICODE, //2
   INTEGER, //3
   FLOAT, //4
   LONG, //5
   DOUBLE, //6
   CLASS, //7
   STRING, //8
   FIELD, //9
   METHOD, //10
   INTERFACEMETHOD, //11
   NAMEANDTYPE, //12
   UNUSED13,
   UNUSED14,
   METHODHANDLE, //15
   METHODTYPE, //16
   UNUSED17,
   INVOKEDYNAMIC //18
};

typedef unsigned char  byte_t;
typedef unsigned char  u1_t;
typedef unsigned short u2_t;
typedef signed int     s4_t;
typedef unsigned int   u4_t;
typedef unsigned long  u8_t;
typedef signed long    s8_t;
typedef float          f4_t;
typedef double         f8_t;

union u4s4f4_u {
   u4_t u4;
   s4_t s4;
   f4_t f4;
};
union u8s8f8_u {
   u8_t u8;
   s8_t s8;
   f8_t f8;
};

class ByteBuffer{
   private:
      size_t len;
      byte_t *bytes;
      byte_t *ptr;
      u1_t u1(byte_t *ptr);
      u2_t u2(byte_t *ptr);
      u4_t u4(byte_t *ptr);
      s4_t s4(byte_t *ptr);
      f4_t f4(byte_t *ptr);
      u8_t u8(byte_t *ptr);
      s8_t s8(byte_t *ptr);
      f8_t f8(byte_t *ptr);
   public:
      ByteBuffer(byte_t *_bytes, size_t _len);
      ~ByteBuffer();
      byte_t *getBytes();
      size_t getLen();
      u1_t u1();
      u2_t u2();
      u4_t u4();
      f4_t f4();
      s4_t s4();
      u8_t u8();
      f8_t f8();
      s8_t s8();
      char *createUTF8(int _len);
      //byte_t *getPtrAndBump(int _len);
      ByteBuffer *getByteBuffer(int _len);
      bool isKernel();
};

class ConstantPoolEntry{
   private:
      ConstantPoolType constantPoolType;
   public:
      ConstantPoolEntry(ByteBuffer *_byteBuffer, ConstantPoolType _constantPoolType);
      virtual ~ConstantPoolEntry();
      ConstantPoolType getConstantPoolType() ;
};

class EmptyConstantPoolEntry : public ConstantPoolEntry{
   public:
      EmptyConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~EmptyConstantPoolEntry();
};

class UTF8ConstantPoolEntry: public ConstantPoolEntry{
   private:
      size_t len; 
      char  *utf8;   // allocated during construction!
   public:
      UTF8ConstantPoolEntry(ByteBuffer *_byteBuffer) ;
      ~UTF8ConstantPoolEntry() ;
      size_t getLen();
      char *getUTF8();
      void write(FILE *file);
};

class IntegerConstantPoolEntry : public ConstantPoolEntry{
   private:
      s4_t value;
   public:
      IntegerConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~IntegerConstantPoolEntry();
      s4_t getValue();
};

class FloatConstantPoolEntry : public ConstantPoolEntry{
   private:
      f4_t value;
   public:
      FloatConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~FloatConstantPoolEntry();
      f4_t getValue();
};

class DoubleConstantPoolEntry : public ConstantPoolEntry{
   private:
      f8_t value;
   public:
      DoubleConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~DoubleConstantPoolEntry();
      f8_t getValue();
};

class LongConstantPoolEntry : public ConstantPoolEntry{
   private:
      s8_t value;
   public:
      LongConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~LongConstantPoolEntry();
      s8_t getValue();
};

class ClassConstantPoolEntry : public ConstantPoolEntry{
   private:
      u2_t nameIndex;
   public:
      ClassConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~ClassConstantPoolEntry();
      u2_t getNameIndex();
};


class ReferenceConstantPoolEntry : public ConstantPoolEntry{
   protected:
      u2_t referenceClassIndex;
      u2_t nameAndTypeIndex;
   public:
      ReferenceConstantPoolEntry(ByteBuffer *_byteBuffer, ConstantPoolType _constantPoolType);
      ~ReferenceConstantPoolEntry();
      u2_t getReferenceClassIndex();
      u2_t getNameAndTypeIndex();
};

class FieldConstantPoolEntry : public ReferenceConstantPoolEntry{
   public:
      FieldConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~FieldConstantPoolEntry();
};
class MethodReferenceConstantPoolEntry : public ReferenceConstantPoolEntry{
   public:
      MethodReferenceConstantPoolEntry(ByteBuffer *_byteBuffer, ConstantPoolType _constantPoolType);
      ~MethodReferenceConstantPoolEntry();
};

class MethodConstantPoolEntry : public MethodReferenceConstantPoolEntry{
   public:
      MethodConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~MethodConstantPoolEntry();
};

class InterfaceMethodConstantPoolEntry : public MethodReferenceConstantPoolEntry{
   public:
      InterfaceMethodConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~InterfaceMethodConstantPoolEntry();
};

class NameAndTypeConstantPoolEntry : public ConstantPoolEntry{
   protected:
      u2_t descriptorIndex;
      u2_t nameIndex;
   public:
      NameAndTypeConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~NameAndTypeConstantPoolEntry();
      u2_t getDescriptorIndex();
      u2_t getNameIndex();
};


class MethodTypeConstantPoolEntry : public ConstantPoolEntry{
   protected:
      u2_t descriptorIndex;

   public:
      MethodTypeConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~MethodTypeConstantPoolEntry();
      u2_t getDescriptorIndex();
};

class MethodHandleConstantPoolEntry : public ConstantPoolEntry{
   protected:
      u1_t referenceKind;
      u2_t referenceIndex;
   public:
      MethodHandleConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~MethodHandleConstantPoolEntry();
      u1_t getReferenceKind();
      u2_t getReferenceIndex();
};

class StringConstantPoolEntry : public ConstantPoolEntry{
   protected:
      u2_t utf8Index;

   public:
      StringConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~StringConstantPoolEntry();
      u2_t getUtf8Index();
};

class InvokeDynamicConstantPoolEntry : public ConstantPoolEntry{
   protected:
      u2_t bootstrapMethodAttrIndex;
      u2_t nameAndTypeIndex;

   public:
      InvokeDynamicConstantPoolEntry(ByteBuffer *_byteBuffer);
      ~InvokeDynamicConstantPoolEntry();
      u2_t getBootStrapMethodAttrIndex();
      u2_t getNameAndTypeIndex();
};

class AttributeInfo; // forward

class LineNumberTableAttribute{
   class LineNumberTableEntry{
   private:
     u2_t start_pc;
     u2_t line_number; 
   public:
      LineNumberTableEntry(ByteBuffer *_byteBuffer, ConstantPoolEntry **_constantPool);
      ~LineNumberTableEntry();
   };
  private:
    u2_t line_number_table_length;
    LineNumberTableEntry **lineNumberTable;
  public:
      LineNumberTableAttribute(ByteBuffer *_byteBuffer, ConstantPoolEntry **_constantPool);
      ~LineNumberTableAttribute();
};

class LocalVariableTableAttribute{
   class LocalVariableTableEntry{
   private:
     u2_t start_pc;
     u2_t length; 
     u2_t name_index; 
     u2_t descriptor_index; 
     u2_t index; 
   public:
      LocalVariableTableEntry(ByteBuffer *_byteBuffer, ConstantPoolEntry **_constantPool);
      ~LocalVariableTableEntry();
   };
  private:
    u2_t local_variable_table_length;
    LocalVariableTableEntry **localVariableTable;
  public:
      LocalVariableTableAttribute(ByteBuffer *_byteBuffer, ConstantPoolEntry **_constantPool);
      ~LocalVariableTableAttribute();
};

class CodeAttribute{
   class ExceptionTableEntry{
   private:
     u2_t start_pc;
     u2_t end_pc;
     u2_t handler_pc;
     u2_t catch_type;

   public:
      ExceptionTableEntry(ByteBuffer *_byteBuffer, ConstantPoolEntry **_constantPool);
      ~ExceptionTableEntry();
   };
   private:
      u2_t max_stack;
      u2_t max_locals;
      ByteBuffer *codeByteBuffer;
      u2_t exception_table_length;
      ExceptionTableEntry **exceptionTable;
      u2_t attributes_count;
      AttributeInfo **attributes;
   public:
      CodeAttribute(ByteBuffer *_byteBuffer, ConstantPoolEntry **_constantPool);
      ~CodeAttribute();
      ByteBuffer *getCodeByteBuffer();
};

enum AttributeType{
   UNKNOWN, 
   Code,
   LineNumberTable,
   LocalVariableTable
};

class AttributeInfo{
   private:
      u2_t attribute_name_index;
      ByteBuffer *infoByteBuffer;
      AttributeType attribute_type;
      union{
         CodeAttribute *codeAttribute;
         LineNumberTableAttribute *lineNumberTableAttribute;
         LocalVariableTableAttribute *localVariableTableAttribute;
      };
   public:
      AttributeInfo(ByteBuffer *_byteBuffer, ConstantPoolEntry **_constantPool);
      ~AttributeInfo();
      u2_t getAttributeNameIndex();
      AttributeType getAttributeType();
      CodeAttribute *getCodeAttribute();
      LineNumberTableAttribute *getLineNumberTableAttribute();
      LocalVariableTableAttribute *getLocalVariableTableAttribute();
};

class FieldInfo{
   private:
      u2_t access_flags;
      u2_t name_index;
      u2_t descriptor_index;
      u2_t attributes_count;
      AttributeInfo **attributes;
   public:
      FieldInfo(ByteBuffer *_byteBuffer, ConstantPoolEntry **_constantPool);
      ~FieldInfo();
      u2_t getNameIndex();
      u2_t getDescriptorIndex();
};

class MethodInfo{
   private:
      u2_t access_flags;
      u2_t name_index;
      u2_t descriptor_index;
      u2_t attributes_count;
      AttributeInfo **attributes;
      CodeAttribute *codeAttribute;
      LocalVariableTableAttribute *localVariableTableAttribute;
      LineNumberTableAttribute *lineNumberTableAttribute;
   public:
      MethodInfo(ByteBuffer *_byteBuffer, ConstantPoolEntry **_constantPool);
      ~MethodInfo();
      u2_t getNameIndex();
      u2_t getDescriptorIndex();
      CodeAttribute *getCodeAttribute();
      LocalVariableTableAttribute *getLocalVariableTableAttribute();
      LineNumberTableAttribute *getLineNumberTableAttribute();
};

class ClassInfo{
   private:
      u4_t magic;
      u2_t minor;
      u2_t major;
      u2_t constantPoolSize;
      ConstantPoolEntry **constantPool;
      u2_t accessFlags;
      u2_t thisClassConstantPoolIndex;
      u2_t superClassConstantPoolIndex;
      u2_t interfaceCount;
      u2_t *interfaces;
      u2_t fieldCount;
      FieldInfo **fields;
      u2_t methodCount;
      MethodInfo **methods;
      u2_t attributeCount;
      AttributeInfo **attributes;
   public:
      ClassInfo(ByteBuffer *_byteBuffer);
      ~ClassInfo();
      // com/amd/aparapi/Main$Kernel.run()V ==  "run", "()V"
      MethodInfo *getMethodInfo(char *_methodName, char *_methodDescriptor); // com/amd/aparapi/Main$Kernel.run()V
      char *getSuperClassName();
      char *getClassName();
};

#endif
