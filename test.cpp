#include "variant.hpp"

#include <vector>
#include <string>

struct Validation
{
    Validation()
        : current(++value)
    {
        printf("\nValidation() %i\n", current);
    }

    Validation(const Validation &)
        : current(++value)
    {
        printf("Copy() %i\n", current);
    }

    ~Validation()
    {
        printf("~Validation() %i\n", current);
        current = -1;
    }

    void test()
    {
        if (current < 0)
        {
            printf("Invalid\n");
        }
    }

    static int value;
    int current;
};

int Validation::value = 0;

struct print
{
    template <typename ...args>
    void operator()(Variant<args...> &variant)
    {
        variant.apply(*this);
    }

    void operator()(int value)
    {
        printf("int %i\n", value);
    }

    void operator()(const std::string &value)
    {
        printf("string \"%s\"\n", value.c_str());
    }

    void operator()(float value)
    {
        printf("float %f\n", value);
    }

    void operator()(Validation &t)
    {
        printf("validation\n");
        t.test();
    }
};

int main(int, char **)
{
    using Test = Variant<int, std::string, float, Validation>;
    Test test;

    print disp;

    test = std::string("five");
    test.apply(disp);

    test = 5;
    test.apply(disp);

    test = 5.f;
    test.apply(disp);

    test = Validation();
    test.apply(disp);

    test = Validation();
    test.apply(disp);

    return 0;
}
