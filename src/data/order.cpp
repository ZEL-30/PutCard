#include "order.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

Order::Order(QString filename)
    : filename_(filename) {
    load();
}

Order::~Order() {}

void Order::print() const {
    for (auto order : orders_) {
        printf("order_name: %s, check_num: %d, create_time: %s\n", order.order_name.toStdString().c_str(),
               order.check_num, order.create_time.toStdString().c_str());
    }
}

const Order::OrderVector &Order::orders() const { return orders_; }

const QStringList &Order::nameList() {
    name_list_.clear();
    for (auto order : orders_) {
        name_list_.push_back(order.order_name);
    }
    return name_list_;
}

bool Order::currentOrder(const QString &current_order) {
    for (auto order : orders_) {
        if (order.order_name == current_order) {
            current_order_ = order;
            return true;
        }
    }
    return false;
}

const Order::OrderInfo &Order::currentOrder() const { return current_order_; }

bool Order::add(const OrderInfo &order_info) {
    orders_.push_back(order_info);
    return save();
}

bool Order::remove(const QString &order_name) {
    for (auto it = orders_.begin(); it != orders_.end(); ++it) {
        if (it->order_name == order_name) {
            orders_.erase(it);
            break;
        }
    }

    return save();
}

bool Order::clear() {
    orders_.clear();
    return save();
}

bool Order::update(const QString &order_name, const OrderInfo &order_info) {
    for (auto it = orders_.begin(); it != orders_.end(); ++it) {
        if (it->order_name == order_name) {
            *it = order_info;
            break;
        }
    }

    return save();
}

bool Order::load() {
    QFile file(filename_);
    if (!file.open(QIODevice::ReadOnly)) {
        printf("open file %s failed\n", filename_.toStdString().c_str());
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError json_error;
    QJsonDocument   json_doc(QJsonDocument::fromJson(data, &json_error));
    if (json_error.error != QJsonParseError::NoError) {
        printf("json error\n");
        return false;
    }

    QJsonObject obj = json_doc.object();
    QJsonArray  array;
    if (obj.contains("orders")) {
        array = obj["orders"].toArray();
    }

    for (int i = 0; i < array.size(); ++i) {
        QJsonObject order_obj = array[i].toObject();
        OrderInfo   order_info;
        order_info.order_name  = order_obj["order_name"].toString();
        order_info.check_num   = order_obj["check_num"].toInt();
        order_info.create_time = order_obj["create_time"].toString();
        orders_.push_back(order_info);
    }

    return true;
}

bool Order::save() {
    QJsonObject obj;
    QJsonArray  array;
    for (auto order : orders_) {
        QJsonObject order_obj;
        order_obj.insert("order_name", order.order_name);
        order_obj.insert("check_num", order.check_num);
        order_obj.insert("create_time", order.create_time);
        array.append(order_obj);
    }
    obj.insert("orders", array);

    QJsonDocument json_doc(obj);
    QByteArray    data = json_doc.toJson();

    QFile file(filename_);
    if (!file.open(QIODevice::WriteOnly)) {
        printf("open file %s failed\n", filename_.toStdString().c_str());
        return false;
    }

    file.write(data);
    file.close();

    return true;
}
