#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

class Order {

  public:
    struct OrderInfo {
        QString order_name;  // 订单名称
        int     check_num;   // 校验位数
        QString create_time; // 创建时间
    };

    typedef QVector<Order::OrderInfo> OrderVector;

    Order(QString filename = "data/order.json");
    ~Order();

    /// @brief 打印订单列表
    void print() const;

    /// @brief 获取订单列表
    const OrderVector &orders() const;

    /// @brief 获取订单名称列表
    const QStringList &nameList();

    /// @brief 设置当前订单
    bool currentOrder(const QString &current_order);

    /// @brief 获取当前订单
    const OrderInfo &currentOrder() const;

    /// @brief 添加订单
    bool add(const OrderInfo &order_info);

    /// @brief 删除订单
    bool remove(const QString &order_name);

    /// @brief 清空订单
    bool clear();

    /// @brief 修改订单
    bool update(const QString &order_name, const OrderInfo &order_info);

  private:
    /// @brief 加载数据文件
    bool load();

    /// @brief 保存数据文件
    bool save();

  private:
    QString     filename_;
    OrderVector orders_;
    QStringList name_list_;
    OrderInfo   current_order_;
};