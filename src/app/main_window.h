#pragma once
#include "data/order.h"
#include "data/user.h"
#include "ui_main_window.h"

#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    MainWindow(User *user, QMainWindow *parent = nullptr);
    ~MainWindow();

  private:
    /// @brief 初始化窗口
    void initWindow();

    /// @brief 初始化UI
    void initUi();

    /// @brief 初始化主界面Tab
    void initMainTab();

    /// @brief 初始化订单管理Tab
    void initOrderTab();

    /// @brief 初始化用户管理Tab
    void initUserTab();

    /// @brief 初始化表格
    void initTable(QTableWidget *table, QStringList header, int col_count);

    /// @brief 初始化信号槽
    void initSignalSlot();

    /// @brief 选择订单
    void selectOrder();

    void getTargetIccid();

    void getBeforeIccid();

    void getAfterIccid();

    /// @brief 刷新主界面 tab
    void refreshMainTab();

    /// @brief 切换用户按钮点击事件
    void switchUserBtnClicked();

    /// @brief 添加订单按钮点击事件
    void addOrderBtnClicked();

    /// @brief 修改订单按钮点击事件
    void updateOrderBtnClicked();

    /// @brief 删除订单按钮点击事件
    void removeOrderBtnClicked();

    /// @brief 清空订单按照点击事件
    void clearOrderBtnClicked();

    /// @brief 显示选中订单
    void showSelectedOrder();

    /// @brief 刷新订单管理 tab
    void refreshOrderTab();

    /// @brief 添加用户按钮点击事件
    void addUserBtnClicked();

    /// @brief 修改用户按钮点击事件
    void updateUserBtnClicked();

    /// @brief 删除用户按钮点击事件
    void removeUserBtnClicked();

    /// @brief 清空订单按照点击事件
    void clearUserBtnClicked();

    /// @brief 显示选中用户
    void showSelectedUser();

    ////  @brief 刷新用户管理 tab
    void refreshUserTab();

    /// @brief 记录日志
    bool log(const QString &filename, const QString &msg);

  private:
    Ui_MainWindow *ui_;
    User          *user_;
    Order         *order_;

    QString target_iccid_;
    QString before_iccid_;
    QString after_iccid_;
};