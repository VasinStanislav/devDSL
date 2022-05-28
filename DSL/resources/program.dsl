def main_programm(argc, argv) {
    x = 422

    if (argc > x) {
        str = to_string(argc) + " is more than " + to_string(x)
    } elif (argc > 420 ) {
        str = to_string(argc) + " at least is more than 420"
    } elif (argc < x) {
        str = to_string(argc) + " is less than " + to_string(x)
    } else {
        str = to_string(argc) + " is equal with " + to_string(x)
    }

    print(str)

    while (1 > 0) {
        print("1 is more than 0")
        break
    }

    var_1 = 1;      var_2 = 2
    var_1_1 = 3;    var_1_2 = 4
    var_1_1_1 = 5;  var_1_1_2 = 6

    var = (((var_1_1_1 + var_1_1_2) * var_1_1 - var_1_2) + var_2) \ var_1

    return var
}

def get_greeting()  {
    print("We are here")
    return "Hello, interpreter!"
}

x = 7 + 5 * 2

line = ("abc" + "def")
print(line)
is_equal_string = line == "abcdef"

print(input())


for (a = 1; a <= 3; a = a++) {
    x = x + a
    print(to_string(x) + " ")
}

a = 2
if (a > 1) {
    while (a < 10) {
        a = ++ a
    }
}

while (x > 0){
    x = x--
}

x = 2
do {
    if (x < 3) {
        ast_expr = (6 - 5) * 4 + 3 \ 2 + 1
        print(to_string(ast_expr))
    }

    x = x--
} while (x > 0)

print(get_greeting())

print(to_string(main_programm(10, "main")))