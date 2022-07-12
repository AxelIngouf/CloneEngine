#pragma once

#include "FuncMeta.h"
#include "StructureField.h"

namespace Core
{
    class StructBuilder
    {
    public:
        StructBuilder(const char* pName, int pSize);
        ~StructBuilder() = default;

        StructBuilder() = delete;
        StructBuilder(const StructBuilder& other) = delete;
        StructBuilder(StructBuilder&& other) = delete;
        StructBuilder& operator=(const StructBuilder& other) = delete;
        StructBuilder& operator=(StructBuilder&& other) = delete;

        StructBuilder& AddBase(const char* pType, int pOffset);
        StructBuilder& AddField(const char* pType, const char* pName, int pOffset, FieldFlag pFlags = FieldFlag::NONE);
        StructBuilder& AddStaticField(const char* pType, const char* pName, void* pPtr);
        StructBuilder& AddFunction(Function* pFunction);
        StructBuilder& SetConstructor(Function* pFunction);
        class Structure* Finish() { return m_struct; }

    private:
        Structure* m_struct;
        bool m_alreadyExist;
    };



    class Structure : public Type
    {
    public:
        Structure(const char* pName, int pSize) noexcept :
            Type(pName, ETypeCategory::STRUCTURE, pSize, -1),
            m_constructor(nullptr)
        {}
        ~Structure() = default;

        Structure() = delete;
        Structure(const Structure& other) = delete;
        Structure(Structure&& other) = delete;
        Structure& operator=(const Structure& other) = delete;
        Structure& operator=(Structure&& other) = delete;

        const std::string& GetName() const { return name.text; }

        ArrayView<StructureBase> GetBases() const { return m_bases; }
        bool Inherit(const char* pName) const { return Inherit(ConstexprCustomHash(pName)); } // todo : switch to CustomHash when ConstexprCustomHash and CustomHash are equivalent

        StructureField FindField(const char* pName) const { return FindField(ConstexprCustomHash(pName)); } // todo : switch to CustomHash when ConstexprCustomHash and CustomHash are equivalent
        //ArrayView<Field> GetFields() const { return ArrayView (m_fields); } // todo : remove if no usage is found
        FieldIterable GetAllFields() const { return FieldIterable(this); }

        const StaticField* FindStaticField(const char* pName) const { return FindStaticField(ConstexprCustomHash(pName)); }

        void* New() const { return *(void**)m_constructor->Invoke(nullptr).Data; }
        const Function* FindFunction(const char* pName) const { return FindFunction(ConstexprCustomHash(pName)); } // todo : switch to CustomHash when ConstexprCustomHash and CustomHash are equivalent

        //ArrayView<const Function*> GetFunctions() const { return ArrayView(m_functions); }

    private:
        friend StructBuilder;
        friend FieldIterator;
        friend void InitStructureReflection(class Structure* type);

        bool Inherit(long long pHash) const;
        StructureField FindField(long long pHash) const;
        const StaticField* FindStaticField(long long pHash) const;
        const Function* FindFunction(long long pName) const;

        std::vector<StructureBase> m_bases;
        std::vector<Field> m_fields;
        std::vector<StaticField> m_staticFields;
        std::vector<Function*> m_functions;
        Function* m_constructor;

    };
}

#define CHOOSE_AND_UNFOLD(choice, first, ...)\
    DEFER1(EVAL1)(CAT(choice, first))/*\n*/\
    IF_ELSE(HAS_ARGS(__VA_ARGS__))\
        (DEFER2(_CHOOSE_AND_UNFOLD)()(choice, __VA_ARGS__))\
    /*ELSE*/\
        (/* Do nothing, just terminate */)

#define _CHOOSE_AND_UNFOLD() CHOOSE_AND_UNFOLD



#define _TEST_FUNCTION(__struct_name__, choice, __function_args__)\
    IF_ELSE(HAS_ARGS(__function_args__))\
		(DEFER2(CAT(choice, _BODY))(__struct_name__, __function_args__))\
    /*else do nothing, just skip */()



#define ADD_AND_UNFOLD(__struct_name__, choice, first, ...)\
    _TEST_FUNCTION(__struct_name__, choice, EVAL1(CAT(choice, first)))\
    IF_ELSE(HAS_ARGS(__VA_ARGS__))\
        (DEFER2(_ADD_AND_UNFOLD)()(__struct_name__, choice, __VA_ARGS__))\
    /*ELSE*/(/*terminate*/)

#define _ADD_AND_UNFOLD() ADD_AND_UNFOLD



