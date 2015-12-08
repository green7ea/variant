#pragma once

#include <type_traits>
#include <typeinfo>
#include <functional>
#include <cstring>

template <size_t ...args>
struct static_max;

template <size_t value>
struct static_max<value>
{
    static const size_t result = value;
};

template <size_t value, size_t ...values>
struct static_max<value, values...>
{
    static const size_t result =
        value > static_max<values...>::result ?
        value :
        static_max<values...>::result;
};

template <size_t count, typename ...args>
struct variant_lookup;

template <size_t count, typename Lookup>
struct variant_lookup<count, Lookup>
{
    static const int result = -1;
};

template <size_t count, typename Lookup, typename Candidate, typename ...rest>
struct variant_lookup<count, Lookup, Candidate, rest...>
{
    static const int result = std::is_same<Lookup, Candidate>::value ?
        count :
        variant_lookup<count + 1, Lookup, rest...>::result;
};

template <typename... ContainedTypes>
struct Variant
{
    Variant()
        : type(-1)
    {

    }

    Variant(const Variant &copy)
        : type(copy.type)
    {
        void *origin = &copy.data;
        void *destination = &data;
        memcpy(destination, origin, data_size);
    }

    Variant(Variant &&) = delete;

    template <typename Type>
    explicit Variant(const Type &value)
        : type(variant_lookup<0, Type, ContainedTypes...>::value)
    {
        init(value);
        static_cast<Type>(data) = value;
    }

    ~Variant()
    {
        destroy();
    }

    template <typename Type>
    void init(const Type &value)
    {
        Type *ptr = reinterpret_cast<Type *>(&data);
        new (ptr) Type(value);
    }

    void destroy()
    {
        static std::function<void (void *)> destructors[] =
            { build_destructor<ContainedTypes>()... };

        if (type < 0) return;

        destructors[type](&data);
        type = -1;
    }

    Variant<ContainedTypes...> & operator=(const Variant &copy)
    {
        destroy();
        type = copy.type;
        void *origin = &copy.data;
        void *destination = &data;
        memcpy(destination, origin, data_size);

        return *this;
    }

    Variant<ContainedTypes...> & operator=(Variant &&) = delete;

    template <typename Type>
    Variant<ContainedTypes...> & operator=(const Type &value)
    {
        destroy();
        type = variant_lookup<0, Type, ContainedTypes...>::result;

        init(value);

        return *this;
    }

    template <typename Evaluator>
    void apply(Evaluator &value)
    {
        static std::function<void (Evaluator &, void *)> dispatchers[] =
            { build_dispatch<Evaluator, ContainedTypes>()... };

        dispatchers[type](value, &data);
    }

    template <typename Evaluator, typename Target>
    std::function<void (Evaluator &, void *)> build_dispatch()
    {
        return [](Evaluator &functor, void *data)
        {
            auto *v = reinterpret_cast<Target *>(data);

            functor.operator()(*v);
        };
    }

    template <typename Target>
    std::function<void (void *)> build_destructor()
    {
        return [](void *data)
        {
            auto *v = reinterpret_cast<Target *>(data);

            v->~Target();
        };
    }

    static const size_t data_size = static_max<sizeof(ContainedTypes)...>::result;
    static const size_t data_align = static_max<alignof(ContainedTypes)...>::result;
    using Storage = typename std::aligned_storage<data_size, data_align>::type;

    int type;
    Storage data;
};
