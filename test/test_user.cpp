#include "user.h"

int main() {

    User user("data/test_user.json");
    printf("===== test user list =====\n");
    user.print();

    printf("===== test add =====\n");
    User::UserInfo user1 = {"张恩乐", "!Qw2!Qw2", User::Admin};
    User::UserInfo user2 = {"zel", "!Qw2!Qw2", User::OrdinaryUser};
    User::UserInfo user3 = {"wzl", "!Qw2!Qw2", User::Admin};
    User::UserInfo user4 = {"库里", "curry30", User::OrdinaryUser};

    user.add(user1);
    user.add(user2);
    user.add(user3);
    user.add(user4);
    user.print();

    printf("===== test remove =====\n");
    user.remove("zel");
    user.print();

    printf("===== test update =====\n");
    User::UserInfo wzy{"王子怡", "wzy", User::Admin};

    user.update("wzl", wzy);
    user.print();

    return 0;
}