// todo: macro support for constructor
// todo: macro support for const field
// todo: macro support for static field
// todo: better support for field default value
// todo: support namespace



#define FIELD(__field_type__, __field_name__, ...) _FIELD(__field_type__, __field_name__, __VA_ARGS__)
#define _BASE_FIELD(...)
#define _DATA_FIELD(__field_type__, __field_name__, ...) public: __field_type__ __field_name__;
#define _FUNC_FIELD(...)
#define _BASE_META_FIELD(...)
#define _FIELD_META_FIELD(...) __VA_ARGS__
#define _STATIC_META_FIELD(...)
#define _FUNCTION_META_FIELD(...)
#define _GENERAL_META_FIELD(...)

#define PRIVATE_FIELD(__field_type__, __field_name__, ...) _PRIVATE_FIELD(__field_type__, __field_name__, __VA_ARGS__)
#define _BASE_PRIVATE_FIELD(...)
#define _DATA_PRIVATE_FIELD(__field_type__, __field_name__, ...) private: __field_type__ __field_name__;
#define _FUNC_PRIVATE_FIELD(...)
#define _BASE_META_PRIVATE_FIELD(...)
#define _FIELD_META_PRIVATE_FIELD(...) __VA_ARGS__
#define _STATIC_META_PRIVATE_FIELD(...)
#define _FUNCTION_META_PRIVATE_FIELD(...)
#define _GENERAL_META_PRIVATE_FIELD(...)

#define _FIELD_META_BODY(__struct_name__, __field_type__, __field_name__, ...)\
    IF_ELSE(HAS_ARGS(__VA_ARGS__))\
        (.AddField(STRINGYFY(__field_type__), STRINGYFY(__field_name__), (int)(long long)&((__struct_name__*)0)->__field_name__, __VA_ARGS__))\
    /*ELSE*/\
        (.AddField(STRINGYFY(__field_type__), STRINGYFY(__field_name__), (int)(long long)&((__struct_name__*)0)->__field_name__))



#define STATIC_FIELD(__field_type__, __field_name__, ...) _STATIC_FIELD(__field_type__, __field_name__, __VA_ARGS__)
#define _BASE_STATIC_FIELD(...)
#define _DATA_STATIC_FIELD(__field_type__, __field_name__, ...)\
    IF_ELSE(HAS_ARGS(__VA_ARGS__))\
        (public: inline static __field_type__ __field_name__ = __VA_ARGS__;)\
    /*ELSE*/\
        (public: static __field_type__ __field_name__)
#define _FUNC_STATIC_FIELD(...)
#define _BASE_META_STATIC_FIELD(...)
#define _FIELD_META_STATIC_FIELD(...)
#define _STATIC_META_STATIC_FIELD(__field_type__, __field_name__, ...) __field_type__, __field_name__
#define _FUNCTION_META_STATIC_FIELD(...)
#define _GENERAL_META_STATIC_FIELD(...)

#define _STATIC_META_BODY(__struct_name__, __field_type__, __field_name__) .AddStaticField(STRINGYFY(__field_type__), STRINGYFY(__field_name__), &__struct_name__::__field_name__)



#define BASE(__base_type__) _BASE(__base_type__)
#define _BASE_BASE(__base_type__) : public __base_type__
#define _DATA_BASE(...)
#define _FUNC_BASE(...)
#define _BASE_META_BASE(__base_type__) __base_type__
#define _FIELD_META_BASE(...)
#define _STATIC_META_BASE(...)
#define _FUNCTION_META_BASE(...)
#define _GENERAL_META_BASE(...)



#define OTHER_BASE(__base_type__) _OTHER_BASE(__base_type__)
#define _BASE_OTHER_BASE(__base_type__) , public __base_type__
#define _DATA_OTHER_BASE(...)
#define _FUNC_OTHER_BASE(...)
#define _BASE_META_OTHER_BASE(__base_type__) __base_type__
#define _FIELD_META_OTHER_BASE(...)
#define _STATIC_META_OTHER_BASE(...)
#define _FUNCTION_META_OTHER_BASE(...)
#define _GENERAL_META_OTHER_BASE(...)



#define BASE2(__base_type__, __base_meta_name__) _BASE2(__base_type__, __base_meta_name__)
#define _BASE_BASE2(__base_type__, __base_meta_name__) : public __base_type__
#define _DATA_BASE2(...)
#define _FUNC_BASE2(...)
#define _BASE_META_BASE2(__base_type__, __base_meta_name__) __base_type__, __base_meta_name__
#define _FIELD_META_BASE2(...)
#define _STATIC_META_BASE2(...)
#define _FUNCTION_META_BASE2(...)
#define _GENERAL_META_BASE2(...)

