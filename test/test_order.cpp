#include "order.h"

#include <QDateTime>

int main() {

    Order order;

    printf("===== test add =====\n");
    // 当前时间
    QDateTime current_date_time = QDateTime::currentDateTime();
    order.add({"order1", 1, current_date_time.toString("yyyy-MM-dd hh:mm:ss")});
    order.add({"order2", 2, current_date_time.toString("yyyy-MM-dd hh:mm:ss")});
    order.add({"order3", 3, current_date_time.toString("yyyy-MM-dd hh:mm:ss")});
    order.print();

    printf("===== test remove =====\n");
    order.remove("order2");
    order.print();

    QString str = "8994029703199000003";
    printf("%s\n", str.toStdString().c_str());
    printf("%s\n", str.left(5).toStdString().c_str());
    printf("%s\n", str.mid(5, 18  - 5).toStdString().c_str());




    return 0;
}