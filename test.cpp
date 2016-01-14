#include "variant.hpp"
#include "variant_utils.hpp"

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
        printf("\nValidation(const Validation &) %i\n", current);
    }

    Validation(Validation &&)
        : current(++value)
    {
        printf("\nValidation(Validation &&) %i\n", current);
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
    void operator()(Variant<args...> &variant) const
    {
        variant.apply(*this);
    }

    void operator()(int value) const
    {
        printf("int %i\n", value);
    }

    void operator()(const std::string &value) const
    {
        printf("string \"%s\"\n", value.c_str());
    }

    void operator()(float value) const
    {
        printf("float %f\n", value);
    }

    void operator()(Validation &t) const
    {
        printf("Validation operator()\n");
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

    printf("\n");

    /*
     * This code creates a core dump when trying to free an
     * invalid pointer.
     *
    auto vec = make_var_vector(std::string("five"), 5, 5.f);
    for (const auto &item: vec)
    {
        item.apply(disp);
    }
    */

    printf("\n");

    auto array = make_var_array(std::string("five"), 5, 5.f);
    for (const auto &item: array)
    {
        item.apply(disp);
    }

    return 0;
}