#define CRTP_BASE(__struct_name__, __base_type__) _CRTP_BASE(__struct_name__, __base_type__)
#define _BASE_CRTP_BASE(__struct_name__, __base_type__) : public __base_type__<__struct_name__>
#define _DATA_CRTP_BASE(...)
#define _FUNC_CRTP_BASE(...)
#define _BASE_META_CRTP_BASE(__struct_name__, __base_type__) __base_type__<__struct_name__>, __base_type__
#define _FIELD_META_CRTP_BASE(...)
#define _STATIC_META_CRTP_BASE(...)
#define _FUNCTION_META_CRTP_BASE(...)
#define _GENERAL_META_CRTP_BASE(...)



#define _BASE_META_BODY(__struct_name__, __base_type__, ...)\
    IF_ELSE(HAS_ARGS(__VA_ARGS__))\
        (.AddBase(STRINGYFY(__VA_ARGS__), (int)(long long)(__base_type__*)((__struct_name__*)8) - 8))\
    /*ELSE*/\
        (.AddBase(STRINGYFY(__base_type__), (int)(long long)(__base_type__*)((__struct_name__*)8) - 8))






#define SUPPLEMENT(__supplement_base__, __supplement_data__, __supplement_meta__) _SUPPLEMENT(__supplement_base__, __supplement_data__, __supplement_meta__)
#define _BASE_SUPPLEMENT(__supplement_base__, __supplement_data__, __supplement_meta__) __supplement_base__
#define _DATA_SUPPLEMENT(__supplement_base__, __supplement_data__, __supplement_meta__) __supplement_data__
#define _FUNC_SUPPLEMENT(...)
#define _BASE_META_SUPPLEMENT(...)
#define _FIELD_META_SUPPLEMENT(...)
#define _STATIC_META_SUPPLEMENT(...)
#define _FUNCTION_META_SUPPLEMENT(...)
#define _GENERAL_META_SUPPLEMENT(__supplement_base__, __supplement_data__, __supplement_meta__) __supplement_meta__



#define STRUCT(__struct_name__, ...)\
namespace Reflection/*\n*/\
{/*\n*/\
	Core::Structure* CAT(__struct_name__, Meta)();/*\n*/\
}/*\n*/\
/*\n*/\
struct __struct_name__ EVAL(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(CHOOSE_AND_UNFOLD)(_BASE, __VA_ARGS__)) (/*inside else*/))/*\n*/\
{/*\n*/\
    EVAL(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(CHOOSE_AND_UNFOLD)(_DATA, __VA_ARGS__)) (/*inside else*/))\
/*\n*/\
private:/*\n*/\
    struct ConstructorObject : public Core::Function {/*\n*/\
        ConstructorObject() : Function("Constructor", "void*") {}/*\n*/\
        void InvokeImpl(void*) const override {/*\n*/\
            *(void**)buffer = new __struct_name__();/*\n*/\
        }/*\n*/\
    };/*\n*/\
    EVAL(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(ADD_AND_UNFOLD)(__struct_name__, _FUNC, __VA_ARGS__)) (/*inside else*/))\
/*\n*/\
public:/*\n*/\
    friend Core::Structure* Reflection::CAT(__struct_name__, Meta)(); /*\n*/\
    inline static const Core::Structure* const MetaData = Reflection::CAT(__struct_name__, Meta)(); /*\n*/\
};/*\n*/\
/*\n*/\
inline Core::Structure* Reflection::CAT(__struct_name__, Meta)()/*\n*/\
{/*\n*/\
    return Core::StructBuilder(STRINGYFY(__struct_name__), sizeof(__struct_name__))/*\n*/\
        EVAL(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(ADD_AND_UNFOLD)(__struct_name__, _STATIC_META, __VA_ARGS__)) (/*inside else*/))\
        EVAL(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(ADD_AND_UNFOLD)(__struct_name__, _FIELD_META, __VA_ARGS__)) (/*inside else*/))\
        EVAL(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(ADD_AND_UNFOLD)(__struct_name__, _BASE_META, __VA_ARGS__)) (/*inside else*/))\
        EVAL(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(ADD_AND_UNFOLD)(__struct_name__, _FUNCTION_META, __VA_ARGS__)) (/*inside else*/))\
        EVAL(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(CHOOSE_AND_UNFOLD)(_GENERAL_META, __VA_ARGS__)) (/*inside else*/))\
        .SetConstructor(new __struct_name__::ConstructorObject())/*\n*/\
        .Finish();/*\n*/\
}